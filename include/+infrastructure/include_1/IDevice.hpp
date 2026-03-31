#ifndef ARKAIM_PAY_FISCAL_REGISTER_I_DEVICE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_I_DEVICE_HPP

#include <KkmSettings.hpp>

#include <DeviceInfoData.hpp>
#include <DeviceState.hpp>
//#include <itp/enum.h>

namespace services::FiscalRegister::fiscal_register_interface
{

    template <typename TSetupData>
    class IDevice
    {
    public:
        /// <summary>
        /// Возвращает состояние устройства
        /// </summary>
        /// <returns></returns>
        virtual services::FiscalRegister::fiscal_register_interface::DeviceState GetDeviceState() = 0;

        /// <summary>
        /// Возвращает расширенное состояние устройства
        /// </summary>
        /// <returns></returns>
        virtual services::FiscalRegister::fiscal_register_interface::DeviceInfoData GetDeviceInfo() = 0;

        /// <summary>
        /// Установка параметров инициализации устройства
        /// </summary>
        /// <param name="data">Данные инициализации</param>
        virtual void Setup(TSetupData data) = 0;

        // virtual void SetNodeStatus(itp::cl2_device_status_t nodeStatus) = 0;

        // virtual itp::cl2_device_status_t GetNodeStatus() = 0;
    };

}

#endif