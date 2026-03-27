#ifndef INITPLUS_GTK_UTILS_HPP_INCLUDED
#define INITPLUS_GTK_UTILS_HPP_INCLUDED

#include <string>
#include <iomanip>

#include <gtkmm-3.0/gtkmm.h>

namespace initp {
namespace gtk {

namespace utils {

inline Glib::ustring to_utf8(const char* s) {
    gchar* utf8 = g_locale_to_utf8(s, -1, NULL, NULL, NULL);
    Glib::ustring string(utf8);
    g_free(utf8);
    return string;
}

inline Glib::ustring to_utf8(const std::string& s) {
    gchar* utf8 = g_locale_to_utf8(s.c_str(), -1, NULL, NULL, NULL);
    Glib::ustring string(utf8);
    g_free(utf8);
    return string;
}

inline std::string to_locale(const Glib::ustring& s) {
    gchar* locale = g_locale_from_utf8(s.c_str(), -1, NULL, NULL, NULL);
    std::string string(locale);
    g_free(locale);
    return string;
}

template<typename T>
inline T parse(const Glib::ustring& string) {
    if (string.empty())
        return T(0);
    T value;
    std::stringstream s;
    s << to_locale(string);
    s >> value;
    return value;
}

}

namespace utf {

template<typename T>
inline Glib::ustring to_string(const T& value, int precision = -1, bool fixed = false) {
    std::stringstream s;
    if (precision >= 0) {
        s << std::fixed << std::showpoint;
        s << std::setprecision(precision);
    }
    s << value;
    std::string string = s.str();
    if (precision >= 0 && !fixed) {
        while (!string.empty()) {
            if (string.back() == '0') {
                string.pop_back();
            } else if (string.back() == '.') {
                string.pop_back();
                break;
            } else break;
        }
    }
    return utils::to_utf8(string);
}

inline Glib::ustring pow_minus(void) {
    const gunichar data[] = { 0x207B, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring pow_minus_one(void) {
    const gunichar data[] = { 0x207B, 0xB9, 0x0 };
    return Glib::ustring(data, data + 2);
}

inline Glib::ustring pow_minus_two(void) {
    const gunichar data[] = { 0x207B, 0xB2, 0x0 };
    return Glib::ustring(data, data + 2);
}

inline Glib::ustring pow_minus_three(void) {
    const gunichar data[] = { 0x207B, 0xB3, 0x0 };
    return Glib::ustring(data, data + 2);
}

inline Glib::ustring pow_one(void) {
    const gunichar data[] = { 0xB9, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring pow_two(void) {
    const gunichar data[] = { 0xB2, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring pow_three(void) {
    const gunichar data[] = { 0xB3, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring multiplication_sign(void) {
    const gunichar data[] = { 0xD7, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring bullet_operator(void) {
    const gunichar data[] = { 0x2219, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring bullet(void) {
    const gunichar data[] = { 0x2022, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring white_bullet(void) {
    const gunichar data[] = { 0x25E6, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring summation(void) {
    const gunichar data[] = { 0x2211, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring plus_minus(void) {
    const gunichar data[] = { 0xB1, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring minus_sign(void) {
    const gunichar data[] = { 0x2212, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring less_or_equal(void) {
    const gunichar data[] = { 0x2264, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring greater_or_equal(void) {
    const gunichar data[] = { 0x2265, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring right_arrow(void) {
    const gunichar data[] = { 0x2192, 0x0 };
    return Glib::ustring(data, data + 1);
}

inline Glib::ustring left_arrow(void) {
    const gunichar data[] = { 0x2190, 0x0 };
    return Glib::ustring(data, data + 1);
}

}

inline Gtk::HeaderBar* new_header(void) {
    Gtk::HeaderBar* header = Gtk::manage(new (std::nothrow) Gtk::HeaderBar());
    header->set_show_close_button(true);
    header->set_visible(true);
    return header;
}

template<typename T>
inline bool set_active(Gtk::ComboBox* combo, int column, T value) {
    if (!combo) return false;
    T row;
    Gtk::TreeModel::const_iterator end = combo->get_model()->children().end();
    for (Gtk::TreeModel::const_iterator it = combo->get_model()->children().begin(); it != end; ++it) {
        it->get_value(column, row);
        if (row == value) {
            combo->set_active(it);
            return true;
        }
    }
    combo->set_active(-1);
    return false;
}

}}

#endif // INITPLUS_GTK_UTILS_HPP_INCLUDED
