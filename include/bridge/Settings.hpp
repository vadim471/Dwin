//
// Created by vadim.tissen on 12.12.2025.
//

#pragma once
#include <iostream>
#include <fstream>
#include <iostream>
#include "bridge/json.hpp"

#define LOAD_HEX(name) \
    s.dwin.name = std::stoul(j["dwin"].value(#name, "0x0000"), nullptr, 16)


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

    // --- Page 1 ---
        uint16_t vp_icon_trk;
        uint16_t vp_text_trk;
        uint16_t vp_pagination_trk;
        uint16_t vp_choose_trk;

    // --- Page PinPad ---
        uint16_t vp_pinpad;
        uint16_t vp_enterring_volume_order_pinpad;


    // --- TRK IDs ---
        uint16_t vp_trk_id_first_page;
        std::vector<uint16_t> vp_trk_id_pages;

    // --- Fuel type picture ---
        std::vector<uint16_t> vp_product_id_pages;

    // --- Fuel type text ---
        std::vector<uint16_t> vp_product_text_pages;

    // --- Current order volume ---
        std::vector<uint16_t> vp_current_order_volume_pages;

    // --- Current order amount ---
        std::vector<uint16_t> vp_current_order_amount_pages;

    // --- Current fuel type price ---
        std::vector<uint16_t> vp_current_fuel_type_price_pages;

    // --- Current date time ---
        std::vector<uint16_t> vp_current_date_time_pages;

    // --- Last order (page 2) ---
        uint16_t vp_volume_last_order;
        uint16_t vp_price_last_order;
        uint16_t vp_current_price_per_liter_last_order;

        uint16_t vp_basic_touch;
        uint16_t vp_next_button;

    // --- Current values ---
        uint16_t vp_chosen_order_volume;
        uint16_t vp_current_order_amount;
        uint16_t vp_current_fuel_volume;
        uint16_t vp_current_price_amount;

        uint16_t vp_progress_order_bar_eleventh_page;
        uint16_t vp_progress_bar_percent_text_eleventh_page;
        uint16_t vp_progress_order_bar_twelvth_page;
        uint16_t vp_progress_bar_percent_text_twelvth_page;

    // --- Pages ---
        int page_choose_trk;
        int page_set_nozzle_into_gasoline;
        int page_put_card_or_scan_code;
        int page_write_volume_order_pinpad;
        int page_set_fuel_volume;
        int page_processing_fuel_card;
        int page_success_processing_fuel_card;
        int page_ready_for_fuelling;
        int page_set_order;
        int page_accept_order;
        int page_fuel_in_progress;
        int page_fuel_ended;
        int page_good_trip;

    // --- Icons ---
        int icon_dispenser_nozzle_up;
        int icon_dispenser_idle;
        int icon_dispenser_order_end;

        int icon_fuel_type_92;
        int icon_fuel_type_95;
        int icon_fuel_type_98;
        int icon_fuel_type_100;
        int icon_fuel_type_dt;

    // --- Text length ---
        int text_len_trk_id;
        int text_len_order_amount;
        int text_len_order_volume;
        int text_len_fuel_type;
        int text_len_percent_progress_bar;
        int text_len_date_time_footer;
} dwin;


    struct BusinessLogic {
        int sleep_after_chosen_trk_page;
    } business_logic;

    struct APIDispenser {
        std::string authorize;
        std::string close;
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

            auto getHex = [&](const std::string& key) -> uint16_t {

                if (!j["dwin"].contains(key)) return 0;

                std::string val = j["dwin"].value(key, "0x0000");
                return static_cast<uint16_t>(std::stoul(val, nullptr, 16));
            };

            s.serial.port = j["serial"]["port"];
            s.serial.baud_rate = j["serial"]["baud_rate"];

            s.server.ip = j["server"]["ip"];
            s.server.port = j["server"]["port"];
            s.server.port_str = std::to_string(s.server.port);
            s.server.host = j["server"]["host_header"];
            s.server.username = j["server"]["username"];
            s.server.password = j["server"]["password"];

            LOAD_HEX(vp_icon_trk);
            LOAD_HEX(vp_text_trk);
            LOAD_HEX(vp_pagination_trk);
            LOAD_HEX(vp_choose_trk);
            LOAD_HEX(vp_pinpad);
            LOAD_HEX(vp_enterring_volume_order_pinpad);

            LOAD_HEX(vp_trk_id_first_page);
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_second_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_third_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_forth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_fifth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_sixth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_seventh_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_eighth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_ninth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_tenth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_eleventh_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_twelvth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_thirteenth_page"));

            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_third_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_forth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_fifth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_sixth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_seventh_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_eighth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_ninth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_tenth_page"));

            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_sixth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_seventh_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_eighth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_ninth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_tenth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_eleventh_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_twelvth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_second_type_twelvth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_thirteenth_page"));
            s.dwin.vp_current_order_volume_pages.push_back(getHex("vp_current_order_volume_second_type_thirteenth_page"));

            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_sixth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_seventh_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_eighth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_ninth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_tenth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_eleventh_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_twelvth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_second_type_twelvth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_thirteenth_page"));
            s.dwin.vp_current_order_amount_pages.push_back(getHex("vp_current_order_amount_second_type_thirteenth_page"));

            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_third_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_forth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_fifth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_sixth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_seventh_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_eighth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_ninth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_tenth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_eleventh_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_twelvth_page"));
            s.dwin.vp_current_fuel_type_price_pages.push_back(getHex("vp_current_fuel_type_price_thirteenth_page"));

            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_first_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_second_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_third_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_forth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_fifth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_sixth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_seventh_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_eighth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_ninth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_tenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_eleventh_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twelvth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirteenth_page"));

            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_eleventh_page"));
            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_twelvth_page"));
            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_thirteenth_page"));

            LOAD_HEX(vp_volume_last_order);
            LOAD_HEX(vp_price_last_order);
            LOAD_HEX(vp_current_price_per_liter_last_order);
            LOAD_HEX(vp_chosen_order_volume);
            LOAD_HEX(vp_current_order_amount);
            LOAD_HEX(vp_current_fuel_volume);
            LOAD_HEX(vp_current_price_amount);
            LOAD_HEX(vp_progress_order_bar_eleventh_page);
            LOAD_HEX(vp_progress_bar_percent_text_eleventh_page);
            LOAD_HEX(vp_progress_order_bar_twelvth_page);
            LOAD_HEX(vp_progress_bar_percent_text_twelvth_page);

            LOAD_HEX(vp_basic_touch);
            LOAD_HEX(vp_next_button);

            s.dwin.page_choose_trk                     = j["dwin"]["page_choose_trk"];
            s.dwin.page_set_nozzle_into_gasoline       = j["dwin"]["page_set_nozzle_into_gasoline"];
            s.dwin.page_put_card_or_scan_code          = j["dwin"]["page_put_card_or_scan_code"];
            s.dwin.page_write_volume_order_pinpad      = j["dwin"]["page_write_volume_order_pinpad"];
            s.dwin.page_set_fuel_volume                = j["dwin"]["page_set_fuel_volume"];
            s.dwin.page_processing_fuel_card           = j["dwin"]["page_processing_fuel_card"];
            s.dwin.page_success_processing_fuel_card   = j["dwin"]["page_success_processing_fuel_card"];
            s.dwin.page_ready_for_fuelling             = j["dwin"]["page_ready_for_fuelling"];
            s.dwin.page_set_order                      = j["dwin"]["page_set_order"];
            s.dwin.page_accept_order                   = j["dwin"]["page_accept_order"];
            s.dwin.page_fuel_in_progress               = j["dwin"]["page_fuel_in_progress"];
            s.dwin.page_fuel_ended                     = j["dwin"]["page_fuel_ended"];
            s.dwin.page_good_trip                      = j["dwin"]["page_good_trip"];


            s.dwin.icon_dispenser_nozzle_up = j["dwin"]["icon_dispenser_nozzle_up"];
            s.dwin.icon_dispenser_idle     = j["dwin"]["icon_dispenser_idle"];
            s.dwin.icon_dispenser_order_end= j["dwin"]["icon_dispenser_order_end"];

            s.dwin.icon_fuel_type_92   = j["dwin"]["icon_fuel_type_92"];
            s.dwin.icon_fuel_type_95   = j["dwin"]["icon_fuel_type_95"];
            s.dwin.icon_fuel_type_98   = j["dwin"]["icon_fuel_type_98"];
            s.dwin.icon_fuel_type_100  = j["dwin"]["icon_fuel_type_100"];
            s.dwin.icon_fuel_type_dt   = j["dwin"]["icon_fuel_type_dt"];

            s.dwin.text_len_trk_id = j["dwin"]["text_len_trk_id"];
            s.dwin.text_len_order_amount = j["dwin"]["text_len_order_amount"];
            s.dwin.text_len_order_volume = j["dwin"]["text_len_order_volume"];
            s.dwin.text_len_fuel_type = j["dwin"]["text_len_fuel_type"];
            s.dwin.text_len_percent_progress_bar = j["dwin"]["text_len_percent_progress_bar"];
            s.dwin.text_len_date_time_footer = j["dwin"]["text_len_date_time_footer"];

            s.dwin.icon_dispenser_idle = j["dwin"]["icon_dispenser_idle"];
            s.dwin.icon_dispenser_nozzle_up = j["dwin"]["icon_dispenser_nozzle_up"];
            s.dwin.icon_dispenser_order_end = j["dwin"]["icon_dispenser_order_end"];

            s.business_logic.sleep_after_chosen_trk_page = j["business"]["sleep_after_chosen_trk_page"];

            s.APIDispenser.authorize = j["api.dispenser_command"]["authorize"];
            s.APIDispenser.close = j["api.dispenser_command"]["close"];
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