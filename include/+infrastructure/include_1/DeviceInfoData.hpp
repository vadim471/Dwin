#ifndef ARKAIM_PAY_FISCAL_REGISTER_DEVICE_INFO_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_DEVICE_INFO_DATA_HPP

#include <string>
#include <vector>

#include <DeviceType.hpp>
#include <DeviceStatus.hpp>

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

class DeviceInfoData
{
public:
    std::string DeviceName;
    std::string DeviceDescription;
    services::FiscalRegister::fiscal_register_interface::DeviceType::DeviceType DeviceType;
    std::vector<services::FiscalRegister::fiscal_register_interface::DeviceStatus> DeviceStatuses;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(DeviceName),
            CEREAL_NVP(DeviceDescription),
            CEREAL_NVP(DeviceType),
            CEREAL_NVP(DeviceStatuses));
    }
};

}

#endif