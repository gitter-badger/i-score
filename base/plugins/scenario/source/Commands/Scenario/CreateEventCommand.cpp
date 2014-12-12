#include "CreateEventCommand.hpp"

#include "Process/ScenarioProcessSharedModel.hpp"
#include "Document/Event/EventModel.hpp"
#include "Document/Interval/IntervalModel.hpp"

#include <core/application/Application.hpp>
#include <core/view/View.hpp>

#include <QApplication>
#include <QDebug>
using namespace iscore;

// TODO maybe the to-be-added event id should be created here ?
CreateEventCommand::CreateEventCommand():
	SerializableCommand{"ScenarioControl",
						"CreateEventCommand",
						QObject::tr("Event creation")}
{

}

CreateEventCommand::CreateEventCommand(ObjectPath&& scenarioPath, int time, double heightPosition):
	SerializableCommand{"ScenarioControl",
						"CreateEventCommand",
						QObject::tr("Event creation")},
	m_path(std::move(scenarioPath)),
	m_time{time},
	m_heightPosition{heightPosition}
{
	auto scenar = static_cast<ScenarioProcessSharedModel*>(m_path.find());

	m_createdIntervalId = getNextId(scenar->intervals());
	m_createdEventId = getNextId(scenar->events());
}

void CreateEventCommand::undo()
{
	auto scenar = static_cast<ScenarioProcessSharedModel*>(m_path.find());
	if(scenar != nullptr)
	{
		scenar->undo_createIntervalAndEndEventFromStartEvent(m_createdIntervalId);
	}
}

void CreateEventCommand::redo()
{
	auto scenar = static_cast<ScenarioProcessSharedModel*>(m_path.find());
	if(scenar != nullptr)
	{
		scenar->createIntervalAndEndEventFromStartEvent(m_time,
														m_heightPosition,
														m_createdIntervalId,
														m_createdEventId);
	}
}

int CreateEventCommand::id() const
{
	return 1;
}

bool CreateEventCommand::mergeWith(const QUndoCommand* other)
{
	return false;
}

void CreateEventCommand::serializeImpl(QDataStream& s)
{
	s << m_path << m_time << m_heightPosition;
}

void CreateEventCommand::deserializeImpl(QDataStream& s)
{
	s >> m_path >> m_time >> m_heightPosition;
}