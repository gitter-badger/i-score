#pragma once
#include <iscore/command/SerializableCommand.hpp>

#include <iscore/tools/ObjectPath.hpp>

namespace Scenario
{
    namespace Command
    {
        // TODO property command?
        template<class T>
        class ChangeElementColor : public iscore::SerializableCommand
        {
                // No ISCORE_COMMAND here since it's a template.
                // TODO what about the uids ?
            public:
                static const char * commandName()
                {
                    static QByteArray name = QString{"ChangeElementColor_%1"}.arg(T::staticMetaObject.className()).toLatin1();
                    return name.constData();
                }
                static QString description()
                {
                    return QObject::tr("Change %1 color").arg(T::prettyName());
                }

                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR(ChangeElementColor, "ScenarioControl")
                ChangeElementColor(ObjectPath&& path, QColor newLabel) :
                    SerializableCommand {"ScenarioControl",
                                         commandName(),
                                         description()},
                    m_path {std::move(path) },
                    m_newColor {newLabel}
                {
                    auto& obj = m_path.find<T>();
                    m_oldColor = obj.metadata.color();
                }

                virtual void undo() override
                {
                    auto& obj = m_path.find<typename std::remove_const<T>::type>();
                    obj.metadata.setColor(m_oldColor);
                }

                virtual void redo() override
                {
                    auto& obj = m_path.find<typename std::remove_const<T>::type>();
                    obj.metadata.setColor(m_newColor);
                }

            protected:
                virtual void serializeImpl(QDataStream& s) const override
                {
                    s << m_path << m_oldColor << m_newColor;
                }

                virtual void deserializeImpl(QDataStream& s) override
                {
                    s >> m_path >> m_oldColor >> m_newColor;
                }

            private:
                ObjectPath m_path;
                QColor m_newColor;
                QColor m_oldColor;
        };
    }
}
