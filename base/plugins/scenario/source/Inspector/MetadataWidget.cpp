#include "MetadataWidget.hpp"

#include "Document/ModelMetadata.hpp"
#include "InspectorInterface/InspectorSectionWidget.hpp"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QTextEdit>
#include <QVBoxLayout>

MetadataWidget::MetadataWidget(ModelMetadata *metadata, QWidget *parent) :
    QWidget (parent)
{
    QVBoxLayout* metadataLayout = new QVBoxLayout{this};
    setLayout(metadataLayout);
    // type
    m_typeLb = new QLabel ("type");

    // LABEL : label + lineEdit in a container
    QWidget* descriptionWidget = new QWidget {this};
    QFormLayout* descriptionLay = new QFormLayout;

    m_scriptingNameLine = new QLineEdit{metadata->name(), this};
    m_labelLine = new QLineEdit{metadata->label(), this};

    descriptionLay->addRow("Name", m_scriptingNameLine);
    descriptionLay->addRow("Label", m_labelLine);

    descriptionWidget->setObjectName ("Description");

    descriptionWidget->setLayout (descriptionLay);

    // color
    m_colorButton = new QPushButton{this};
    m_colorButton->setMaximumSize (QSize (1.5 * m_colorIconSize, 1.5 * m_colorIconSize) );
    m_colorButton->setIconSize (QSize (m_colorIconSize, m_colorIconSize) );
    m_colorButtonPixmap.fill (metadata->color());
    m_colorButton->setIcon (QIcon (m_colorButtonPixmap) );

    // comments
    m_comments = new QTextEdit{metadata->comment(), this};
    InspectorSectionWidget* comments = new InspectorSectionWidget ("Comments");
    comments->addContent (m_comments);
    comments->expand(); // todo à enlever par la suite

    metadataLayout->addWidget(m_typeLb);
    metadataLayout->addWidget(descriptionWidget);
    metadataLayout->addWidget (m_colorButton);
    metadataLayout->addWidget (comments);
/*
    connect(m_scriptingNameLine &QLineEdit::textEdited,
            metadata,           &ModelMetadata::setName);
            */
}

QString MetadataWidget::scriptingName() const
{
    return m_scriptingNameLine->text();
}

void MetadataWidget::setScriptingName(QString arg)
{
    if (m_scriptingNameLine->text() == arg)
        return;

    m_scriptingNameLine->setText(arg);
    emit scriptingNameChanged(arg);
}

void MetadataWidget::setType(QString type)
{
    m_typeLb->setText(type);
}