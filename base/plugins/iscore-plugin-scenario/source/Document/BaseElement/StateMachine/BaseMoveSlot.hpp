#pragma once
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/command/Dispatchers/SingleOngoingCommandDispatcher.hpp>
#include <QState>
#include <QStateMachine>

class QGraphicsScene;
class BaseStateMachine;
class SlotModel;
class BaseMoveSlot : public QState
{
    public:
        BaseMoveSlot(
                const QGraphicsScene& scene,
                iscore::CommandStack& stack,
                BaseStateMachine& sm);

        void start()
        { m_localSM.start(); }

    private:
        const BaseStateMachine& m_sm;
        QStateMachine m_localSM;
        QState* m_waitState{};

        CommandDispatcher<> m_dispatcher;
        SingleOngoingCommandDispatcher m_ongoingDispatcher;
        const QGraphicsScene& m_scene;
};

