#pragma once
#include <iscore/command/SerializableCommand.hpp>

#include <iscore/tools/ObjectPath.hpp>

namespace Scenario
{
    namespace Command
    {
        template<class T>
        class ChangeElementComments : public iscore::SerializableCommand
        {
                // No ISCORE_COMMAND here since it's a template.
            public:
                static const char * commandName()
                {
                    static QByteArray name = QString{"ChangeElementComments_%1"}.arg(T::staticMetaObject.className()).toLatin1();
                    return name.constData();
                }
                static QString description()
                {
                    return QObject::tr("Change %1 comments").arg(T::prettyName());
                }

                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR(ChangeElementComments, "ScenarioControl")
                ChangeElementComments(ObjectPath&& path, QString newComments) :
                    SerializableCommand {"ScenarioControl",
                                         commandName(),
                                         description()},
                    m_path{std::move(path)},
                    m_newComments {newComments}
                {
                    auto& obj = m_path.find<T>();
                    m_oldComments = obj.metadata.comment();
                }

                virtual void undo() override
                {
                    auto& obj = m_path.find<typename std::remove_const<T>::type>();
                    obj.metadata.setComment(m_oldComments);
                }

                virtual void redo() override
                {
                    auto& obj = m_path.find<typename std::remove_const<T>::type>();
                    obj.metadata.setComment(m_newComments);
                }

            protected:
                virtual void serializeImpl(QDataStream& s) const override
                {
                    s << m_path << m_oldComments << m_newComments;
                }

                virtual void deserializeImpl(QDataStream& s) override
                {
                    s >> m_path >> m_oldComments >> m_newComments;
                }

            private:
                ObjectPath m_path;
                QString m_oldComments;
                QString m_newComments;
        };
    }
}
