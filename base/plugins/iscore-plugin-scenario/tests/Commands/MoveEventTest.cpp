#include <QtTest/QtTest>

#include <Commands/Scenario/Displacement/MoveEvent.hpp>
#include <Commands/Scenario/Creations/CreateEvent.hpp>

#include <Document/Event/EventModel.hpp>
#include <Document/Event/EventData.hpp>

#include <Process/ScenarioModel.hpp>

using namespace iscore;
using namespace Scenario::Command;

class MoveEventTest: public QObject
{
        Q_OBJECT
    public:

    private slots:

        void MoveCommandTest()
        {
            ScenarioModel* scenar = new ScenarioModel(std::chrono::seconds(15), id_type<ProcessModel> {0}, qApp);
            // 1. Create a new event (the first one cannot move since it does not have
            // predecessors ?)

            EventData data {};
            data.dDate.setMSecs(56);
            data.relativeY = 0.1;

            CreateEvent create_ev_cmd(
            {{"ScenarioModel", {}}},
            data);

            create_ev_cmd.redo();
            auto eventid = create_ev_cmd.m_cmd->m_createdEventId;

            MoveEvent cmd(
            {
                {"ScenarioModel", {}},
            }, eventid, data.dDate, data.relativeY);

            cmd.redo();
            QCOMPARE(scenar->event(eventid)->heightPercentage(), 0.1);

            cmd.undo();
            QCOMPARE(scenar->event(eventid)->heightPercentage(), 0.5);

            cmd.redo();
            QCOMPARE(scenar->event(eventid)->heightPercentage(), 0.1);

            // TODO test an horizontal displacement.

            // Delete them else they stay in qApp !

            delete scenar;
        }
};

QTEST_MAIN(MoveEventTest)
#include "MoveEventTest.moc"



