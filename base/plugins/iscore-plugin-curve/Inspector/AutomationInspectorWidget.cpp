#include "AutomationInspectorWidget.hpp"
#include "Automation/AutomationModel.hpp"
#include <Inspector/InspectorSectionWidget.hpp>
#include "Commands/ChangeAddress.hpp"
#include "Commands/SetCurveMin.hpp"
#include "Commands/SetCurveMax.hpp"

#include <Singletons/DeviceExplorerInterface.hpp>
#include <DeviceExplorer/../Plugin/Widgets/DeviceCompleter.hpp>
#include <DeviceExplorer/../Plugin/Widgets/DeviceExplorerMenuButton.hpp>
#include <DeviceExplorer/../Plugin/Panel/DeviceExplorerModel.hpp>

#include <iscore/document/DocumentInterface.hpp>
#include <core/document/Document.hpp>

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QDoubleSpinBox>

#include <QApplication>

AutomationInspectorWidget::AutomationInspectorWidget(
        const AutomationModel* automationModel,
        QWidget* parent) :
    InspectorWidgetBase {automationModel, parent},
    m_model {automationModel}
{
    setObjectName("AutomationInspectorWidget");
    setParent(parent);

    QVector<QWidget*> vec;

    auto widg = new QWidget;
    auto vlay = new QVBoxLayout{widg};
    vlay->setSpacing(0);
    vlay->setContentsMargins(0,0,0,0);
    auto hlay = new QHBoxLayout{};
    hlay->setSpacing(0);
    hlay->setContentsMargins(0,0,0,0);

    vec.push_back(widg);

    // LineEdit
    m_lineEdit = new QLineEdit;
    m_lineEdit->setText(m_model->address().toString());
    connect(m_model, &AutomationModel::addressChanged,
            this, [&] (const iscore::Address& addr) {
       m_lineEdit->setText(addr.toString());
    });

    connect(m_lineEdit, &QLineEdit::editingFinished,
            [=]() {
        // Validate the address
        const auto& str = m_lineEdit->text();

        // TODO DeviceCompleter should be updated.
        if(iscore::Address::validateString(str))
        {
            on_addressChange(iscore::Address::fromString(str));
        }
        else
        {
            // TODO warn the user and clear.
            m_lineEdit->clear();
        }
    });

    vlay->addWidget(m_lineEdit);

    // Min / max
    auto minmaxwid = new QWidget;
    auto minmaxlay = new QFormLayout{minmaxwid};
    vec.push_back(minmaxwid);
    minmaxlay->setSpacing(0);
    minmaxlay->setContentsMargins(0, 0, 0, 0);

    m_minsb = new QDoubleSpinBox;
    m_maxsb = new QDoubleSpinBox;
    m_minsb->setValue(m_model->min());
    m_maxsb->setValue(m_model->max());
    minmaxlay->addRow(tr("Min"), m_minsb);
    minmaxlay->addRow(tr("Max"), m_maxsb);

    connect(m_model, SIGNAL(minChanged(double)), m_minsb, SLOT(setValue(double)));
    connect(m_model, SIGNAL(maxChanged(double)), m_maxsb, SLOT(setValue(double)));

    connect(m_minsb, SIGNAL(editingFinished()), this, SLOT(on_minValueChanged()));
    connect(m_maxsb, SIGNAL(editingFinished()), this, SLOT(on_maxValueChanged()));

    // If there is a DeviceExplorer in the current document, use it
    // to make a widget.
    auto deviceexplorer = iscore::IDocument::documentFromObject(automationModel)->findChild<DeviceExplorerModel*>("DeviceExplorerModel");
    if(deviceexplorer)
    {
        // LineEdit completion
        auto completer = new DeviceCompleter {deviceexplorer, this};
        m_lineEdit->setCompleter(completer);

        auto pb = new DeviceExplorerMenuButton{deviceexplorer};
        connect(pb, &DeviceExplorerMenuButton::addressChosen,
                this, [&] (const iscore::Address& addr)
        {
            m_lineEdit->setText(addr.toString());
            on_addressChange(addr);
        });

        hlay->addWidget(pb);
    }

    // Add it to a new deck
    auto display = new QPushButton{"~"};
    hlay->addWidget(display);
    connect(display,    &QPushButton::clicked,
            [ = ]()
    {
        createViewInNewDeck(QString::number(m_model->id_val()));
    });

    vlay->addLayout(hlay);


    updateSectionsView(static_cast<QVBoxLayout*>(layout()), vec);
}

void AutomationInspectorWidget::on_addressChange(const iscore::Address& newText)
{
    if(newText != m_model->address())
    {
        auto cmd = new ChangeAddress{
                    iscore::IDocument::path(m_model),
                    newText };

        commandDispatcher()->submitCommand(cmd);
    }
}

void AutomationInspectorWidget::on_minValueChanged()
{
    auto newVal = m_minsb->value();
    if(newVal != m_model->min())
    {
        auto cmd = new SetCurveMin{
                    iscore::IDocument::path(m_model), newVal};

        commandDispatcher()->submitCommand(cmd);
    }
}

void AutomationInspectorWidget::on_maxValueChanged()
{
    auto newVal = m_maxsb->value();
    if(newVal != m_model->max())
    {
        auto cmd = new SetCurveMax{
                    iscore::IDocument::path(m_model), newVal};

        commandDispatcher()->submitCommand(cmd);
    }
}
