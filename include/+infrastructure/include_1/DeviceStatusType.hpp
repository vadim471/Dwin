#ifndef ARKAIM_PAY_FISCAL_REGISTER_DEVICE_STATUS_TYPE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_DEVICE_STATUS_TYPE_HPP

namespace services::FiscalRegister::fiscal_register_interface::DeviceStatusType
{

enum StatusType
    {
        Error = 0,    // device malfunction that prevents ANY device usage
        Warning,      // may be cause of the device malfunction in future
        Service,      // device service status like a "FiscalRegisterShiftClosed", "FiscalRegisterNeedFinalyzeShift"
                      // in it device can be used ONLY in the maintenance mode, customer service does NOT allowed
        Info,          // device trace info like a shift open date, uptime etc.
        Trace          // trace device statuses
    };

}

#endif