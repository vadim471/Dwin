#ifndef ARKAIM_PAY_FISCAL_REGISTER_DEVICE_STATUS_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_DEVICE_STATUS_HPP

#include <string>

#include <DeviceStatusType.hpp>

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

class DeviceStatus
{
public:
    services::FiscalRegister::fiscal_register_interface::DeviceStatusType::StatusType DeviceStatusType; // device status type, look at the DeviceStatusType enum for more info

    std::string DeviceStatusName;        // device specified name of the status like a "PaperFinished", "PortBlocked", "PortNotFound" etc.
                                         // look at the DeviceStatusName class for predefined names
    std::string DeviceStatusDescription; // human readable device status description with detailed info

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(DeviceStatusType),
            CEREAL_NVP(DeviceStatusName),
            CEREAL_NVP(DeviceStatusDescription));
    }
};

}

#endif