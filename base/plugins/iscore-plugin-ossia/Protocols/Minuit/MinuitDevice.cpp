#include "MinuitDevice.hpp"
#include <API/Headers/Network/Node.h>
#include <API/Headers/Network/Device.h>
#include <API/Headers/Editor/Value.h>

#include "iscore2OSSIA.hpp"
#include "OSSIA2iscore.hpp"
using namespace iscore::convert;
using namespace OSSIA::convert;

MinuitDevice::MinuitDevice(const DeviceSettings &settings):
    OSSIADevice{settings},
    m_minuitSettings{[&] () {
    auto stgs = settings.deviceSpecificSettings.value<MinuitSpecificSettings>();
    return OSSIA::Minuit{stgs.host.toStdString(),
        stgs.inPort,
        stgs.outPort};
}()}

{
    using namespace OSSIA;

    m_dev = Device::create(m_minuitSettings, settings.name.toStdString());

    Q_ASSERT(m_dev);
}

bool MinuitDevice::canRefresh() const
{
    return true;
}

iscore::Node MinuitDevice::refresh()
{
    iscore::Node device_node;

    try {
    if(m_dev->updateNamespace())
    {
        // Make a device explorer node from the current state of the device.
        // First make the node corresponding to the root node.

        device_node.setDeviceSettings(settings());
        device_node.setAddressSettings(ToAddressSettings(*m_dev.get()));

        // Recurse on the children
        for(const auto& node : m_dev->children())
        {
            device_node.addChild(ToDeviceExplorer(*node.get()));
        }
    }
    }
    catch(std::runtime_error& e)
    {
        qDebug() << "Couldn't load the device:" << e.what();
    }

    device_node.deviceSettings().name = settings().name;

    return device_node;
}
