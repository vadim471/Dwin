#ifndef ARKAIM_PAY_FISCAL_REGISTER_GET_FISCAL_COUNTERS_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_GET_FISCAL_COUNTERS_DATA_HPP

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::cl2::Data
{
    struct GetFiscalCountersData
    {
        bool IsEnableLog;

        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(CEREAL_NVP(IsEnableLog));
        }
    };
}

#endif