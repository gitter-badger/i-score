#include "OSSIAConstraintElement.hpp"
#include <API/Headers/Editor/TimeConstraint.h>
#include <API/Headers/Editor/TimeProcess.h>
#include "../iscore-plugin-scenario/source/Document/Constraint/ConstraintModel.hpp"
#include "OSSIAScenarioElement.hpp"

#include <boost/range/algorithm.hpp>
#include "../iscore-plugin-curve/Automation/AutomationModel.hpp"
#include "OSSIAAutomationElement.hpp"
#include "OSSIAScenarioElement.hpp"
#include "iscore2OSSIA.hpp"
OSSIAConstraintElement::OSSIAConstraintElement(
        std::shared_ptr<OSSIA::TimeConstraint> ossia_cst,
        ConstraintModel& iscore_cst,
        QObject* parent):
    QObject{parent},
    m_iscore_constraint{iscore_cst},
    m_ossia_constraint{ossia_cst}
{
    connect(&iscore_cst, &ConstraintModel::processCreated,
            this, &OSSIAConstraintElement::on_processAdded);
    connect(&iscore_cst, &ConstraintModel::processRemoved,
            this, &OSSIAConstraintElement::on_processRemoved);

    // Setup updates
    // todo : should be in OSSIAConstraintElement
    connect(&iscore_cst, &ConstraintModel::defaultDurationChanged, this,
            [=] (const TimeValue& t) {
        ossia_cst->setDuration(iscore::convert::time(t));
    });
    connect(&iscore_cst, &ConstraintModel::minDurationChanged, this,
            [=] (const TimeValue& t) {
        ossia_cst->setDurationMin(iscore::convert::time(t));
    });
    connect(&iscore_cst, &ConstraintModel::maxDurationChanged, this,
            [=] (const TimeValue& t) {
        ossia_cst->setDurationMax(iscore::convert::time(t));
    });


    for(auto& process : iscore_cst.processes())
    {
        on_processAdded(process->processName(), process->id());
    }
}

std::shared_ptr<OSSIA::TimeConstraint> OSSIAConstraintElement::constraint() const
{
    return m_ossia_constraint;
}

void OSSIAConstraintElement::stop()
{
    m_ossia_constraint->stop();
    for(auto& process : m_processes)
    {
        process.second->stop();
    }

    m_iscore_constraint.reset();
}

void OSSIAConstraintElement::on_processAdded(
        const QString& name,
        const id_type<ProcessModel>& id)
{
    // The DocumentPlugin creates the elements in the processes.
    auto proc = m_iscore_constraint.process(id);
    OSSIAProcessElement* plug{};
    if(auto scenar = dynamic_cast<ScenarioModel*>(proc))
    {
        plug = new OSSIAScenarioElement{this, scenar, proc};
    }
    else if(auto autom = dynamic_cast<AutomationModel*>(proc))
    {
        plug = new OSSIAAutomationElement{this, autom, proc};
    }

    if(plug)
    {
        m_processes.insert({id, plug});

        // i-score scenario has ownership, hence
        // we have to remove it from the array if deleted
        connect(plug, &QObject::destroyed, this,
                [=] (QObject*) {
            // The OSSIA::Process removal is in each process dtor
            m_processes.erase(id);
        }, Qt::DirectConnection);

        // Processes might change (for instance automation needs to be recreated
        // at each address change) so we do this little dance.
        connect(plug, &OSSIAProcessElement::changed,
                this, [=] (auto&& oldProc, auto&& newProc) {
            if(oldProc)
                m_ossia_constraint->removeTimeProcess(oldProc);

            if(newProc)
                m_ossia_constraint->addTimeProcess(plug->process());
        });


        if(plug->process())
        {
            m_ossia_constraint->addTimeProcess(plug->process());
        }
    }
}
#include <sstream>
void OSSIAConstraintElement::on_processRemoved(const id_type<ProcessModel>& process)
{
    auto it = m_processes.find(process);
    if(it != m_processes.end())
    {
        // It is possible for a process to be null
        // (e.g. invalid state in GUI like automation without address)
        if(auto proc = (*it).second->process())
            m_ossia_constraint->removeTimeProcess(proc);

        // We don't have ownership so we don't delete. The process has it.
        m_processes.erase(it);
    }
}
