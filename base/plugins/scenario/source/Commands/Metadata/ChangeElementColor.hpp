#pragma once
#include <core/presenter/command/SerializableCommand.hpp>

#include <core/tools/ObjectPath.hpp>

namespace Scenario
{
    namespace Command
    {
    template<class T>
        class ChangeElementLabel : public iscore::SerializableCommand
        {

            public:
            ChangeElementLabel(ObjectPath&& path, QColor newLabel) :
                SerializableCommand{"ScenarioControl",
                                    "Change Label",
                                    QObject::tr("Change current objects label")},
                m_newColor{newLabel},
                m_path{std::move(path)}
            {

            }

                virtual void undo() override
            {
                auto obj = m_path.find<T>();
                obj->metadata.setColor(m_oldColor);
            }
                virtual void redo() override
            {
                auto obj = m_path.find<T>();
                obj->metadata.setColor(m_newColor);
            }
                virtual int id() const override
            {
                return 1;
            }

                virtual bool mergeWith(const QUndoCommand* other) override
            {
                return false;
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
                ObjectPath m_path{};
                QColor m_oldColor{};
                QColor m_newColor;
        };
    }
}