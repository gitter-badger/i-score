#pragma once
#include <core/presenter/command/SerializableCommand.hpp>

#include <core/tools/ObjectPath.hpp>

namespace Scenario
{
	namespace Command
	{
		/**
 * @brief The ClearEvent class
 *
 * For now, just delete the states from the event.
 *
 *
 * Another possibility would be : if an event is not the first or last event,
 * and if the event does not have followers,
 * remove the event and clear all of its predecessors.
 *
 * It is the responsibility of the Presenter to check
 * if it is removable (not first or last event / no followers).
 */
		class ClearEvent : public iscore::SerializableCommand
		{
			public:
				ClearEvent();
				ClearEvent(ObjectPath&& path);
				virtual void undo() override;
				virtual void redo() override;
				virtual int id() const override;
				virtual bool mergeWith(const QUndoCommand* other) override;

			protected:
				virtual void serializeImpl(QDataStream&) override;
				virtual void deserializeImpl(QDataStream&) override;

			private:
				ObjectPath m_path;

				QVector<QByteArray> m_serializedStates;
				//		QByteArray m_serializedEvent;
				//		QVector<QByteArray> m_serializedConstraints; // The handlers inside the events are IN the constraints / Boxes / etc.
		};
	}
}