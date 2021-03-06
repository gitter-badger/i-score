#include "AddressEditDialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "Common/AddressSettings/AddressSettingsFactory.hpp"
#include "Common/AddressSettings/Widgets/AddressSettingsWidget.hpp"

AddressEditDialog::AddressEditDialog(QWidget* parent)
    : QDialog(parent),
      m_addressWidget(nullptr), m_index(-1)
{
    setModal(true);
    buildGUI();
}

AddressEditDialog::~AddressEditDialog()
{

}

void
AddressEditDialog::buildGUI()
{
    QLabel* nameLabel = new QLabel(tr("Name"), this);
    m_nameEdit = new QLineEdit(this);

    QLabel* valueTypeLabel = new QLabel(tr("Value type"), this);
    m_valueTypeCBox = new QComboBox(this);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


    m_gLayout = new QGridLayout;
    m_gLayout->addWidget(nameLabel, 0, 0, 1, 1);
    m_gLayout->addWidget(m_nameEdit, 0, 1, 1, 1);

    m_gLayout->addWidget(valueTypeLabel, 1, 0, 1, 1);
    m_gLayout->addWidget(m_valueTypeCBox, 1, 1, 1, 1);

    //keep one row for m_addressWidget

    m_gLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setLayout(m_gLayout);

    initAvailableValueTypes(); //populate m_valueTypeCBox

    connect(m_valueTypeCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNodeWidget()));

    if(m_valueTypeCBox->count() > 0)
    {
        Q_ASSERT(m_valueTypeCBox->currentIndex() == 0);
        updateNodeWidget();
    }

    m_nameEdit->setText("addr");
}

void
AddressEditDialog::initAvailableValueTypes()
{
    Q_ASSERT(m_valueTypeCBox);

    m_valueTypeCBox->addItems(AddressSettingsFactory::instance().getAvailableValueTypes());

    //initialize previous settings
    m_previousSettings.clear();

    for(int i = 0; i < m_valueTypeCBox->count(); ++i)
    {
        m_previousSettings.append(AddressSettings());
    }

    m_index = m_valueTypeCBox->currentIndex();
}

void
AddressEditDialog::updateNodeWidget()
{
    Q_ASSERT(m_valueTypeCBox);

    if(m_addressWidget)
    {
        Q_ASSERT(m_index < m_valueTypeCBox->count());
        Q_ASSERT(m_index < m_previousSettings.count());

        m_previousSettings[m_index] = m_addressWidget->getSettings();
        delete m_addressWidget;
    }

    m_index = m_valueTypeCBox->currentIndex();

    const QString valueType = m_valueTypeCBox->currentText();
    m_addressWidget = AddressSettingsFactory::instance().getValueTypeWidget(valueType);

    if(m_addressWidget)
    {
/*
        //set previous settings for this protocol if any
        if(! m_previousSettings.at(m_index).name.isEmpty())
        {
            m_addressWidget->setSettings(m_previousSettings.at(m_index));
        }
//*/
        m_gLayout->addWidget(m_addressWidget, 2, 0, 1, 2);
        updateGeometry();
    }

}


AddressSettings AddressEditDialog::getSettings() const
{
    AddressSettings settings;

    if(m_addressWidget)
    {
        settings = m_addressWidget->getSettings();
    }
    else
    {
        // Int by default
        settings.value = 0;
    }

    settings.name = m_nameEdit->text();

    return settings;
}

AddressSettings AddressEditDialog::makeDefaultSettings()
{
    static AddressSettings defaultSettings
            = [] () {
        AddressSettings s;
        s.value = 0;
        s.domain.min = 0;
        s.domain.max = 0;
        return s;
    }();

    return defaultSettings;
}

void
AddressEditDialog::setSettings(const AddressSettings& settings)
{
    const QString name = settings.name;
    m_nameEdit->setText(name);

    QString type;
    switch(QMetaType::Type(settings.value.type()))
    {
        case QMetaType::Int:
            type = "Int";
            break;
        case QMetaType::Float:
            type = "Float";
            break;
        case QMetaType::QString:
            type = "String";
            break;
        case QMetaType::Bool:
            type = "Bool";
            break;
        case QMetaType::Char:
            type = "Char";
            break;
        case QMetaType::QVariantList:
            type = "Tuple";
            break;
        default:
            type = "None";
            break;
    }

    const int index = m_valueTypeCBox->findText(type);
    Q_ASSERT(index != -1);
    Q_ASSERT(index < m_valueTypeCBox->count());
    m_valueTypeCBox->setCurrentIndex(index);  //will emit currentIndexChanged(int) & call slot

    m_addressWidget->setSettings(settings);

}
