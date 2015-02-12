#pragma once
#include <QObject>
#include <InspectorInterface/InspectorWidgetFactoryInterface.hpp>


class TimeNodeInspectorFactory : public InspectorWidgetFactoryInterface
{
	public:
		TimeNodeInspectorFactory() :
			InspectorWidgetFactoryInterface {}
		{

		}

		virtual InspectorWidgetBase* makeWidget (QObject* sourceElement) override;
		virtual InspectorWidgetBase* makeWidget (QList<QObject*> sourceElements) override;

		virtual QString correspondingObjectName() const override
		{
			return "TimeNodeModel";
		}
};