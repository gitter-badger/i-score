#pragma once
#include <QObject>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>


class AutomationInspectorFactory : public InspectorWidgetFactory
{
    public:
        AutomationInspectorFactory() :
            InspectorWidgetFactory {}
        {

        }

        virtual InspectorWidgetBase* makeWidget(
                const QObject* sourceElement,
                QWidget* parent) override;

        virtual QList<QString> correspondingObjectsNames() const override
        {
            return {"Automation"};
        }
};
