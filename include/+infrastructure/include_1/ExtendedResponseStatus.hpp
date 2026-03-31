#ifndef ARKAIM_PAY_FISCAL_REGISTER_EXTENDED_RESPONSE_STATUS_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_EXTENDED_RESPONSE_STATUS_HPP

namespace services::FiscalRegister::fiscal_register_interface
{

enum ExtendedResponseStatus
        {
            Success = 0,
            UnknownError,
            NotSupported
        };

}

#endif