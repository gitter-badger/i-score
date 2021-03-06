#include "MIDIProtocolSettingsWidget.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>

MIDIProtocolSettingsWidget::MIDIProtocolSettingsWidget(QWidget* parent)
    : ProtocolSettingsWidget(parent)
{
    buildGUI();
}

void
MIDIProtocolSettingsWidget::buildGUI()
{
    QLabel* ioTypeLabel = new QLabel(tr("Device I/O type"), this);
    m_inButton = new QRadioButton(tr("Input"), this);
    m_outButton = new QRadioButton(tr("Output"), this);
    //radioButtons with same parent are auto-exclusive per default, i.e., behave as belonging to the same group.

    QLabel* midiDeviceLabel = new QLabel(tr("MIDI device"), this);
    m_deviceCBox = new QComboBox(this);

    QGridLayout* gLayout = new QGridLayout;
    gLayout->addWidget(ioTypeLabel, 0, 0, 1, 1);
    gLayout->addWidget(m_inButton, 0, 1, 1, 1);
    gLayout->addWidget(m_outButton, 0, 2, 1, 1);

    gLayout->addWidget(midiDeviceLabel, 1, 0, 1, 1);
    gLayout->addWidget(m_deviceCBox, 1, 1, 1, 2);

    setLayout(gLayout);

    connect(m_inButton, SIGNAL(clicked()), this, SLOT(updateInputDevices()));
    connect(m_outButton, SIGNAL(clicked()), this, SLOT(updateOutputDevices()));


    m_inButton->setChecked(true);  //TODO: QSettings
    updateInputDevices();
}

#include "MIDISpecificSettings.hpp"
DeviceSettings MIDIProtocolSettingsWidget::getSettings() const
{
    Q_ASSERT(m_deviceCBox);
    Q_ASSERT(m_inButton);

    DeviceSettings s;
    MIDISpecificSettings midi;
    s.name = m_deviceCBox->currentText();

    midi.io = m_inButton->isChecked()
              ? MIDISpecificSettings::IO::In
              : MIDISpecificSettings::IO::Out;
    s.deviceSpecificSettings = QVariant::fromValue(midi);

    return s;
}

void
MIDIProtocolSettingsWidget::setSettings(const DeviceSettings &settings)
{
    /*
    Q_ASSERT(settings.size() == 2);

    if(settings.at(1) == "In")
    {
        m_inButton->setChecked(true);
    }
    else
    {
        m_outButton->setChecked(true);
    }
*/
    int index = m_deviceCBox->findText(settings.name);

    if(index >= 0 && index < m_deviceCBox->count())
    {
        m_deviceCBox->setCurrentIndex(index);
    }

    if (settings.deviceSpecificSettings.canConvert<MIDISpecificSettings>())
    {
        MIDISpecificSettings midi = settings.deviceSpecificSettings.value<MIDISpecificSettings>();
        if(midi.io == MIDISpecificSettings::IO::In)
        {
            m_inButton->setChecked(true);
        }
        else
        {
            m_outButton->setChecked(true);
        }
    }
}



QList<QString> getAvailableInputMIDIDevices()
{
    QList<QString> list;
    list.append(QObject::tr("MIDI.1"));
    list.append(QObject::tr("MIDI.5"));
    list.append(QObject::tr("MIDI.Z"));
    return list;
}

QList<QString> getAvailableOutputMIDIDevices()
{
    QList<QString> list;
    list.append(QObject::tr("MIDI.out.2"));
    list.append(QObject::tr("MIDI.7"));
    list.append(QObject::tr("MIDI.AB"));
    list.append(QObject::tr("MIDI.Abcdef"));
    return list;
}


void
MIDIProtocolSettingsWidget::updateInputDevices()
{
    m_deviceCBox->clear();
    QList<QString> deviceNames = getAvailableInputMIDIDevices();
    m_deviceCBox->addItems(deviceNames);
}

void
MIDIProtocolSettingsWidget::updateOutputDevices()
{
    m_deviceCBox->clear();
    QList<QString> deviceNames = getAvailableOutputMIDIDevices();
    m_deviceCBox->addItems(deviceNames);
}
