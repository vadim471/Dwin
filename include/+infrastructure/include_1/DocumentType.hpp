#ifndef ARKAIM_PAY_FISCAL_REGISTER_DOCUMENTTYPE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_DOCUMENTTYPE_HPP

#include <cstdint>

namespace services::FiscalRegister::fiscal_register_interface
{

enum DocumentType : uint8_t
{
    Sale = 0,
    Return
};

}

#endif