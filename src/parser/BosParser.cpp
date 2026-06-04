//
// Created by vadim.tissen on 12.05.2026.
//

#include "bridge/parser/BosParser.hpp"
#include "bridge/core/types.hpp"
#include "bridge/json.hpp"
#include "bridge/parser/PrimeParser.hpp"


namespace bridge {
    BosParser::BosParser() {
        addRoute(SET_SALES, "POST", "/hs/Report/Sales/");
        addRoute(SET_METROLOGICAL, "POST", "/hs/Report/LevelFuel");
    };
}