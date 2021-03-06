#include "MoveEvent.hpp"

#include "Process/ScenarioModel.hpp"
#include "Document/Event/EventModel.hpp"
#include "Document/TimeNode/TimeNodeModel.hpp"
#include "Process/Algorithms/StandardDisplacementPolicy.hpp"

#include <iscore/document/DocumentInterface.hpp>
#include "Commands/Scenario/Deletions/ClearConstraint.hpp"
#include "Document/Constraint/Rack/RackModel.hpp"
#include "Document/Constraint/Rack/Slot/SlotModel.hpp"

#include "Document/Constraint/ViewModels/AbstractConstraintViewModel.hpp"
#include "Process/Algorithms/StandardCreationPolicy.hpp"
#include "Process/Algorithms/VerticalMovePolicy.hpp"
#include <ProcessInterface/LayerModel.hpp>
using namespace iscore;
using namespace Scenario::Command;

MoveEvent::MoveEvent(ObjectPath&& scenarioPath,
                     id_type<EventModel> eventId,
                     const TimeValue& date,
                     ExpandMode mode) :
    SerializableCommand {"ScenarioControl",
                         commandName(),
                         description()},
    m_path {std::move(scenarioPath)},
    m_eventId {eventId},
    m_newDate {date},
    m_mode{mode}
{
    auto& scenar = m_path.find<ScenarioModel>();
    const auto& movedEvent = scenar.event(m_eventId);
    m_oldDate = movedEvent.date();

    StandardDisplacementPolicy::getRelatedTimeNodes(scenar,
                                                   scenar.event(m_eventId).timeNode(),
                                                   m_movableTimenodes);

    // 1. Make a list of the constraints that need to be resized
    QSet<id_type<ConstraintModel>> constraints;
    for(const auto& tn_id : m_movableTimenodes)
    {
        const auto& tn = scenar.timeNode(tn_id);
        for(const auto& ev_id : tn.events())
        {
            const auto& ev = scenar.event(ev_id);
            for(const auto& st_id : ev.states())
            {
                const auto& st = scenar.state(st_id);
                if(st.previousConstraint())
                    constraints += st.previousConstraint();
            }
        }
    }

    // 2. Save them
    for(const auto& cst_id : constraints)
    {
        const auto& constraint = scenar.constraint(cst_id);

        // Save the constraint data
        QByteArray arr;
        Visitor<Reader<DataStream>> jr{&arr};
        jr.readFrom(constraint);

        // Save for each view model of this constraint
        // the identifier of the rack that was displayed
        QMap<id_type<AbstractConstraintViewModel>, id_type<RackModel>> map;
        for(const AbstractConstraintViewModel* vm : constraint.viewModels())
        {
            map[vm->id()] = vm->shownRack();
        }

        m_savedConstraints.push_back({{iscore::IDocument::path(constraint), arr}, map});
    }
}

void MoveEvent::undo()
{
    auto& scenar = m_path.find<ScenarioModel>();
    auto& event = scenar.event(m_eventId);

    TimeValue deltaDate{};
    deltaDate = m_oldDate - event.date();

    StandardDisplacementPolicy::updatePositions(
                scenar,
                m_movableTimenodes,
                m_oldDate - event.date(),
                [&] (ProcessModel* , const TimeValue& ) { });

    // Now we have to restore the state of each constraint that might have been modified
    // during this command.
    for(auto& obj : m_savedConstraints)
    {
        // 1. Clear the constraint
        auto cmd1 = new ClearConstraint{
                    ObjectPath{obj.first.first}};
        cmd1->redo();

        auto& constraint = obj.first.first.find<ConstraintModel>();
        // 2. Restore the rackes & processes.

        // TODO if possible refactor this with CopyConstraintContent and ConstraintModel::clone
        // Be careful however, the code differs in subtle ways
        {
            ConstraintModel src_constraint{
                    Deserializer<DataStream>{obj.first.second},
                    &constraint}; // Temporary parent

            std::map<const ProcessModel*, ProcessModel*> processPairs;

            // Clone the processes
            for(const auto& sourceproc : src_constraint.processes())
            {
                auto newproc = sourceproc->clone(sourceproc->id(), &constraint);

                processPairs.insert(std::make_pair(sourceproc, newproc));
                constraint.addProcess(newproc);
            }

            // Clone the rackes
            for(const auto& sourcerack : src_constraint.racks())
            {
                // A note about what happens here :
                // Since we want to duplicate our process view models using
                // the target constraint's cloned shared processes (they might setup some specific data),
                // we maintain a pair mapping each original process to their cloned counterpart.
                // We can then use the correct cloned process to clone the process view model.
                auto newrack = new RackModel{
                        *sourcerack,
                        sourcerack->id(),
                        [&] (const SlotModel& source, SlotModel& target)
                        {
                            for(const auto& lm : source.layerModels())
                            {
                                // We can safely reuse the same id since it's in a different slot.
                                ProcessModel* proc = processPairs[&lm->sharedProcessModel()];
                                // TODO harmonize the order of parameters (source first, then new id)
                                target.addLayerModel(proc->cloneLayer(lm->id(), *lm, &target));
                            }
                        },
                        &constraint};
                constraint.addRack(newrack);
            }
        }

        // 3. Restore the correct rackes in the constraint view models
        for(auto& viewmodel : constraint.viewModels())
        {
            viewmodel->showRack(obj.second[viewmodel->id()]);
        }
    }

    updateEventExtent(m_eventId, scenar);
}

void MoveEvent::redo()
{
    auto& scenar = m_path.find<ScenarioModel>();
    auto& event = scenar.event(m_eventId);

    TimeValue deltaDate{};
    deltaDate = m_newDate - event.date();

    StandardDisplacementPolicy::updatePositions(
                scenar,
                m_movableTimenodes,
                deltaDate,
                [&] (ProcessModel* p, const TimeValue& t)
    { p->expandProcess(m_mode, t); });

    updateEventExtent(m_eventId, scenar);
}

void MoveEvent::serializeImpl(QDataStream& s) const
{
    s << m_path
      << m_eventId
      << m_oldDate
      << m_newDate
      << m_movableTimenodes
      << (int)m_mode
      << m_savedConstraints;
}

void MoveEvent::deserializeImpl(QDataStream& s)
{
    int mode;
    s >> m_path
      >> m_eventId
      >> m_oldDate
      >> m_newDate
      >> m_movableTimenodes
      >> mode
      >> m_savedConstraints;

    m_mode = static_cast<ExpandMode>(mode);
}
