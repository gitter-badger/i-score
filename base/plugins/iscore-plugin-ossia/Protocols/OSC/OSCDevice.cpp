#include "OSCDevice.hpp"
#include <API/Headers/Network/Protocol.h>
#include <API/Headers/Network/Device.h>
OSCDevice::OSCDevice(const DeviceSettings &stngs):
    OSSIADevice{stngs}
{
    using namespace OSSIA;

    auto settings = stngs.deviceSpecificSettings.value<OSCSpecificSettings>();
    OSSIA::OSC oscDeviceParameter{settings.host.toStdString(), settings.inputPort, settings.outputPort};
    m_dev = OSSIA::Device::create(oscDeviceParameter);

    Q_ASSERT(m_dev);
}
