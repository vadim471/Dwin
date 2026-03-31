#ifndef ARKAIM_PAY_FISCAL_REGISTER_INCASSATION_DATA_HPP
#define ARKAIM_PAY_FISCAL_REGISTER_INCASSATION_DATA_HPP

#include <string>

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <TypeDecimal.hpp>

namespace services::FiscalRegister::cl2::Data
{
    
struct IncassationData
{
    Decimal IncassationSum;

    template <class Archive>
    void save(Archive &ar) const
    {
        std::string sum = decimal_to_string(IncassationSum);
        ar(CEREAL_NVP(sum));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        std::string sum;
        ar(CEREAL_NVP(sum));
        IncassationSum = string_to_decimal(sum);
    }
};

}

#endif