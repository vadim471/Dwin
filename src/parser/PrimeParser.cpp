//
// Created by vadim.tissen on 10.12.2025.
//

#include "bridge/parser/PrimeParser.hpp"
#include "bridge/json.hpp"
#include "bridge/core/types.hpp"
#include "bridge/core/constant.hpp"


namespace bridge {
    PrimeParser::PrimeParser() {
        //GET
        addRoute(GET_EVENTS, "GET", "/api/v3/events?last_id=ID"); //GET /events?last_id=N&max=M  on_dsp_status_changed
        addRoute(GET_DEVICES, "GET", "/api/v3/devices"); // get all devices and get
        addRoute(GET_IO_PARAMETERS, "GET", "/api/v3/devices/ID/parameters"); //replace ID
        addRoute(GET_DISPENSER_STATUS, "GET", "/api/v3/devices/ID/status");
        addRoute(GET_PRODUCTS, "GET", "/api/v3/products");
        addRoute(GET_TASKS, "GET", "/api/v3/tasks/ID");
        addRoute(GET_TANKERS, "GET", "/api/v3/tankers");

        //POST
        addRoute(CREATE_ORDER, "POST", "/api/v3/devices/ID/order");
        addRoute(SET_COMMAND, "POST", "/api/v3/devices/ID/command");

        //NOT YET IMPLEMENTED
        addRoute("set_io_parameters", "POST", "");
    }
}
