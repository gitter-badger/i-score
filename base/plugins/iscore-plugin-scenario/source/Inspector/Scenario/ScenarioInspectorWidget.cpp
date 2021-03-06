#include "ScenarioInspectorWidget.hpp"
#include <Inspector/InspectorSectionWidget.hpp>

#include "Process/ScenarioModel.hpp"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

ScenarioInspectorWidget::ScenarioInspectorWidget(
        const ScenarioModel* object,
        QWidget* parent) :
    InspectorWidgetBase {object, parent},
    m_model {object}
{
    setObjectName("ScenarioInspectorWidget");
    setParent(parent);

    QVector<QWidget*> vec;
    vec.push_back(new QLabel{ QString::number(object->id_val())} );

    QPushButton* displayBtn = new QPushButton {tr("Display in new Slot"), this};
    vec.push_back(displayBtn);

    connect(displayBtn, &QPushButton::clicked,
            [=] ()
    {
        emit createViewInNewSlot(QString::number(m_model->id_val()));
    });

    updateSectionsView(static_cast<QVBoxLayout*>(layout()), vec);
}
