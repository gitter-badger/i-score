#include "RemoveProcessFromConstraint.hpp"

#include "Document/Constraint/ConstraintModel.hpp"

#include "ProcessInterface/ProcessSharedModelInterface.hpp"

#include <QDebug>

using namespace iscore;
using namespace Scenario::Command;

RemoveProcessFromConstraint::RemoveProcessFromConstraint(ObjectPath&& constraintPath, int processId):
	SerializableCommand{"ScenarioControl",
						"DeleteProcessFromConstraintCommand",
						"Delete process"},
	m_path{std::move(constraintPath)},
	m_processId{processId}
{
	auto constraint = static_cast<ConstraintModel*>(m_path.find());
	auto process = constraint->process(m_processId);

	{
		QDataStream s(&m_serializedProcessData, QIODevice::WriteOnly);
		s.setVersion(QDataStream::Qt_5_3);
		s << process->processName(); // We have to serialize it.
		s << *process;
	}

	m_processName = process->processName();
}

void RemoveProcessFromConstraint::undo()
{
	auto constraint = static_cast<ConstraintModel*>(m_path.find());
	{
		QDataStream s(&m_serializedProcessData, QIODevice::ReadOnly);
		constraint->createProcess(s);
	}
}

void RemoveProcessFromConstraint::redo()
{
	auto constraint = static_cast<ConstraintModel*>(m_path.find());
	constraint->removeProcess(m_processId);
}

int RemoveProcessFromConstraint::id() const
{
	return 1;
}

bool RemoveProcessFromConstraint::mergeWith(const QUndoCommand* other)
{
	return false;
}

void RemoveProcessFromConstraint::serializeImpl(QDataStream& s)
{
	s << m_path << m_processName << m_processId << m_serializedProcessData;
}

void RemoveProcessFromConstraint::deserializeImpl(QDataStream& s)
{
	s >> m_path >> m_processName >> m_processId >> m_serializedProcessData;
}