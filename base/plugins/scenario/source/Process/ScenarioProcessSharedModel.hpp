#pragma once
#include "ProcessInterface/ProcessSharedModelInterface.hpp"

namespace OSSIA
{
	class Scenario;
}
class IntervalModel;
class EventModel;

/**
 * @brief The ScenarioProcessSharedModel class
 *
 * Creation methods return tuples with the identifiers of the objects in their temporal order.
 * (first to last)
 */
class ScenarioProcessSharedModel : public ProcessSharedModelInterface
{
	Q_OBJECT

		friend QDataStream& operator <<(QDataStream& s, const ScenarioProcessSharedModel& scenario);
		friend QDataStream& operator >>(QDataStream& s, ScenarioProcessSharedModel& scenario);
	public:
		ScenarioProcessSharedModel(int id, QObject* parent);
		ScenarioProcessSharedModel(QDataStream& data, QObject* parent);
		virtual ~ScenarioProcessSharedModel() = default;
		virtual ProcessViewModelInterface* makeViewModel(int viewModelId, int processId, QObject* parent) override;
		virtual ProcessViewModelInterface* makeViewModel(QDataStream&, QObject* parent) override;

		virtual QString processName() const override
		{
			return "Scenario";
		}

		// Creation of objects.

		// Creates an interval between two pre-existing events
		int createIntervalBetweenEvents(int startEventId, int endEventId, int newIntervalModelId);

		/**
		 * @brief createIntervalAndEndEventFromEvent Base building block of a scenario.
		 * @param startEventId Identifier of the start event of the new interval
		 * @param duration duration of the new interval
		 * @return A pair : <new interval id, new event id>
		 *
		 * Given a starting event and a duration, creates an interval and an event where
		 * the interval is linked to both events.
		 */
		std::tuple<int, int> createIntervalAndEndEventFromEvent(int startEventId,
																int duration,
																double heightPos,
																int newIntervalId,
																int newEventId);

		// Creates an interval between the start event of the scenario and this one
		// and an event at the end of this interval
		std::tuple<int, int> createIntervalAndEndEventFromStartEvent(int time,
																	 double heightPos,
																	 int newIntervalId,
																	 int newEventId);

		// Creates :
		/// - An interval from the start event of the scenario to an event at startTime
		/// - The event at startTime
		/// - An interval going from the event at startTime to the event at startTime + duration
		/// - The event at startTime + duration
		std::tuple<int, int, int, int> createIntervalAndBothEvents(int startTime,
																   int duration,
																   double heightPos,
																   int createdFirstIntervalId,  // todo maybe put in a tuple.
																   int createdFirstEventId,
																   int createdSecondIntervalId,
																   int createdSecondEventId);

		void moveEventAndInterval(int eventId, int time, double heightPosition);
		void moveInterval(int intervalId, double heightPosition);


		void undo_createIntervalBetweenEvents(int intervalId);
		void undo_createIntervalAndEndEventFromEvent(int intervalId);
		void undo_createIntervalAndEndEventFromStartEvent(int intervalId);
		void undo_createIntervalAndBothEvents(int intervalId);



		IntervalModel* interval(int intervalId);
		EventModel* event(int eventId);

		EventModel* startEvent();
		EventModel* endEvent();

		// For the presenter :
		const std::vector<IntervalModel*> intervals() const
		{ return m_intervals; }
		const std::vector<EventModel*> events() const
		{ return m_events; }

	signals:
		void eventCreated(int eventId);
		void intervalCreated(int intervalId);
		void eventDeleted(int eventId);
		void intervalDeleted(int intervalId);
		void eventMoved(int eventId);
		void intervalMoved(int intervalId);

	private:
		OSSIA::Scenario* m_scenario;

		std::vector<IntervalModel*> m_intervals;
		std::vector<EventModel*> m_events;

		int m_startEventId{};
		int m_endEventId{};
};
