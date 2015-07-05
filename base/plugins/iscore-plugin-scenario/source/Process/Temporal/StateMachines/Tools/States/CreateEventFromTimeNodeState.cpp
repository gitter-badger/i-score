#include "CreateEventState.hpp"

#include "Document/TimeNode/TimeNodeModel.hpp"
#include "Process/Temporal/StateMachines/ScenarioStateMachine.hpp"
#include <Process/ScenarioModel.hpp>

#include "Commands/Scenario/Displacement/MoveEvent.hpp"
#include "Commands/Scenario/Displacement/MoveNewEvent.hpp"
#include "Commands/Scenario/Creations/CreateEventAfterEvent.hpp"
#include "Commands/Scenario/Creations/CreateEventAfterEventOnTimeNode.hpp"
#include "Commands/Scenario/Creations/CreateConstraint.hpp"
#include "Commands/TimeNode/MergeTimeNodes.hpp"
#include "Commands/Scenario/Creations/CreateEventOnTimeNode.hpp"

#include "Process/Temporal/StateMachines/Transitions/NothingTransitions.hpp"
#include "Process/Temporal/StateMachines/Transitions/AnythingTransitions.hpp"
#include "Process/Temporal/StateMachines/Transitions/EventTransitions.hpp"
#include "Process/Temporal/StateMachines/Transitions/ConstraintTransitions.hpp"
#include "Process/Temporal/StateMachines/Transitions/TimeNodeTransitions.hpp"

#include <QFinalState>

#include "../ScenarioRollbackStrategy.hpp"
//TODO : is it really needed according to the lyon's workflow ?
    // i.e. we always create event from event or on nothing

using namespace Scenario::Command;

