#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <string>

using Decimal = boost::multiprecision::cpp_dec_float_50;

inline std::string decimal_to_string(const Decimal& d) 
{
    return d.str();
}

inline Decimal string_to_decimal(const std::string& s) 
{
    return Decimal(s);
}
