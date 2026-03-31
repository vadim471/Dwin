#ifndef ARKAIM_PAY_FISCAL_REGISTER_EXTENDED_RESPONSE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_EXTENDED_RESPONSE_HPP

#include <ExtendedResponseStatus.hpp>

namespace services::FiscalRegister::fiscal_register_interface
{

template <typename T> class ExtendedResponse
    {
        public:
        T Response;
        ExtendedResponseStatus Status;

        ExtendedResponse(T response, ExtendedResponseStatus status)
        {
            Response = response;
            Status = status;
        }
    };

}

#endif