CreateFromTimeNodeState::CreateFromTimeNodeState(
    const ScenarioStateMachine& stateMachine,
    ObjectPath &&scenarioPath,
    iscore::CommandStack& stack,
    QState* parent):
    CreationState{std::move(scenarioPath), parent},
    m_dispatcher{stack}
{
    using namespace Scenario::Command;
    auto finalState = new QFinalState{this};
    connect(finalState, &QState::entered, [&] ()
    {
        clearCreatedIds();
    });

    QState* mainState = new QState{this};
    {
    QState* pressedState = new QState{mainState};
    QState* releasedState = new QState{mainState};
    QState* movingOnNothingState = new QState{mainState};
    QState* movingOnEventState = new QState{mainState};
    QState* movingOnTimeNodeState = new QState{mainState};

    // General setup
    mainState->setInitialState(pressedState);
    releasedState->addTransition(finalState);

    // Release
    make_transition<ReleaseOnAnything_Transition>(mainState, releasedState);

    // Pressed -> ...
    auto t_pressed_moving_nothing =
        make_transition<MoveOnNothing_Transition>(
            pressedState, movingOnNothingState, *this);

    connect(t_pressed_moving_nothing, &QAbstractTransition::triggered,
        [&] ()
    {
        createEventFromEventOnNothing(stateMachine);
    });

    /// MoveOnNothing -> ...
    // MoveOnNothing -> MoveOnNothing. Nothing particular to trigger.
    make_transition<MoveOnNothing_Transition>(
        movingOnNothingState, movingOnNothingState, *this);



    // MoveOnNothing -> MoveOnEvent.
    auto t_move_nothing_event =
        make_transition<MoveOnEvent_Transition>(
            movingOnNothingState, movingOnEventState, *this);

    connect(t_move_nothing_event, &QAbstractTransition::triggered,
        [&] ()
    {
        if(createdEvents.contains(hoveredEvent))
        {
            return;
        }
        m_dispatcher.rollback<ScenarioRollbackStrategy>();
        createSingleEventOnTimeNode();
        createConstraintBetweenEvents();
    });

    // MoveOnNothing -> MoveOnTimeNode
    auto t_move_nothing_timenode =
        make_transition<MoveOnTimeNode_Transition>(
            movingOnNothingState, movingOnTimeNodeState, *this);

    connect(t_move_nothing_timenode, &QAbstractTransition::triggered,
        [&] ()
    {
        if(createdTimeNodes.contains(hoveredTimeNode))
        {
            return;
        }
        m_dispatcher.rollback<ScenarioRollbackStrategy>();
        createSingleEventOnTimeNode();
        createEventFromEventOnTimenode();
     });


    /// MoveOnEvent -> ...
    // MoveOnEvent -> MoveOnNothing
    auto t_move_event_nothing =
        make_transition<MoveOnNothing_Transition>(
            movingOnEventState, movingOnNothingState, *this);

    connect(t_move_event_nothing, &QAbstractTransition::triggered,
        [&] ()
    {
        m_dispatcher.rollback<ScenarioRollbackStrategy>();
        createSingleEventOnTimeNode();
        createEventFromEventOnNothing(stateMachine);
     });

    // MoveOnEvent -> MoveOnEvent : nothing to do.

    // MoveOnEvent -> MoveOnTimeNode
    auto t_move_event_timenode =
        make_transition<MoveOnTimeNode_Transition>(
            movingOnEventState, movingOnTimeNodeState, *this);

    connect(t_move_event_timenode, &QAbstractTransition::triggered,
        [&] ()
    {
        if(createdTimeNodes.contains(hoveredTimeNode))
        {
            return;
        }
        m_dispatcher.rollback<ScenarioRollbackStrategy>();
        createSingleEventOnTimeNode();
        createEventFromEventOnTimenode();
    });


    /// MoveOnTimeNode -> ...
    // MoveOnTimeNode -> MoveOnNothing
    auto t_move_timenode_nothing =
        make_transition<MoveOnNothing_Transition>(
            movingOnTimeNodeState, movingOnNothingState, *this);

    connect(t_move_timenode_nothing, &QAbstractTransition::triggered,
        [&] ()
    {
        m_dispatcher.rollback<ScenarioRollbackStrategy>();
        createSingleEventOnTimeNode();
        createEventFromEventOnNothing(stateMachine);
     });


    // MoveOnTimeNode -> MoveOnEvent
    auto t_move_timenode_event =
        make_transition<MoveOnEvent_Transition>(
            movingOnTimeNodeState, movingOnEventState, *this);

    connect(t_move_timenode_event, &QAbstractTransition::triggered,
        [&] ()
    {
        if(createdEvents.contains(hoveredEvent))
        {
            return;
        }
        m_dispatcher.rollback<ScenarioRollbackStrategy>();
        createSingleEventOnTimeNode();
        createConstraintBetweenEvents();
    });


    // MoveOnTimeNode -> MoveOnTimeNode
    make_transition<MoveOnTimeNode_Transition>(
            movingOnTimeNodeState, movingOnTimeNodeState, *this);

    // What happens in each state.
    QObject::connect(pressedState, &QState::entered,
             [&] ()
    {
        m_clickedPoint = currentPoint;
        createSingleEventOnTimeNode();
    });

    QObject::connect(movingOnNothingState, &QState::entered, [&] ()
    {
        ISCORE_TODO
        /*
        m_dispatcher.submitCommand<MoveNewEvent>(
                ObjectPath{m_scenarioPath},
                createdConstraint(),
                createdEvent(),
                currentPoint.date,
                currentPoint.y,
                !stateMachine.isShiftPressed());
                */
    });

    QObject::connect(movingOnTimeNodeState, &QState::entered, [&] ()
    {
        ISCORE_TODO
                /*
        m_dispatcher.submitCommand<MoveEvent>(
                ObjectPath{m_scenarioPath},
                createdEvent(),
                m_scenarioPath.find<ScenarioModel>().timeNode(hoveredTimeNode).date(),
                stateMachine.expandMode());
                        */
    });

    QObject::connect(releasedState, &QState::entered, [&] ()
    {
        m_dispatcher.commit<Scenario::Command::CreationMetaCommand>();
    });
    }

    QState* rollbackState = new QState{this};
    make_transition<Cancel_Transition>(mainState, rollbackState);
    rollbackState->addTransition(finalState);
    QObject::connect(rollbackState, &QState::entered, [&] ()
    {
    m_dispatcher.rollback<ScenarioRollbackStrategy>();
    });

    setInitialState(mainState);
}
void CreateFromTimeNodeState::createSingleEventOnTimeNode()
{
    ISCORE_TODO
            /*
    auto cmd =
        new CreateEventOnTimeNode(
        ObjectPath{m_scenarioPath},
        clickedTimeNode,
        m_clickedPoint.y);
    m_dispatcher.submitCommand(cmd);

    m_createdFirstEvent = cmd->createdEvent();
            */
}

void CreateFromTimeNodeState::createEventFromEventOnNothing(const ScenarioStateMachine &stateMachine)
{
    ISCORE_TODO
            /*
    // If we start moving, we have to put a createEventAfterEvent.
    // Note : there will be two created events; how to prevent a mess in collision management ?
    auto cmd = new CreateEventAfterEvent{
           ObjectPath{m_scenarioPath},
           m_createdFirstEvent,
           currentPoint.date,
           currentPoint.y,
           stateMachine.isShiftPressed()};

    setCreatedEvent(cmd->createdEvent());
    setCreatedTimeNode(cmd->createdTimeNode());
    m_dispatcher.submitCommand(cmd);
            */
}

void CreateFromTimeNodeState::createEventFromEventOnTimenode()
{

    ISCORE_TODO
            /*
    auto cmd = new CreateEventAfterEventOnTimeNode(
           ObjectPath{m_scenarioPath},
           m_createdFirstEvent,
           hoveredTimeNode,
           currentPoint.date,
           currentPoint.y);

    setCreatedEvent(cmd->createdEvent());
    setCreatedTimeNode(id_type<TimeNodeModel>{});
    m_dispatcher.submitCommand(cmd);
            */
}

void CreateFromTimeNodeState::createConstraintBetweenEvents()
{
    ISCORE_TODO
    /*
    auto cmd = new CreateConstraint(
           ObjectPath{m_scenarioPath},
           m_createdFirstEvent,
           clickedState,
           hoveredEvent);

    m_dispatcher.submitCommand(cmd);
    */
}
