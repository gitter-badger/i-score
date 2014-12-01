#pragma once
#include <core/presenter/command/SerializableCommand.hpp>
#include <QNamedObject>

class CreateEventAfterEventCommand : public iscore::SerializableCommand
{
	public:
		CreateEventAfterEventCommand();
		CreateEventAfterEventCommand(ObjectPath&& scenarioPath, int firstEventId, int time);
		virtual void undo() override;
		virtual void redo() override;
		virtual int id() const override;
		virtual bool mergeWith(const QUndoCommand* other) override;

	protected:
		virtual void serializeImpl(QDataStream&) override;
		virtual void deserializeImpl(QDataStream&) override;

	private:
		ObjectPath m_path;

		int m_intervalId{};

		int m_firstEventId{};
		int m_time{};
};