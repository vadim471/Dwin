#ifndef ARKAIM_PAY_FISCAL_REGISTER_GET_SHIFT_SALE_INFO_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_GET_SHIFT_SALE_INFO_DATA_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::cl2::Data
{
    struct GetShiftSaleInfoData
    {
        int TerminalNumber;
        time_t StartShift;
        bool IsEndOfShift;

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(CEREAL_NVP(TerminalNumber),
                CEREAL_NVP(StartShift),
                CEREAL_NVP(IsEndOfShift)
            );
        }
    };
}

#endif