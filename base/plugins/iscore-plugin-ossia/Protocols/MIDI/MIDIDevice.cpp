#include "MIDIDevice.hpp"
#include <API/Headers/Network/Protocol.h>
#include <API/Headers/Network/Device.h>
MIDIDevice::MIDIDevice(const DeviceSettings &settings):
    OSSIADevice{settings}
{
    using namespace OSSIA;
    auto stgs = settings.deviceSpecificSettings.value<MIDISpecificSettings>();
    Midi parameters;

    m_dev = Device::create(parameters, settings.name.toStdString());
    Q_ASSERT(m_dev);
}
