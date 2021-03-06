#include "BaseScenarioModel.hpp"
#include "Document/Event/EventModel.hpp"
#include "Document/TimeNode/TimeNodeModel.hpp"
#include "Document/State/DisplayedStateModel.hpp"
#include "source/Document/Constraint/ConstraintModel.hpp"
#include <iscore/document/DocumentInterface.hpp>

class AbstractConstraintViewModel;
BaseScenario::BaseScenario(const id_type<BaseScenario>& id, QObject* parent):
    IdentifiedObject<BaseScenario>{id, "BaseScenario", parent},
    pluginModelList{iscore::IDocument::documentFromObject(parent), this},

    m_startNode{new TimeNodeModel{id_type<TimeNodeModel>{0}, {{0.2, 0.8}}, TimeValue::zero(),  this}},
    m_endNode  {new TimeNodeModel{id_type<TimeNodeModel>{1}, {{0.2, 0.8}}, TimeValue::zero(), this}}, // TODO take the baseconstraint duration

    m_startEvent{new EventModel{id_type<EventModel>{0}, m_startNode->id(), {{0.4, 0.6}}, TimeValue::zero(), this}},
    m_endEvent  {new EventModel{id_type<EventModel>{1}, m_endNode->id(),   {{0.4, 0.6}}, TimeValue::zero(), this}},

    m_startState{new StateModel{id_type<StateModel>{0}, m_startEvent->id(), 0, this}},
    m_endState  {new StateModel{id_type<StateModel>{1}, m_endEvent->id(),   0, this}},

    m_constraint {new ConstraintModel{
                            id_type<ConstraintModel>{0},
                            id_type<AbstractConstraintViewModel>{0},
                            0,
                            this}}
{
    m_startNode->addEvent(m_startEvent->id());
    m_endNode->addEvent(m_endEvent->id());

    m_startEvent->addState(m_startState->id());
    m_endEvent->addState(m_endState->id());

    m_constraint->setStartState(m_startState->id());
    m_constraint->setEndState(m_endState->id());

    m_startState->setNextConstraint(m_constraint->id());
    m_endState->setPreviousConstraint(m_constraint->id());

    ConstraintModel::Algorithms::changeAllDurations(*m_constraint, std::chrono::minutes{3});
    m_endEvent->setDate(m_constraint->defaultDuration());
    m_endNode->setDate(m_constraint->defaultDuration());
    m_constraint->setObjectName("BaseConstraintModel");
}


ConstraintModel& BaseScenario::baseConstraint() const
{
    return *m_constraint;
}


TimeNodeModel& BaseScenario::startTimeNode() const
{
    return *m_startNode;
}

TimeNodeModel& BaseScenario::endTimeNode() const
{
    return *m_endNode;
}


EventModel& BaseScenario::startEvent() const
{
    return *m_startEvent;
}

EventModel& BaseScenario::endEvent() const
{
    return *m_endEvent;
}


StateModel& BaseScenario::startState() const
{
    return *m_startState;
}

StateModel& BaseScenario::endState() const
{
    return *m_endState;
}


ConstraintModel &BaseScenario::constraint(const id_type<ConstraintModel> &constraintId) const
{
    Q_ASSERT(constraintId == m_constraint->id());
    return *m_constraint;
}

EventModel &BaseScenario::event(const id_type<EventModel> &id) const
{
    Q_ASSERT(id == m_startEvent->id() || id == m_endEvent->id());
    return id == m_startEvent->id()
            ? *m_startEvent
            : *m_endEvent;
}

TimeNodeModel &BaseScenario::timeNode(const id_type<TimeNodeModel> &id) const
{
    Q_ASSERT(id == m_startNode->id() || id == m_endNode->id());
    return id == m_startNode->id()
            ? *m_startNode
            : *m_endNode;
}

StateModel &BaseScenario::state(const id_type<StateModel> &id) const
{
    Q_ASSERT(id == m_startState->id() || id == m_endState->id());
    return id == m_startState->id()
            ? *m_startState
            : *m_endState;
}
