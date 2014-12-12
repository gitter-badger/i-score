#pragma once
#include <core/presenter/command/SerializableCommand.hpp>
#include <tools/ObjectPath.hpp>

class DeleteProcessFromIntervalCommand : public iscore::SerializableCommand
{
	public:
		DeleteProcessFromIntervalCommand(ObjectPath&& intervalPath, QString processName, int processId);
		virtual void undo() override;
		virtual void redo() override;
		virtual int id() const override;
		virtual bool mergeWith(const QUndoCommand* other) override;

	protected:
		virtual void serializeImpl(QDataStream&) override;
		virtual void deserializeImpl(QDataStream&) override;

	private:
		ObjectPath m_path;
		QString m_processName;
		int m_processId;

		QByteArray m_serializedProcessData;
};