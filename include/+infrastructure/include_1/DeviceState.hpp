#ifndef ARKAIM_PAY_FISCAL_REGISTER_DEVICE_STATE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_DEVICE_STATE_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

class DeviceState
{

public:
    bool IsChannelReady;
    bool IsComPortExist;
    bool IsComPortOpen;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(IsChannelReady),
            CEREAL_NVP(IsComPortExist),
            CEREAL_NVP(IsComPortOpen));
    }
};

}

#endif