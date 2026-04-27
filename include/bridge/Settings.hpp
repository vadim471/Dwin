//
// Created by vadim.tissen on 12.12.2025.
//

#pragma once
#include <iostream>
#include <fstream>
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
        uint16_t vp_choose_multi_trk;

    // --- Page PinPad ---
        uint16_t vp_fuel_volume_pinpad;
        uint16_t vp_enterring_volume_order_pinpad;

    // --- Page Number Document ---
        uint16_t vp_button_number_document_pinpad;
        uint16_t vp_text_number_document_pinpad;

    // --- Page PinPad Balance Card
        uint16_t vp_button_pincode_balance_card_pinpad;
        uint16_t vp_text_pincode_balance_card_pinpad;

    // --- Page PinPad Set Fuel Price
        uint16_t vp_set_fuel_price_pinpad;
        uint16_t vp_text_enter_fuel_price;

    // --- Page Choose Amount TRK ---
        uint16_t vp_button_choose_amount_trk;

    // --- Page Fuel Reception ---
        uint16_t vp_text_reception_gauge_filling_percent;
        uint16_t vp_text_reception_gauge_id;
        uint16_t vp_icon_reception_gauge;
        uint16_t vp_text_upper_level_reception_gauge_integer;
        uint16_t vp_text_upper_level_reception_gauge_decimal;
        uint16_t vp_text_upper_volume_reception_gauge_integer;
        uint16_t vp_text_upper_volume_reception_gauge_decimal;
        uint16_t vp_text_weight_reception_gauge_integer;
        uint16_t vp_text_weight_reception_gauge_decimal;
        uint16_t vp_text_density_reception_gauge_integer;
        uint16_t vp_text_density_reception_gauge_decimal;
        uint16_t vp_text_lower_level_reception_gauge_integer;
        uint16_t vp_text_lower_level_reception_gauge_decimal;
        uint16_t vp_text_lower_volume_reception_gauge_integer;
        uint16_t vp_text_lower_volume_reception_gauge_decimal;
        uint16_t vp_text_total_volume_reception_gauge_integer;
        uint16_t vp_text_total_volume_reception_gauge_decimal;
        uint16_t vp_button_finish_reception_fuel;

    // --- Page Choose Fuel Tanker For Reception ---
        uint16_t vp_text_choose_reception_gauge_filling_percent;
        uint16_t vp_text_choose_reception_gauge_id;
        uint16_t vp_icon_choose_reception_gauge;
        uint16_t vp_text_upper_level_choose_reception_gauge_integer;
        uint16_t vp_text_upper_level_choose_reception_gauge_decimal;
        uint16_t vp_text_upper_volume_choose_reception_gauge_integer;
        uint16_t vp_text_upper_volume_choose_reception_gauge_decimal;
        uint16_t vp_text_weight_choose_reception_gauge_integer;
        uint16_t vp_text_weight_choose_reception_gauge_decimal;
        uint16_t vp_text_density_choose_reception_gauge_integer;
        uint16_t vp_text_density_choose_reception_gauge_decimal;
        uint16_t vp_text_lower_level_choose_reception_gauge_integer;
        uint16_t vp_text_lower_level_choose_reception_gauge_decimal;
        uint16_t vp_text_lower_volume_choose_reception_gauge_integer;
        uint16_t vp_text_lower_volume_choose_reception_gauge_decimal;
        uint16_t vp_text_total_volume_choose_reception_gauge_integer;
        uint16_t vp_text_total_volume_choose_reception_gauge_decimal;
        uint16_t vp_button_choose_reception_fuel;
        uint16_t vp_button_pagination_reception_tanker;


    // --- Page enter PinCode PinPad ---
        uint16_t vp_enter_pin_code_pinpad;
        uint16_t vp_text_enter_pincode;

    // --- Page enter service code PinPad ---
        uint16_t vp_set_service_pincode_pinpad;
        uint16_t vp_text_pinpad_service_code;

    // --- Page Service Menu
        uint16_t vp_button_service_menu;

    // --- Page editing Fuel Type
        std::vector<uint16_t> vp_text_fuel_price_edit_page;
        std::vector<uint16_t> vp_text_fuel_integer_price_edit_page;
        std::vector<uint16_t> vp_text_fuel_decimal_price_edit_page;

        uint16_t vp_button_fuel_edit_price_chosen;
        uint16_t vp_editing_fuel_type;
        uint16_t vp_editing_fuel_integer_part;
        uint16_t vp_editing_fuel_decimal_part;

    // --- Page Level Gauges Volume ---
        uint16_t vp_text_gauges_filling_percent;
        uint16_t vp_text_level_gauge_id;
        uint16_t vp_icon_level_gauges_volume;
        uint16_t vp_button_pagination_level_gauges;
        uint16_t vp_text_upper_level_gauges_integer;
        uint16_t vp_text_upper_level_gauges_decimal;
        uint16_t vp_text_upper_volume_gauges_integer;
        uint16_t vp_text_upper_volume_gauges_decimal;
        uint16_t vp_text_weight_gauge_integer;
        uint16_t vp_text_weight_gauge_decimal;
        uint16_t vp_text_density_gauge_integer;
        uint16_t vp_text_density_gauge_decimal;
        uint16_t vp_text_lower_level_gauge_integer;
        uint16_t vp_text_lower_level_gauge_decimal;
        uint16_t vp_text_lower_volume_gauge_integer;
        uint16_t vp_text_lower_volume_gauge_decimal;
        uint16_t vp_text_total_volume_gauge_integer;
        uint16_t vp_text_total_volume_gauge_decimal;

    // --- Page Uno TRK config
        uint16_t vp_button_choose_trk_uno_trk_config;
        uint16_t vp_icon_trk_uno;
        uint16_t vp_text_trk_uno;

    // --- Page Duo TRK config
        uint16_t vp_button_choose_left_trk_double_trk_config;
        uint16_t vp_button_choose_right_trk_double_trk_config;
        uint16_t vp_icon_trk_duo_left;
        uint16_t vp_text_trk_duo_left;
        uint16_t vp_icon_trk_duo_right;
        uint16_t vp_text_trk_duo_right;

    // --- Page Select Used TRK
        std::vector<uint16_t> vp_nums_selected_trk;
        std::vector<uint16_t> vp_values_selected_trk;
        uint16_t vp_text_select_trk_button;

    // --- Buttons ---
        uint16_t vp_button_cancel_transaction;

    // --- Page Service Menu ---
        uint16_t vp_back_button_service_menu;

    // --- Page Choose Fuel Type For Editing (second variable) ---
        uint16_t vp_icon_fuel_type_for_editing;
        uint16_t vp_fuel_price_for_editing_integer;
        uint16_t vp_fuel_price_for_editing_decimal;
        uint16_t vp_button_pagination_fuel_type_for_edit;
        uint16_t vp_button_chosen_fuel_type_for_edit;

    // --- TRK IDs ---
        uint16_t vp_trk_id_first_page;
        std::vector<uint16_t> vp_trk_id_pages;
        uint16_t vp_trk_id_second_page;

    // --- Fuel type picture ---
        std::vector<uint16_t> vp_product_id_pages;

    // --- Fuel type text ---
        std::vector<uint16_t> vp_product_text_pages;

    // --- Current order volume ---
        std::vector<uint16_t> vp_current_order_volume_integer_pages;
        std::vector<uint16_t> vp_current_order_volume_decimal_pages;

    // --- Current order amount ---
        std::vector<uint16_t> vp_current_order_amount_integer_pages;
        std::vector<uint16_t> vp_current_order_amount_decimal_pages;

    // --- Current fuel type price ---
        std::vector<uint16_t> vp_current_fuel_type_price_pages_integer;
        std::vector<uint16_t> vp_current_fuel_type_price_pages_decimal;

    // --- Current date time ---
        std::vector<uint16_t> vp_current_date_time_pages;

    // --- Last order (page 2) ---
        uint16_t vp_volume_last_order_integer;
        uint16_t vp_price_last_order_integer;
        uint16_t vp_current_price_per_liter_last_order_integer;

        uint16_t vp_volume_last_order_decimal;
        uint16_t vp_price_last_order_decimal;
        uint16_t vp_current_price_per_liter_last_order_decimal;

        uint16_t vp_basic_touch;


    // --- Current values ---
        uint16_t vp_chosen_order_volume; // кнопка быстрого выбора количества топлива (не пинпад).
        uint16_t vp_button_get_balance; // кнопка запроса баланса карты.

        uint16_t vp_current_order_amount_integer;
        std::vector<uint16_t> vp_current_fuel_volume_integer;
        uint16_t vp_current_order_amount_decimal;
        std::vector<uint16_t> vp_current_fuel_volume_decimal;

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
        int page_print_pin;
        int page_service_menu;
        int page_level_gauges_info;
        int page_level_gauge_info;
        int page_select_fuel_type_edit;
        int page_set_fuel_price;
        int page_set_amount_trk;
        int page_set_fuel_price_another_variable;
        int page_choose_trk_config_uno_trk;
        int page_choose_trk_config_double_trk;
        int page_error_creating_order;
        int page_error_low_fuel_level;
        int page_return_money_process;
        int page_return_money_process_end;
        int page_type_service_code;
        int page_select_used_trk;
        int page_return_money_cancel_transaction;
        int page_error_transaction_failed;
        int page_error_incorrect_pincode;

    // --- Icons ---
        int icon_dispenser_nozzle_up;
        int icon_dispenser_idle;
        int icon_dispenser_order_end;
        int icon_dispenser_fueling;
        int icon_dispenser_error;
        int icon_dispenser_halted;
        int icon_dispenser_locked;
        int icon_dispenser_offline;

        int icon_fuel_type_92;
        int icon_fuel_type_95;
        int icon_fuel_type_98;
        int icon_fuel_type_100;
        int icon_fuel_type_dt;

    // --- Text length ---
        int text_len_trk_id;
        int text_len_order_decimal;
        int text_len_order_integer;
        int text_len_fuel_type;
        int text_len_percent_progress_bar;
        int text_len_date_time_footer;
        int text_len_fuel_price;
        int text_len_fuel_integer;

    // --- Audio ID ---
        int audio_id_welcome_nozzle_up;
        int audio_id_fuelling_end;
        int audio_id_fuelling_interrupted;
} dwin;

    struct BusinessLogic {
        int sleep_after_chosen_trk_page;
        int waiting_dispenser_fuelling;
        int show_incorrect_pin_page;
    } business_logic;

    struct APIDispenser {
        std::string authorize;
        std::string close;
    } APIDispenser;

    struct GasStation {
        int amount_trk; // Количество колонок на АЗС.
        std::map<std::string, std::string> fuel_prices; // ID топлива - цена.
        int service_code;
        std::vector<std::string> used_trks;
        std::map<std::string, std::string> prime_standalone;
        std::map<std::string, std::string> processing_standalone;
    } gas_station;

    struct Pipe {
        std::string name;
    } pipe;

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

            // s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_second_page"));
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
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_fourteenth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_twentyfourth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_thirtyth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_thirtyfirst_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_twentysixth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_twentyseventh_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_thirtysecond_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_thirtyfifth_page"));
            s.dwin.vp_trk_id_pages.push_back(getHex("vp_trk_id_thirtyseventh_page"));

            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_third_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_forth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_fifth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_sixth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_seventh_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_eighth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_ninth_page"));
            s.dwin.vp_product_id_pages.push_back(getHex("vp_chosen_fuel_type_tenth_page"));

            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_sixth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_seventh_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_eighth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_ninth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_tenth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_eleventh_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_twelvth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_second_type_twelvth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_thirteenth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_second_type_thirteenth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_fourteenth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_twentysixth_page"));
            s.dwin.vp_current_order_volume_integer_pages.push_back(getHex("vp_current_order_volume_integer_twentyseventh_page"));

            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_sixth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_seventh_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_eighth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_ninth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_tenth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_eleventh_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_twelvth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_second_type_twelvth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_thirteenth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_second_type_thirteenth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_fourteenth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_twentysixth_page"));
            s.dwin.vp_current_order_volume_decimal_pages.push_back(getHex("vp_current_order_volume_decimal_twentyseventh_page"));

            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_sixth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_seventh_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_eighth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_ninth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_tenth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_eleventh_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_twelvth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_second_type_twelvth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_thirteenth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_second_type_thirteenth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_twentysixth_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_twentyseventh_page"));
            s.dwin.vp_current_order_amount_integer_pages.push_back(getHex("vp_current_order_amount_integer_thirtyth_page"));

            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_sixth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_seventh_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_eighth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_ninth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_tenth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_eleventh_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_twelvth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_second_type_twelvth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_thirteenth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_second_type_thirteenth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_twentysixth_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_twentyseventh_page"));
            s.dwin.vp_current_order_amount_decimal_pages.push_back(getHex("vp_current_order_amount_decimal_thirtyth_page"));

            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_third_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_forth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_fifth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_sixth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_seventh_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_eighth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_ninth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_tenth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_eleventh_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_twelvth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_thirteenth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_fourteenth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_twentysixth_page"));
            s.dwin.vp_current_fuel_type_price_pages_integer.push_back(getHex("vp_current_fuel_type_price_integer_twentyseventh_page"));

            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_third_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_forth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_fifth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_sixth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_seventh_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_eighth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_ninth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_tenth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_eleventh_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_twelvth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_thirteenth_page"));
            s.dwin.vp_current_fuel_type_price_pages_decimal.push_back(getHex("vp_current_fuel_type_price_decimal_fourteenth_page"));


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
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_fourteenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_fifteenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_sixteenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_seventeenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_eightteenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_ninteenth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyoneth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentytwoth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentythreeth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyfourth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyfifth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentysixth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyseventh_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyeighth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_twentyninth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtyth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtyfirst_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtysecond_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtythird_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtyfourth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtyfifth_page"));
            s.dwin.vp_current_date_time_pages.push_back(getHex("vp_current_date_time_footer_thirtyseventh_page"));

            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_eleventh_page"));
            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_twelvth_page"));
            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_thirteenth_page"));
            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_twentysixth_page"));
            s.dwin.vp_product_text_pages.push_back(getHex("vp_current_fuel_type_twentyseventh_page"));

            s.dwin.vp_current_fuel_volume_integer.push_back(getHex("vp_current_fuel_volume_integer"));
            s.dwin.vp_current_fuel_volume_integer.push_back(getHex("vp_current_fuel_volume_integer_twentysixth_page"));
            s.dwin.vp_current_fuel_volume_integer.push_back(getHex("vp_current_fuel_volume_integer_twentyseventh_page"));

            s.dwin.vp_current_fuel_volume_decimal.push_back(getHex("vp_current_fuel_volume_decimal"));
            s.dwin.vp_current_fuel_volume_decimal.push_back(getHex("vp_current_fuel_volume_decimal_twentysixth_page"));
            s.dwin.vp_current_fuel_volume_decimal.push_back(getHex("vp_current_fuel_volume_decimal_twentyseventh_page"));

            LOAD_HEX(vp_volume_last_order_integer);
            LOAD_HEX(vp_price_last_order_integer);
            LOAD_HEX(vp_current_price_per_liter_last_order_integer);

            LOAD_HEX(vp_volume_last_order_decimal);
            LOAD_HEX(vp_price_last_order_decimal);
            LOAD_HEX(vp_current_price_per_liter_last_order_decimal);

            LOAD_HEX(vp_chosen_order_volume);
            LOAD_HEX(vp_button_get_balance);
            LOAD_HEX(vp_current_order_amount_integer);
            LOAD_HEX(vp_current_order_amount_decimal);

            LOAD_HEX(vp_progress_order_bar_eleventh_page);
            LOAD_HEX(vp_progress_bar_percent_text_eleventh_page);
            LOAD_HEX(vp_progress_order_bar_twelvth_page);
            LOAD_HEX(vp_progress_bar_percent_text_twelvth_page);

            LOAD_HEX(vp_basic_touch);

            LOAD_HEX(vp_icon_trk);
            LOAD_HEX(vp_text_trk);
            LOAD_HEX(vp_pagination_trk);
            LOAD_HEX(vp_choose_multi_trk);
            LOAD_HEX(vp_fuel_volume_pinpad);
            LOAD_HEX(vp_enterring_volume_order_pinpad);
            LOAD_HEX(vp_button_number_document_pinpad);
            LOAD_HEX(vp_text_number_document_pinpad);
            LOAD_HEX(vp_set_fuel_price_pinpad);
            LOAD_HEX(vp_text_enter_fuel_price);
            LOAD_HEX(vp_enter_pin_code_pinpad);
            LOAD_HEX(vp_button_service_menu);
            LOAD_HEX(vp_button_choose_amount_trk);

            LOAD_HEX(vp_set_service_pincode_pinpad);
            LOAD_HEX(vp_text_pinpad_service_code);

            LOAD_HEX(vp_text_enter_pincode);

            LOAD_HEX(vp_text_gauges_filling_percent);
            LOAD_HEX(vp_text_level_gauge_id);
            LOAD_HEX(vp_button_pagination_level_gauges);
            LOAD_HEX(vp_icon_level_gauges_volume);
            LOAD_HEX(vp_text_upper_level_gauges_integer);
            LOAD_HEX(vp_text_upper_level_gauges_decimal);
            LOAD_HEX(vp_text_upper_volume_gauges_integer);
            LOAD_HEX(vp_text_upper_volume_gauges_decimal);
            LOAD_HEX(vp_text_weight_gauge_integer);
            LOAD_HEX(vp_text_weight_gauge_decimal);
            LOAD_HEX(vp_text_density_gauge_integer);
            LOAD_HEX(vp_text_density_gauge_decimal);
            LOAD_HEX(vp_text_lower_level_gauge_integer);
            LOAD_HEX(vp_text_lower_level_gauge_decimal);
            LOAD_HEX(vp_text_lower_volume_gauge_integer);
            LOAD_HEX(vp_text_lower_volume_gauge_decimal);
            LOAD_HEX(vp_text_total_volume_gauge_integer);
            LOAD_HEX(vp_text_total_volume_gauge_decimal);

            LOAD_HEX(vp_text_reception_gauge_filling_percent);
            LOAD_HEX(vp_text_reception_gauge_id);
            LOAD_HEX(vp_icon_reception_gauge);
            LOAD_HEX(vp_text_upper_level_reception_gauge_integer);
            LOAD_HEX(vp_text_upper_level_reception_gauge_decimal);
            LOAD_HEX(vp_text_upper_volume_reception_gauge_integer);
            LOAD_HEX(vp_text_upper_volume_reception_gauge_decimal);
            LOAD_HEX(vp_text_weight_reception_gauge_integer);
            LOAD_HEX(vp_text_weight_reception_gauge_decimal);
            LOAD_HEX(vp_text_density_reception_gauge_integer);
            LOAD_HEX(vp_text_density_reception_gauge_decimal);
            LOAD_HEX(vp_text_lower_level_reception_gauge_integer);
            LOAD_HEX(vp_text_lower_level_reception_gauge_decimal);
            LOAD_HEX(vp_text_lower_volume_reception_gauge_integer);
            LOAD_HEX(vp_text_lower_volume_reception_gauge_decimal);
            LOAD_HEX(vp_text_total_volume_reception_gauge_integer);
            LOAD_HEX(vp_text_total_volume_reception_gauge_decimal);
            LOAD_HEX(vp_button_finish_reception_fuel);



            LOAD_HEX(vp_button_fuel_edit_price_chosen);
            LOAD_HEX(vp_editing_fuel_type);
            LOAD_HEX(vp_editing_fuel_integer_part);
            LOAD_HEX(vp_editing_fuel_decimal_part);

            LOAD_HEX(vp_button_choose_left_trk_double_trk_config);
            LOAD_HEX(vp_button_choose_right_trk_double_trk_config);
            LOAD_HEX(vp_icon_trk_duo_left);
            LOAD_HEX(vp_text_trk_duo_left);
            LOAD_HEX(vp_icon_trk_duo_right);
            LOAD_HEX(vp_text_trk_duo_right);

            LOAD_HEX(vp_button_choose_trk_uno_trk_config);
            LOAD_HEX(vp_icon_trk_uno);
            LOAD_HEX(vp_text_trk_uno);


            s.dwin.vp_text_fuel_price_edit_page.push_back(getHex("vp_text_first_fuel_type_edit_price"));
            s.dwin.vp_text_fuel_price_edit_page.push_back(getHex("vp_text_second_fuel_type_edit_price"));
            s.dwin.vp_text_fuel_price_edit_page.push_back(getHex("vp_text_third_fuel_type_edit_price"));
            s.dwin.vp_text_fuel_price_edit_page.push_back(getHex("vp_text_fourth_fuel_type_edit_price"));
            s.dwin.vp_text_fuel_integer_price_edit_page.push_back(getHex("vp_text_first_fuel_integer_price_edit_price"));
            s.dwin.vp_text_fuel_integer_price_edit_page.push_back(getHex("vp_text_second_fuel_integer_price_edit_price"));
            s.dwin.vp_text_fuel_integer_price_edit_page.push_back(getHex("vp_text_third_fuel_integer_price_edit_price"));
            s.dwin.vp_text_fuel_integer_price_edit_page.push_back(getHex("vp_text_fourth_fuel_integer_price_edit_price"));
            s.dwin.vp_text_fuel_decimal_price_edit_page.push_back(getHex("vp_text_first_fuel_decimal_price_edit_price"));
            s.dwin.vp_text_fuel_decimal_price_edit_page.push_back(getHex("vp_text_second_fuel_decimal_price_edit_price"));
            s.dwin.vp_text_fuel_decimal_price_edit_page.push_back(getHex("vp_text_third_fuel_decimal_price_edit_price"));
            s.dwin.vp_text_fuel_decimal_price_edit_page.push_back(getHex("vp_text_fourth_fuel_decimal_price_edit_price"));

            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_first_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_second_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_third_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_fourth_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_fifth_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_sixth_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_seventh_line_num"));
            s.dwin.vp_nums_selected_trk.push_back(getHex("vp_text_choose_trk_eighth_line_num"));

            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_first_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_second_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_third_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_fourth_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_fifth_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_sixth_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_seventh_line_value"));
            s.dwin.vp_values_selected_trk.push_back(getHex("vp_text_choose_trk_eighth_line_value"));

            LOAD_HEX(vp_text_select_trk_button);
            LOAD_HEX(vp_trk_id_first_page);
            LOAD_HEX(vp_trk_id_second_page);

            LOAD_HEX(vp_button_cancel_transaction);

            LOAD_HEX(vp_back_button_service_menu);

            LOAD_HEX(vp_icon_fuel_type_for_editing);
            LOAD_HEX(vp_fuel_price_for_editing_integer);
            LOAD_HEX(vp_fuel_price_for_editing_decimal);
            LOAD_HEX(vp_button_pagination_fuel_type_for_edit);
            LOAD_HEX(vp_button_chosen_fuel_type_for_edit);

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
            s.dwin.page_print_pin                      = j["dwin"]["page_print_pin"];
            s.dwin.page_service_menu                   = j["dwin"]["page_service_menu"];
            s.dwin.page_level_gauges_info              = j["dwin"]["page_level_gauges_info"];
            s.dwin.page_level_gauge_info               = j["dwin"]["page_level_gauge_info"];
            s.dwin.page_select_fuel_type_edit          = j["dwin"]["page_select_fuel_type_edit"];
            s.dwin.page_set_fuel_price                 = j["dwin"]["page_set_fuel_price"];
            s.dwin.page_set_amount_trk                 = j["dwin"]["page_set_amount_trk"];
            s.dwin.page_set_fuel_price_another_variable= j["dwin"]["page_set_fuel_price_another_variable"];
            s.dwin.page_choose_trk_config_uno_trk      = j["dwin"]["page_choose_trk_config_uno_trk"];
            s.dwin.page_choose_trk_config_double_trk   = j["dwin"]["page_choose_trk_config_double_trk"];
            s.dwin.page_error_creating_order           = j["dwin"]["page_error_creating_order"];
            s.dwin.page_error_low_fuel_level           = j["dwin"]["page_error_low_fuel_level"];
            s.dwin.page_return_money_process           = j["dwin"]["page_return_money_process"];
            s.dwin.page_return_money_process_end       = j["dwin"]["page_return_money_process_end"];
            s.dwin.page_type_service_code              = j["dwin"]["page_type_service_code"];
            s.dwin.page_select_used_trk                = j["dwin"]["page_select_used_trk"];
            s.dwin.page_return_money_cancel_transaction= j["dwin"]["page_return_money_cancel_transaction"];
            s.dwin.page_error_transaction_failed       = j["dwin"]["page_error_transaction_failed"];
            s.dwin.page_error_incorrect_pincode       = j["dwin"]["page_error_incorrect_pincode"];

            s.dwin.audio_id_welcome_nozzle_up          = j["dwin"]["audio_id_welcome_nozzle_up"];
            s.dwin.audio_id_fuelling_end               = j["dwin"]["audio_id_fuelling_end"];
            s.dwin.audio_id_fuelling_interrupted       = j["dwin"]["audio_id_fuelling_interrupted"];

            s.dwin.icon_dispenser_nozzle_up              = j["dwin"]["icon_dispenser_nozzle_up"];
            s.dwin.icon_dispenser_idle                   = j["dwin"]["icon_dispenser_idle"];
            s.dwin.icon_dispenser_order_end              = j["dwin"]["icon_dispenser_order_end"];
            s.dwin.icon_dispenser_fueling                = j["dwin"]["icon_dispenser_fueling"];
            s.dwin.icon_dispenser_error                  = j["dwin"]["icon_dispenser_error"];
            s.dwin.icon_dispenser_halted                 = j["dwin"]["icon_dispenser_halted"];
            s.dwin.icon_dispenser_offline                = j["dwin"]["icon_dispenser_offline"];
            s.dwin.icon_dispenser_locked                 = j["dwin"]["icon_dispenser_locked"];

            s.dwin.icon_fuel_type_92   = j["dwin"]["icon_fuel_type_92"];
            s.dwin.icon_fuel_type_95   = j["dwin"]["icon_fuel_type_95"];
            s.dwin.icon_fuel_type_98   = j["dwin"]["icon_fuel_type_98"];
            s.dwin.icon_fuel_type_100  = j["dwin"]["icon_fuel_type_100"];
            s.dwin.icon_fuel_type_dt   = j["dwin"]["icon_fuel_type_dt"];

            s.dwin.text_len_trk_id = j["dwin"]["text_len_trk_id"];
            s.dwin.text_len_order_decimal = j["dwin"]["text_len_order_decimal"];
            s.dwin.text_len_order_integer = j["dwin"]["text_len_order_integer"];
            s.dwin.text_len_fuel_type = j["dwin"]["text_len_fuel_type"];
            s.dwin.text_len_percent_progress_bar = j["dwin"]["text_len_percent_progress_bar"];
            s.dwin.text_len_date_time_footer = j["dwin"]["text_len_date_time_footer"];
            s.dwin.text_len_fuel_price = j["dwin"]["text_len_fuel_price"];
            s.dwin.text_len_fuel_integer = j["dwin"]["text_len_fuel_integer"];

            s.dwin.icon_dispenser_idle = j["dwin"]["icon_dispenser_idle"];
            s.dwin.icon_dispenser_nozzle_up = j["dwin"]["icon_dispenser_nozzle_up"];
            s.dwin.icon_dispenser_order_end = j["dwin"]["icon_dispenser_order_end"];

            s.business_logic.sleep_after_chosen_trk_page = j["business"]["sleep_after_chosen_trk_page"];
            s.business_logic.waiting_dispenser_fuelling  = j["business"]["waiting_dispenser_fuelling"];
            s.business_logic.show_incorrect_pin_page        = j["business"]["show_incorrect_pin_page"];

            s.APIDispenser.authorize = j["api.dispenser_command"]["authorize"];
            s.APIDispenser.close = j["api.dispenser_command"]["close"];

            s.pipe.name = j["pipe"]["name"];

            if (j.contains("gas.station") && j["gas.station"].is_object()) {
                auto& gas_station_node = j["gas.station"];

                s.gas_station.amount_trk = gas_station_node.value("amount_trk", 0);
                s.gas_station.service_code = gas_station_node.value("service_code", 0000);


                if (gas_station_node.contains("fuel_price") && gas_station_node["fuel_price"].is_object()) {
                    for (auto& el : gas_station_node["fuel_price"].items()) {

                        s.gas_station.fuel_prices[el.key()] = el.value().get<std::string>();
                    }
                }

                if (gas_station_node.contains("used_trks") && gas_station_node["used_trks"].is_array()) {
                    for (const auto& trk_id : gas_station_node["used_trks"]) {
                        s.gas_station.used_trks.push_back(trk_id.get<std::string>());
                    }
                }

                if (gas_station_node.contains("standalone_prime") && gas_station_node["standalone_prime"].is_object()) {
                    for (const auto& fuel_id : gas_station_node["standalone_prime"].items()) {
                        s.gas_station.prime_standalone[fuel_id.key()] = fuel_id.value().get<std::string>();
                    }
                }

                if (gas_station_node.contains("standalone_processing") && gas_station_node["standalone_processing"].is_object()) {
                    for (const auto& fuel_id : gas_station_node["standalone_processing"].items()) {
                        s.gas_station.processing_standalone[fuel_id.key()] = fuel_id.value().get<std::string>();
                    }
                }
            }

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