#include <itp/cl2.h>
#include <itp/tools.hpp>

extern "C" {

#include <itp/symbol.h>
#include <initp/system/debug.h>

}

#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <sstream>

namespace itp {
namespace tools {

std::string get_device_api(uint32_t value) {
    std::stringstream stream;
    if (value & itp::CL2_API_CONTROLLER) {
        value &= ~itp::CL2_API_CONTROLLER;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Контроллер платежей";
    }
    if (value & itp::CL2_API_CARD_READER) {
        value &= ~itp::CL2_API_CARD_READER;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Считыватель магнитных карт";
    }
    if (value & itp::CL2_API_CONTACTLESS_READER) {
        value &= ~itp::CL2_API_CONTACTLESS_READER;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Бесконтактный считыватель";
    }
    if (value & itp::CL2_API_SAM_READER) {
        value &= ~itp::CL2_API_SAM_READER;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Считыватель SAM-карт";
    }
    if (value & itp::CL2_API_PINPAD) {
        value &= ~itp::CL2_API_PINPAD;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Пин-клавиатура";
    }
    if (value & itp::CL2_API_PRINTER) {
        value &= ~itp::CL2_API_PRINTER;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Принтер чеков";
    }
    if (value & itp::CL2_API_BARCODE_SCANNER) {
        value &= ~itp::CL2_API_BARCODE_SCANNER;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Сканер штрих-кодов";
    }
    if (value & itp::CL2_API_PAYMENT) {
        value &= ~itp::CL2_API_PAYMENT;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Сервис работы с различными картами";
    }
    if (value & itp::CL2_API_ACTIVE_TERMINAL) {
        value &= ~itp::CL2_API_ACTIVE_TERMINAL;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Активный терминал";
    }
    if (value & itp::CL2_API_BANK_PROCESSING) {
        value &= ~itp::CL2_API_BANK_PROCESSING;
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Разрешение на банковкие операции";
    }
    if (value & itp::CL2_API_BASIC) {
        value &= ~itp::CL2_API_BASIC;
        if (stream.rdbuf()->in_avail() == 0) {
            stream << u8"Базовое устройство";
        }
    }
    if (value) {
        if (stream.rdbuf()->in_avail() > 0) stream << u8", ";
        stream << u8"Неизвестный тип API";
    } else {
        if (stream.rdbuf()->in_avail() == 0) {
            stream << u8"Интерфейс отсутствует";
        }
    }
    return stream.str();
}

std::string get_device_name(uint8_t value) {
    switch (value) {
        case itp::CL2_DEV_UNKNOWN:
            return u8"Неизвестное устройство";
        case itp::CL2_DEV_GENERIC:
            return u8"Универсальное устройство";
        case itp::CL2_DEV_BOOT_MANAGER:
            return u8"Менеджер устройств и модулей";
        case itp::CL2_DEV_ARKAIM_PAY:
            return u8"Контроллер платежей";
        case itp::CL2_DEV_CRYPTERA_1X:
            return u8"Пин-клавиатура Cryptera PCI 1.x";
        case itp::CL2_DEV_CRYPTERA_2X:
            return u8"Пин-клавиатура Cryptera PCI 2.x";
        case itp::CL2_DEV_CRYPTERA_3X:
            return u8"Пин-клавиатура Cryptera PCI 3.x";
        case itp::CL2_DEV_PINPAD_INIT:
            return u8"Пин-клавиатура Инит-Плюс";
        case itp::CL2_DEV_SANKYO_ICT3K5:
            return u8"Считыватель магнитных карт Sankyo ICT3K5";
        case itp::CL2_DEV_CREATOR_CRT310:
            return u8"Считыватель магнитных карт Creator CRT310";
        case itp::CL2_DEV_VIVOPAY_KIOSK3:
            return u8"Бесконтактный считыватель Vivopay Kiosk III";
        case itp::CL2_DEV_PCSC_ACR122:
            return u8"Бесконтактный считыватель ACS ACR122";
        case itp::CL2_DEV_PCSC_ACR1251:
            return u8"Бесконтактный считыватель ACS ACR1251";
        case itp::CL2_DEV_PCSC_ACR128_ICC:
            return u8"Контактный считыватель смарт-карт ACS ACR128 ICC";
        case itp::CL2_DEV_PCSC_ACR128_PICC:
            return u8"Бесконтактный считыватель ACS ACR128 PICC";
        case itp::CL2_DEV_PCSC_SCR33X:
            return u8"Контактный считыватель смарт-карт SCM SCR33x";
        case itp::CL2_DEV_PCSC_ACR39U:
            return u8"Считыватель SAM карт ACS ACR39U";
        case itp::CL2_DEV_CUSTOM_TL80:
            return u8"Принтер чеков Custom TL80 или совместимый";
        case itp::CL2_DEV_OTI_GENERIC:
            return u8"Считыватель бесконтактных карт OTI";
        case itp::CL2_DEV_SAAS_GENERIC:
            return u8"Считыватель бесконтактных карт SAAS";
        case itp::CL2_DEV_IBUTTON_EMARINE:
            return u8"Считыватель бесконтактных карт iButton (EM-MARIN)";
        case itp::CL2_DEV_YOKO_EP3000:
            return u8"Сканер штрих-кодов Yoko EP3000";
        case itp::CL2_DEV_PAY_BRENT_ONLINE:
            return u8"Сервис топливных карт Brent Fueller";
        case itp::CL2_DEV_PAY_E100_ONLINE:
            return u8"Сервис топливных карт E100";
        case itp::CL2_DEV_PAY_INITPLUS_ONLINE:
            return u8"Сервис топливных карт Инит-Плюс";
        case itp::CL2_DEV_PAY_SERVIO_ONLINE:
            return u8"Сервис работы с картами Servio Online";
        case itp::CL2_DEV_PAY_YANDEX_TERMINAL:
            return u8"Сервис Яндекс.Заправки (терминал)";
        case itp::CL2_DEV_PAY_OPS_ONLINE:
            return u8"Сервис работы с топливными картами OPS";
        case itp::CL2_DEV_PAY_BF_BONUS_ONLINE:
            return u8"Сервис бонусных карт Brent Fueller";
        case itp::CL2_DEV_PAY_EKA_ONLINE:
            return u8"Сервис работы с топливными картами EKA";
        case itp::CL2_DEV_PAY_UNIVERSAL_PAYMENT_API:
            return u8"Сервис работы с топливными картами Universal Payment API";
        case itp::CL2_DEV_PAY_INGENICO:
            return u8"Сервис терминала Ingenico";
        case itp::CL2_DEV_PAY_SBP_RAIF:
            return u8"Сервис СБП Raiffeisen";
        case itp::CL2_DEV_PAY_TAIFF:
            return u8"Сервис работы с картами TAIFF";
        case itp::CL2_DEV_PAY_SBP_GPB:
            return u8"Сервис СБП Газпромбанк";
        case itp::CL2_DEV_PAY_SBP_SBR:
            return u8"Сервис СБП Сбер";
        case itp::CL2_DEV_PAY_OPTI:
            return u8"Сервис работы с картами OPTI";
        case itp::CL2_DEV_VENDOTEK_GENERIC:
            return u8"Сервис терминала Vendotek";
        case itp::CL2_DEV_PROXY:
            return u8"Proxy-подключение для RabbitMQ";
        case itp::CL2_DEV_UNIPOS_GENERIC:
            return u8"Сервис работы с платежными терминалами по протоколу UniPOS";
        default:
            return u8"Неизвестное устройство";
    }
}

std::string get_device_status(uint32_t status) {
    if (status & itp::CL2_DST_DISABLED) {
        return u8"Отключено";
    } else if (status & itp::CL2_DST_OFFLINE) {
        return u8"Нет связи";
    } else if (status & itp::CL2_DST_ERROR) {
        return u8"Ошибка в работе";
    }
    std::stringstream stream;
    stream << u8"Онлайн";
    if (status & itp::CL2_DST_CARD_GATE)
        stream << u8", Карта на входе";
    if (status & itp::CL2_DST_CARD_INSIDE)
        stream << u8", Карта внутри";
    if (status & itp::CL2_DST_RFID_ACTIVE)
        stream << u8", Обнаружен бесконтакт";
    return stream.str();
}

std::string time_to_string(const std::string& prefix, uint64_t time) {
    char array[] = "qazwsxedcrfvtgbyhnujmikolp1234567890";
    std::stringstream stream;
    boost::posix_time::ptime t = boost::posix_time::from_time_t(time / 1000L) + boost::posix_time::milliseconds(time % 1000L);
    boost::gregorian::date d = t.date();
    boost::posix_time::ptime epoch(boost::gregorian::date(d.year(), d.month(), d.day()));
    stream << prefix;
    stream << int(d.year() % 100);
    stream << array[d.month() - 1];
    stream << array[d.day() - 1];
    stream << (t - epoch).total_milliseconds();
    return stream.str();
}

uint8_t cast_system_debug_level(uint8_t level) {
    switch (level) {
        case SYSTEM_LEVEL_FATAL:    return ITP_DEBUG_LEVEL_FATAL;
        case SYSTEM_LEVEL_ERROR:    return ITP_DEBUG_LEVEL_ERROR;
        case SYSTEM_LEVEL_WARNING:  return ITP_DEBUG_LEVEL_WARNING;
        case SYSTEM_LEVEL_INFO:     return ITP_DEBUG_LEVEL_INFO;
        case SYSTEM_LEVEL_DEBUG:    return ITP_DEBUG_LEVEL_DEBUG;
        case SYSTEM_LEVEL_TRACE:
        default:                    return ITP_DEBUG_LEVEL_TRACE;
    }
}

}}
