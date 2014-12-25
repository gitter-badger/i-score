#pragma once
#include <core/presenter/command/SerializableCommand.hpp>

#include <core/tools/ObjectPath.hpp>
#include <QMap>
#include <tuple>

namespace Scenario
{
	namespace Command
	{
		/**
		 * @brief The ClearConstraint class
		 *
		 * Removes all the processes and the boxes of a constraint.
		 */
		class ClearConstraint : public iscore::SerializableCommand
		{
			public:
				ClearConstraint();
				ClearConstraint(ObjectPath&& constraintPath);
				virtual void undo() override;
				virtual void redo() override;
				virtual int id() const override;
				virtual bool mergeWith(const QUndoCommand* other) override;

			protected:
				virtual void serializeImpl(QDataStream&) override;
				virtual void deserializeImpl(QDataStream&) override;

			private:
				ObjectPath m_path;

				QVector<QByteArray> m_serializedBoxes;
				QVector<QByteArray> m_serializedProcesses;

				QMap<std::tuple<int,int,int>, QPair<bool, int>> m_scenarioViewModelsBoxMappings;
		};
	}
}