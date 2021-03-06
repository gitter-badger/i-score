#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ObjectPath.hpp>

#include "Commands/Constraint/SetRigidity.hpp"

namespace Scenario
{
    namespace Command
    {
        class SetTrigger : public iscore::SerializableCommand
        {
                ISCORE_COMMAND_DECL("SetTrigger", "SetTrigger")
            public:
                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR(SetTrigger, "ScenarioControl")
                SetTrigger(ObjectPath&& eventPath, QString condition);
                virtual void undo() override;
                virtual void redo() override;

            protected:
                virtual void serializeImpl(QDataStream&) const override;
                virtual void deserializeImpl(QDataStream&) override;

            private:
                ObjectPath m_path;
                QString m_trigger;
                QString m_previousTrigger;

                QVector<SetRigidity*> m_cmd; //TODO serialization
        };
    }
}
