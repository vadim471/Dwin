#ifndef INITPLUS_TOOLS_DECIMAL_JSON_HPP_INCLUDED
#define INITPLUS_TOOLS_DECIMAL_JSON_HPP_INCLUDED

#include "decimal.hpp"

#include <nlohmann/json.hpp>

namespace initp {
namespace tools {

template<typename T, bool F, uint8_t R>
void to_json(nlohmann::json& j, const decimal<T, F, R>& d) {
    j = d.to_string();

#ifndef WSL_BUILD
    j.set_quoted_string(true);
#endif // WSL_BUILD
}

}}

#endif // INITPLUS_TOOLS_DECIMAL_JSON_HPP_INCLUDED
