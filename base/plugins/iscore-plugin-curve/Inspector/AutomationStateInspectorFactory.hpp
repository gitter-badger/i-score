#pragma once
#include <QObject>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>


class AutomationStateInspectorFactory : public InspectorWidgetFactory
{
    public:
        AutomationStateInspectorFactory() :
            InspectorWidgetFactory {}
        {

        }

        virtual InspectorWidgetBase* makeWidget(
                const QObject* sourceElement,
                QWidget* parent) override;

        virtual QList<QString> correspondingObjectsNames() const override
        {
            return {"AutomationState"};
        }
};
