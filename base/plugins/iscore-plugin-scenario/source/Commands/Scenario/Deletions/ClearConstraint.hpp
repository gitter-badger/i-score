#pragma once
#include <iscore/command/SerializableCommand.hpp>

#include <iscore/tools/ObjectPath.hpp>
#include <QMap>
#include <tuple>
class AbstractConstraintViewModel;
class RackModel;
namespace Scenario
{
    namespace Command
    {
        /**
         * @brief The ClearConstraint class
         *
         * Removes all the processes and the rackes of a constraint.
         */
        class ClearConstraint : public iscore::SerializableCommand
        {
                ISCORE_COMMAND_DECL("ClearConstraint", "ClearConstraint")
            public:
                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR(ClearConstraint, "ScenarioControl")
                ClearConstraint(ObjectPath&& constraintPath);
                virtual void undo() override;
                virtual void redo() override;

            protected:
                virtual void serializeImpl(QDataStream&) const override;
                virtual void deserializeImpl(QDataStream&) override;

            private:
                ObjectPath m_path;

                QVector<QByteArray> m_serializedRackes;
                QVector<QByteArray> m_serializedProcesses;

                QMap<id_type<AbstractConstraintViewModel>, id_type<RackModel>> m_rackMappings;
        };
    }
}
