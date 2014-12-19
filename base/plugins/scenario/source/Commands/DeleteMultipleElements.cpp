#include "DeleteMultipleElements.hpp"
#include <QApplication>

#include "core/presenter/Presenter.hpp"

DeleteMultipleElementsCommand::DeleteMultipleElementsCommand(
		QVector<iscore::SerializableCommand*> deletionCommands):
	iscore::SerializableCommand{"ScenarioControl",
								"DeleteMultipleElementsCommand",
								QObject::tr("Group deletion")}
{
	for(auto& cmd : deletionCommands)
	{
		m_serializedCommands.push_back({{cmd->parentName(), cmd->name()}, cmd->serialize()});
	}
}

void DeleteMultipleElementsCommand::undo()
{
	for(auto& cmd_pack : m_serializedCommands)
	{
		// Put this in the ctor as an optimization
		auto presenter = qApp->findChild<iscore::Presenter*>("Presenter");
		auto cmd = presenter->instantiateUndoCommand(cmd_pack.first.first,
													 cmd_pack.first.second,
													 cmd_pack.second);

		cmd->undo();
	}
}

void DeleteMultipleElementsCommand::redo()
{
	for(auto& cmd_pack : m_serializedCommands)
	{
		// Put this in the ctor as an optimization
		auto presenter = qApp->findChild<iscore::Presenter*>("Presenter");
		auto cmd = presenter->instantiateUndoCommand(cmd_pack.first.first,
													 cmd_pack.first.second,
													 cmd_pack.second);

		cmd->redo();
	}
}

int DeleteMultipleElementsCommand::id() const
{
	return 1;
}

bool DeleteMultipleElementsCommand::mergeWith(const QUndoCommand* other)
{
	return false;
}

void DeleteMultipleElementsCommand::serializeImpl(QDataStream& s)
{
	s << m_serializedCommands;
}

void DeleteMultipleElementsCommand::deserializeImpl(QDataStream& s)
{
	s >> m_serializedCommands;
}