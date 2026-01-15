//
// Created by vadim.tissen on 12.12.2025.
//

#pragma once
#include <iostream>
#include <fstream>
#include <iostream>
#include "bridge/json.hpp"

using json = nlohmann::json;

struct Settings {
    struct Serial {
        std::string port;
        unsigned int baud_rate;
    } serial;

    struct Server {
        std::string ip;
        int port; // socket
        std::string port_str; // asio
        std::string host; // http header
        std::string username; //digest user
        std::string password; //digest pass
    } server;

    struct Dwin {
        uint16_t vp_fuel_type;
        uint16_t vp_sum_order;
        uint16_t vp_volume_order;
        uint16_t vp_price_order;
        uint16_t vp_fuel_volume;
        uint16_t vp_current_fuel_volume;
        uint16_t vp_current_order_amount;
        uint16_t vp_current_price_amount;
        uint16_t vp_progress_order_bar;

        int page_chosen_fuel;
        int page_main_order;
        int page_service_menu;
        int page_set_fuel_price;
        int page_get_tanker_volume;
        int page_fuel_progress;
        int page_get_fuel_card;
        int page_set_fuel_volume_order;
    } dwin;

    struct BusinessLogic {
        int sleep_after_chosen_fuel_type;
    } business_logic;

    struct APIDispenser {
        std::string authorize;
    } APIDispenser;

    static Settings load(const std::string& filename) {
        Settings s;
        try {
            std::ifstream f(filename);
            if (!f.is_open()) {
                throw std::runtime_error("Config file not found: " + filename);
            }
            json j;
            f >> j;

            s.serial.port = j["serial"]["port"];
            s.serial.baud_rate = j["serial"]["baud_rate"];

            s.server.ip = j["server"]["ip"];
            s.server.port = j["server"]["port"];
            s.server.port_str = std::to_string(s.server.port);
            s.server.host = j["server"]["host_header"];
            s.server.username = j["server"]["username"];
            s.server.password = j["server"]["password"];

            s.dwin.vp_fuel_type = std::stoul(j["dwin"].value("vp_fuel_type", "0x0000"), nullptr, 16);;
            s.dwin.vp_sum_order = std::stoul(j["dwin"].value("vp_sum_order", "0x0000"), nullptr, 16);
            s.dwin.vp_volume_order = std::stoul(j["dwin"].value("vp_volume_order", "0x0000"), nullptr, 16);
            s.dwin.vp_price_order = std::stoul(j["dwin"].value("vp_price_order", "0x0000"), nullptr, 16);
            s.dwin.vp_fuel_volume = std::stoul(j["dwin"].value("vp_fuel_volume", "0x0000"), nullptr, 16);
            s.dwin.vp_current_fuel_volume = std::stoul(j["dwin"].value("vp_current_fuel_volume", "0x0000"), nullptr, 16);
            s.dwin.vp_current_order_amount = std::stoul(j["dwin"].value("vp_current_order_amount", "0x0000"), nullptr, 16);
            s.dwin.vp_current_price_amount = std::stoul(j["dwin"].value("vp_current_price_amount", "0x0000"), nullptr, 16);
            s.dwin.vp_progress_order_bar = std::stoul(j["dwin"].value("vp_progress_order_bar", "0x0000"), nullptr, 16);

            s.dwin.page_chosen_fuel = j["dwin"]["page_chosen_fuel"];
            s.dwin.page_main_order = j["dwin"]["page_main_order"];
            s.dwin.page_service_menu = j["dwin"]["page_service_menu"];
            s.dwin.page_set_fuel_price = j["dwin"]["page_set_fuel_price"];
            s.dwin.page_get_fuel_card = j["dwin"]["page_get_fuel_card"];
            s.dwin.page_fuel_progress = j["dwin"]["page_fuel_progress"];
            s.dwin.page_get_tanker_volume = j["dwin"]["page_get_tanker_volume"];
            s.dwin.page_chosen_fuel = j["dwin"]["page_chosen_fuel"];
            s.dwin.page_set_fuel_volume_order = j["dwin"]["page_set_fuel_volume_order"];

            s.business_logic.sleep_after_chosen_fuel_type = j["business"]["sleep_after_chosen_fuel_page"];

            s.APIDispenser.authorize = j["api.dispenser_command"]["authorize"];


        } catch (const std::exception& e) {
            std::cerr << "[Config] Error loading settings: " << e.what() << std::endl;
            // default
            s.serial.port = "/dev/ttyUSB0";
            s.serial.baud_rate = 115200;
            s.server.ip = "127.0.0.1";
            s.server.port_str = "80";
            s.server.host = "localhost";
        }
        return s;
    }
};