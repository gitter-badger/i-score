#pragma once
#include "Protocols/OSSIADevice.hpp"
#include "MinuitSpecificSettings.hpp"
#include <API/Headers/Network/Protocol.h>
#include <QThread>
namespace OSSIA
{
class Device;
}
class MinuitDevice : public OSSIADevice
{
    public:
        MinuitDevice(const DeviceSettings& settings);
        bool canRefresh() const override;
        iscore::Node refresh() override;

    private:
        OSSIA::Minuit m_minuitSettings;
        QThread m_updateThread;
};
