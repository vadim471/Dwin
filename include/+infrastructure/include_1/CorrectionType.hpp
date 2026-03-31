#ifndef ARKAIM_PAY_FISCAL_REGISTER_CORRECTIONTYPE_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_CORRECTIONTYPE_HPP

#include <cstdint>

namespace services::FiscalRegister::fiscal_register_interface
{

enum CorrectionType : uint8_t
    {
        FakeSale,
        ZeroTotal,
        FakeRound,
        IncreasedPrecision,
        TwoDigitsPrecision
    };

}

    #endif
