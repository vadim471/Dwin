#pragma once

#include <TypeDecimal.hpp>

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace services::FiscalRegister::cl2::Data
{

struct AddCashToBoxData
{
    Decimal CashSum;

    template <class Archive>
    void save(Archive &ar) const
    {
        std::string s = decimal_to_string(CashSum);
        ar(CEREAL_NVP(s));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        std::string s;
        ar(CEREAL_NVP(s));
        CashSum = string_to_decimal(s);
    }
};

}
