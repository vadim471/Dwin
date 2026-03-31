#ifndef ARKAIM_PAY_FISCAL_REGISTER_PAYMENTTYPE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_PAYMENTTYPE_HPP

#include <cstdint>

namespace services::FiscalRegister::fiscal_register_interface
{

enum PaymentType : uint8_t
    {
        Cash = 0,
        Voucher,
        BankCard,
        FuelCard,
        BonusCard,
        BonusCardAccrual,
        None = 99
    };

}

#endif