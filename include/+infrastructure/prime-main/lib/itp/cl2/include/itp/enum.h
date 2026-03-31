#ifndef ITP_CL2_ENUM_H_INCLUDED
#define ITP_CL2_ENUM_H_INCLUDED

/**
 * \file enum.h
 * \brief Файл содержит набор используемых кодов ошибок.
 */

#ifdef ITP_COMPILE_CXX
namespace itp {
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление всех возможных типов API.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> = 0b 0 Пустой интерфейс, не поддерживает никаких команд.
     */
    CL2_API_NONE = 0x00,

    /**
     * \brief \c <b>0x01</b> = 0b 1 Базовый интерфейс.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_ALL_ENABLE\endlink</code> <i>(опционально)</i>;</li>
     * <li><code>\link CL2_CMD_ALL_DISABLE\endlink</code> <i>(опционально)</i>;</li>
     * <li><code>\link CL2_CMD_ALL_GET_ID\endlink</code> <i>(для дочерних сервисов)</i>;</li>
     * <li><code>\link CL2_CMD_ALL_GET_TYPE\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ALL_GET_STATUS\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ALL_CONFIGURE\endlink</code> <i>(опционально)</i>;</li>
     * <li><code>\link CL2_CMD_ALL_STATUS_CHANGED\endlink</code>.</li>
     * </ul>
     */
    CL2_API_BASIC = 0x01,

    /**
     * \brief \c <b>0x02</b> = 0b 01 Основной сервис контроллера платежей.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_CTR_RESOLVE_CARD_TYPE\endlink</code>.</li>
     * </ul>
     */
    CL2_API_CONTROLLER = 0x02,

    /**
     * \brief \c <b>0x04</b> = 0b 001 Считыватель магнитных карт.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_CRD_COMMAND\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_READ_DATA\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_FORWARD_APDU\endlink</code>.</li>
     * </ul>
     */
    CL2_API_CARD_READER = 0x04,

    /**
     * \brief \c <b>0x08</b> = 0b 0001 Бесконтактный считыватель.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_CRD_COMMAND\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_READ_UID\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_AUTHENTICATE_MIFARE\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_READ_MIFARE\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_FORWARD_APDU\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_CARD_DETECTED\endlink</code>.</li>
     * </ul>
     */
    CL2_API_CONTACTLESS_READER = 0x08,

    /**
     * \brief \c <b>0x10</b> = 0b 0000 1 Считыватель SAM-карт.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_SAM_GET_APPLET_TYPE\endlink</code>;</li>
     * <li><code>\link CL2_CMD_SAM_TRANSLATE_DUKPT\endlink</code>;</li>
     * <li><code>\link CL2_CMD_SAM_GET_SESSION_KEY\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_FORWARD_APDU\endlink</code>;</li>
     * <li><code>\link CL2_CMD_CRD_CARD_DETECTED\endlink</code>.</li>
     * </ul>
     */
    CL2_API_SAM_READER = 0x10,

    /**
     * \brief \c <b>0x20</b> = 0b 0000 01 Пин-клавиатура.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_PIN_GET_PLAIN\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PIN_GET_DUKPT\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PIN_KEY_PRESSED\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PIN_COMPLETE\endlink</code>.</li>
     * </ul>
     */
    CL2_API_PINPAD = 0x20,

    /**
     * \brief \c <b>0x40</b> = 0b 0000 001 Принтер чеков.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_PRT_PRINT_TEXT\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PRT_CUT_PAPER\endlink</code>.</li>
     * </ul>
     */
    CL2_API_PRINTER = 0x40,

    /**
     * \brief \c <b>0x80</b> = 0b 0000 0001 Сервис работы с различными картами.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_PAY_CHECK_CARD\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_CLOSE_SHIFT\endlink</code>;</li>
     * <br>Для топливных процессингов:
     * <li><code>\link CL2_CMD_PAY_GET_INFO\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_TRANSACTION\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_CONFIRM\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_CANCEL\endlink</code>;</li>
     * <br>Для дисконтных процессингов:
     * <li><code>\link CL2_CMD_PAY_DISCOUNT\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_DISCOUNT_DONE\endlink</code>;</li>
     * <br>Для бонусных процессингов:
     * <li><code>\link CL2_CMD_PAY_GET_BONUS\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_USE_BONUS\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_CONFIRM_BONUS\endlink</code>;</li>
     * <li><code>\link CL2_CMD_PAY_CANCEL_BONUS\endlink</code>.</li>
     * </ul>
     */
    CL2_API_PAYMENT = 0x80,

    /**
     * \brief \c <b>0x100</b> = 0b 0000 0000 1  Поддержка банковских операций.
     */
    CL2_API_BANK_PROCESSING = 0x100,

    /**
     * \brief \c <b>0x200</b> = 0b 0000 0000 01 Активный терминал (размещение заказов).
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_ATM_GET_CONFIG\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ATM_GET_PRICES\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ATM_GET_STATUS\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ATM_GET_ORDER\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ATM_AUTHORIZE\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ATM_CANCEL\endlink</code>;</li>
     * <li><code>\link CL2_CMD_ATM_CONFIG_CHANGED\endlink</code> <i>(опционально)</i>;</li>
     * <li><code>\link CL2_CMD_ATM_PRICES_CHANGED\endlink</code> <i>(опционально)</i>;</li>
     * <li><code>\link CL2_CMD_ATM_ORDER_CHANGED\endlink</code> <i>(опционально)</i>.</li>
     * </ul>
     */
    CL2_API_ACTIVE_TERMINAL = 0x200,

    /**
     * \brief \c <b>0x400</b> = 0b 0000 0000 001 Сканер штрих-кодов.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_BARCODE_READ\endlink</code>;</li>
     * </ul>
     */
    CL2_API_BARCODE_SCANNER = 0x400,

    /**
     * \brief \c <b>0x800</b> = 0b 0000 0000 0001 Платежный терминал, например PAX D200.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_???\endlink</code>;</li>
     * </ul>
     */
    CL2_API_PAYMENT_TERMINAL = 0x800,

    /**
     * \brief \c <b>0x1000</b> = 0b 0000 0000 0000 1 Кассовый аппарат.
     *
     * Поддерживает следующие команды:
     * <ul>
     * <li><code>\link CL2_CMD_???\endlink</code>;</li>
     * </ul>
     */
    CL2_API_KKT = 0x1000

#ifdef ITP_COMPILE_CXX
} cl2_api_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_api_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление всех возможных типов устройств.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Универсальное устройство.
     */
    CL2_DEV_UNKNOWN = 0x00,

    /**
     * \brief \c <b>0x01</b> Универсальное устройство.
     */
    CL2_DEV_GENERIC = 0x01,

    /**
     * \brief \c <b>0x02</b> Менеджер устройств и модулей.
     */
    CL2_DEV_BOOT_MANAGER = 0x02,

    /**
     * \brief \c <b>0x03</b> Контроллер платежей.
     */
    CL2_DEV_ARKAIM_PAY = 0x03,

    /**
     * \brief \c <b>0x04</b> Пин-клавиатура Cryptera PCI 1.x.
     */
    CL2_DEV_CRYPTERA_1X = 0x04,

    /**
     * \brief \c <b>0x05</b> Пин-клавиатура Cryptera PCI 2.x.
     */
    CL2_DEV_CRYPTERA_2X = 0x05,

    /**
     * \brief \c <b>0x06</b> Пин-клавиатура Cryptera PCI 3.x.
     */
    CL2_DEV_CRYPTERA_3X = 0x06,

    /**
     * \brief \c <b>0x07</b> Пин-клавиатура Инит-Плюс.
     */
    CL2_DEV_PINPAD_INIT = 0x07,

    /**
     * \brief \c <b>0x08</b> Считыватель магнитных карт Sankyo ICT3K5.
     */
    CL2_DEV_SANKYO_ICT3K5 = 0x08,

    /**
     * \brief \c <b>0x09</b> Считыватель магнитных карт Creator CRT310.
     */
    CL2_DEV_CREATOR_CRT310 = 0x09,

    /**
     * \brief \c <b>0x0A</b> Бесконтактный считыватель Vivopay Kiosk III.
     */
    CL2_DEV_VIVOPAY_KIOSK3 = 0x0A,

    /**
     * \brief \c <b>0x0B</b> Бесконтактный считыватель ACS ACR122.
     */
    CL2_DEV_PCSC_ACR122 = 0x0B,

    /**
     * \brief \c <b>0x0C</b> Бесконтактный считыватель ACS ACR128.
     */
    // CL2_DEV_PCSC_ACR128 = 0x0C,

    /**
     * \brief \c <b>0x0D</b> Считыватель SAM карт ACS ACR39U.
     */
    CL2_DEV_PCSC_ACR39U = 0x0D,

    /**
     * \brief \c <b>0x0E</b> Принтер чеков Custom TL80 или совместимый.
     */
    CL2_DEV_CUSTOM_TL80 = 0x0E,

    /**
     * \brief \c <b>0x0F</b> Считыватель бесконтактных карт OTI.
     */
    CL2_DEV_OTI_GENERIC = 0x0F,

    /**
     * \brief \c <b>0x10</b> Терминал Инит-Плюс.
     */
    CL2_DEV_INITPLUS_TERMINAL = 0x10,

    /**
     * \brief \c <b>0x11</b> Инит-Плюс Standalone.
     */
    CL2_DEV_INITPLUS_STANDALONE = 0x11,

    /**
     * \brief \c <b>0x12</b> Сервис работы с топливными картами Brent Fueller.
     */
    CL2_DEV_PAY_BRENT_ONLINE = 0x12,

    /**
     * \brief \c <b>0x13</b> Сервис работы с топливными картами E100.
     */
    CL2_DEV_PAY_E100_ONLINE = 0x13,

    /**
     * \brief \c <b>0x14</b> Сервис работы с топливными картами Инит-Плюс.
     */
    CL2_DEV_PAY_INITPLUS_ONLINE = 0x14,

    /**
     * \brief \c <b>0x15</b> Сервис работы с картами Servio Online.
     *
     * Дополнительные данные для команд <code>\link CL2_CMD_PAY_TRANSACTION\endlink</code>,<br>
     * <code>\link CL2_CMD_PAY_DISCOUNT\endlink</code>, <code>\link CL2_CMD_PAY_GET_BONUS\endlink</code>, <code>\link CL2_CMD_PAY_USE_BONUS\endlink</code>:
     * <ul>
     * <li>\b uint64 Идентификатор транзакции из \b Servio.</li>
     * </ul>
     */
    CL2_DEV_PAY_SERVIO_ONLINE = 0x15,

    /**
     * \brief \c <b>0x16</b> Сервис работы с Яндекс.Заправки (терминал).
     */
    CL2_DEV_PAY_YANDEX_TERMINAL = 0x16,

    /**
     * \brief \c <b>0x17</b> Сервис работы с топливными картами OPS.
     */
    CL2_DEV_PAY_OPS_ONLINE = 0x17,
    
    /**
     * \brief \c <b>0x18</b> Считыватель IButton EMarine
     */
    CL2_DEV_IBUTTON_EMARINE = 0x18,

    /**
     * \brief \c <b>0x19</b> Сканер штрих-кодов Yoko EP3000
     */
    CL2_DEV_YOKO_EP3000 = 0x19,

    /**
     * \brief \c <b>0x1A</b> Сервис работы с бонусными картами БФ розницы.
     */
    CL2_DEV_PAY_BF_BONUS_ONLINE = 0x1A,

    /**
     * \brief \c <b>0x1B</b> Бесконтактный считыватель ACS ACR1251.
     */
    CL2_DEV_PCSC_ACR1251 = 0x1B,

    /**
     * \brief \c <b>0x1C</b> Контактный считыватель смарт-карт ACS ACR128 ICC
     */
    CL2_DEV_PCSC_ACR128_ICC = 0x1C,

     /**
     * \brief \c <b>0x1D</b> Бесконтактный считыватель ACS ACR128 PICC, для MIFARE карт, например Сервио или Инит+Онлайн.
     */
    CL2_DEV_PCSC_ACR128_PICC = 0x1D,

    /**
     * \brief \c <b>0x1E</b> Контактный считыватель смарт-карт SCM SCR33x
     */
    CL2_DEV_PCSC_SCR33X = 0x1E,

    /**
     * \brief \c <b>0x1F</b> Сервис работы с топливными картами EKA.
     */
    CL2_DEV_PAY_EKA_ONLINE = 0x1F,

    /**
     * \brief \c <b>0x20</b> Сервис работы с топливными картами Universal Payment API.
     * 
     * Дополнительные опциональные данные команд JSON-формата типа \b wstring <br>
     * \code{.json}
     * [{ "ItemId": "itemA", "Value": "value-A" }, { "ItemId": "itemB", "Value": "value-B" },  ...]
     * \endcode
     * 
     */
    CL2_DEV_PAY_UNIVERSAL_PAYMENT_API = 0x20,

    /**
     * \brief \c <b>0x21</b> Считыватель бесконтактных карт SAAS.
     */
    CL2_DEV_SAAS_GENERIC = 0x21,

    /**
     * \brief \c <b>0x22</b> Сервис терминала Ingenico
     */
    CL2_DEV_PAY_INGENICO = 0x22,

    /**
     * \brief \c <b>0x23</b> Сервис СБП Raiffeisen
     */
    CL2_DEV_PAY_SBP_RAIF = 0x23,

    /**
     * \brief \c <b>0x24</b> Сервис работы с картами TAIFF.
     */
    CL2_DEV_PAY_TAIFF = 0x24,

    /**
     * \brief \c <b>0x25</b> Сервис СБП Газпромбанк
     */
    CL2_DEV_PAY_SBP_GPB = 0x25,

    /**
     * \brief \c <b>0x26</b> Сервис СБП Сбер
     */
    CL2_DEV_PAY_SBP_SBR = 0x26,

    /**
     * \brief \c <b>0x27</b> Сервис работы с картами Cloudio.
     */
    CL2_DEV_PAY_CLOUDIO = 0x27,

    /**
     * \brief \c <b>0x31</b> Сервис терминала pax d200
     */
    CL2_DEV_PAX_D200 = 0x31,

    /**
     * \brief \c <b>0x32</b> Сервис терминала Vendotek
     */
    CL2_DEV_VENDOTEK_GENERIC = 0x32,

    /**
     * \brief \c <b>0x33</b> Proxy-подключение для RabbitMQ (via MQTT)
     */
    CL2_DEV_PROXY = 0x33

#ifdef ITP_COMPILE_CXX
} cl2_device_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_device_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление состояний и флагов устройств.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Устройство работает в штатном режиме.
     *
     * При нормальной работе могут передаваться дополнительные флаги, начиная с \b 0x08.
     */
    CL2_DST_ONLINE = 0x00,

    /**
     * \brief \c <b>0x01</b> Устройство отключено.
     */
    CL2_DST_DISABLED = 0x01,

    /**
     * \brief \c <b>0x02</b> Нет связи с устройством.
     */
    CL2_DST_OFFLINE = 0x02,

    /**
     * \brief \c <b>0x04</b> Ошибка при работе с устройством.
     */
    CL2_DST_ERROR = 0x04,

    /**
     * \brief \c <b>0x08</b> Карта на входе считывателя.
     */
    CL2_DST_CARD_GATE = 0x08,

    /**
     * \brief \c <b>0x10</b> Карта внутри считывателя.
     */
    CL2_DST_CARD_INSIDE = 0x10,

    /**
     * \brief \c <b>0x20</b> Обнаружена бесконтактная карта.
     */
    CL2_DST_RFID_ACTIVE = 0x20

#ifdef ITP_COMPILE_CXX
} cl2_device_status_t;
#else // ITP_COMPILE_CXX
} itp_cl2_device_status_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление групп платёжных карт.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Тип не определён.
     */
    CL2_SERVICE_UNKNOWN = 0x00,

    /**
     * \brief \c <b>0x01</b> Банковская карта.
     */
    CL2_SERVICE_BANK = 0x01,

    /**
     * \brief \c <b>0x02</b> Топливная карта.
     */
    CL2_SERVICE_FUEL = 0x02,

    /**
     * \brief \c <b>0x04</b> Бонусная карта.
     */
    CL2_SERVICE_BONUS = 0x04,

    /**
     * \brief \c <b>0x08</b> Дисконтная карта.
     */
    CL2_SERVICE_DISCOUNT = 0x08

#ifdef ITP_COMPILE_CXX
} cl2_service_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_service_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление команд считывателя карт.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Включить приём карт.
     */
    CL2_CRX_ENABLE = 0x00,

    /**
     * \brief \c <b>0x01</b> Выключить приём карт.
     */
    CL2_CRX_DISABLE = 0x01,

    /**
     * \brief \c <b>0x02</b> Извлечь карту.
     */
    CL2_CRX_EJECT = 0x02,

    /**
     * \brief \c <b>0x03</b> Захватить карту.
     */
    CL2_CRX_CAPTURE = 0x03

#ifdef ITP_COMPILE_CXX
} cl2_reader_command_t;
#else // ITP_COMPILE_CXX
} itp_cl2_reader_command_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление дорожек на магнитной полосе.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Дорожка не определена.
     */
    CL2_MAG_TRACK_NONE = 0x00,

    /**
     * \brief \c <b>0x01</b> ISO Track #1.
     */
    CL2_MAG_TRACK_ISO_1 = 0x01,

    /**
     * \brief \c <b>0x02</b> ISO Track #2.
     */
    CL2_MAG_TRACK_ISO_2 = 0x02,

    /**
     * \brief \c <b>0x03</b> ISO Track #3.
     */
    CL2_MAG_TRACK_ISO_3 = 0x03,

    /**
     * \brief \c <b>0x04</b> JIS II.
     */
    CL2_MAG_TRACK_JIS_II = 0x04

#ifdef ITP_COMPILE_CXX
} cl2_mag_track_t;
#else // ITP_COMPILE_CXX
} itp_cl2_mag_track_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление типов бесконтактных карт.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Карта не определена.
     */
    CL2_CARD_UNKNOWN = 0x00,

    /**
     * \brief \c <b>0x01</b> Mifare Classic S50 (1k).
     *
     * 16 секторов по 4 блока из 16 байт.
     */
    CL2_CARD_MIFARE_1K = 0x01,

    /**
     * \brief \c <b>0x02</b> Mifare Classic S70 (4k).
     *
     * 32 сектора по 4 блока из 16 байт.<br>
     * 8 секторов по 16 блоков из 16 байт.
     */
    CL2_CARD_MIFARE_4K = 0x02,

    /**
     * \brief \c <b>0x03</b> Mifare Classic UltraLight.
     *
     * 16 секторов по 1 блоку из 4 байт.
     */
    CL2_CARD_MIFARE_UL = 0x03,

    /**
     * \brief \c <b>0x04</b> EmMarine
     *
     * UID карты - 8 байт - long
     */
    CL2_CARD_EMARINE = 0x04,

    /**
     * \brief \c <b>0x05</b> Barcode (Emulated)
     *
     * данные штрих-кода в ASCII
     */
    CL2_CARD_BARCODE = 0x05,

     /**
     * \brief \c <b>0x06</b> IC 
     *
     * IC карта
     */
    CL2_CARD_IC = 0x06



#ifdef ITP_COMPILE_CXX
} cl2_card_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_card_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление типов штрих-кодов.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Тип штрих-кода не определен.
     */
    CL2_BARCODE_UNKNOWN = 0x00,

    /**
     * \brief \c <b>0x00</b> Тип штрих-кода EAN13.
     */
    CL2_BARCODE_EAN13 = 0x01,
	
	/**
     * \brief \c <b>0x00</b> Тип штрих-кода QR Code.
     */
    CL2_BARCODE_QR_CODE = 0x02,
	
	/**
     * \brief \c <b>0x00</b> Тип штрих-кода Code128C.
     */
    CL2_BARCODE_CODE128C = 0x03


#ifdef ITP_COMPILE_CXX
} cl2_barcode_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_barcode_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление типов ключей для бесконтактных карт.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Ключ по умолчанию.
     */
    CL2_KEY_DEFAULT = 0x00,

    /**
     * \brief \c <b>0x01</b> Mifare ключ A.
     */
    CL2_KEY_MIFARE_A = 0x01,

    /**
     * \brief \c <b>0x02</b> Mifare ключ B.
     */
    CL2_KEY_MIFARE_B = 0x02

#ifdef ITP_COMPILE_CXX
} cl2_key_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_key_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление специальных символов пин-клавиатуры.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x80</b> Символ подтверждения.
     */
    CL2_PIN_SYM_ACCEPT = 0x80,

    /**
     * \brief \c <b>0x81</b> Символ отмены.
     */
    CL2_PIN_SYM_CANCEL = 0x81,

    /**
     * \brief \c <b>0x82</b> Символ удаления (последнего символа).
     */
    CL2_PIN_SYM_REMOVE = 0x82

#ifdef ITP_COMPILE_CXX
} cl2_pin_symbol_t;
#else // ITP_COMPILE_CXX
} itp_cl2_pin_symbol_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление способов ввода пин-кода.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Без шифрования.
     */
    CL2_PIN_TYPE_PLAIN = 0x00,

    /**
     * \brief \c <b>0x01</b> Шифрование DUKPT.
     */
    CL2_PIN_TYPE_DUKPT = 0x01

#ifdef ITP_COMPILE_CXX
} cl2_pin_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_pin_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление поддерживаемых апплетов SAM карт.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Неподдерживаемый апплет.
     */
    CL2_SAM_APPLET_UNKNOWN = 0x00,

    /**
     * \brief \c <b>0x01</b> Апплет Инит+.
     */
    CL2_SAM_APPLET_INIT_PLUS = 0x01,

    /**
     * \brief \c <b>0x02</b> Апплет Петрол+.
     */
    CL2_SAM_APPLET_PETROL_PLUS = 0x02

#ifdef ITP_COMPILE_CXX
} cl2_applet_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_applet_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление статусов ТРК.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> ТРК сброшена.
     */
    CL2_DS_IDLE = 0x00,

    /**
     * \brief \c <b>0x01</b> Пистолет снят, ТРК ожидает авторизации.
     */
    CL2_DS_NOZZLE_UP = 0x01,

    /**
     * \brief \c <b>0x02</b> ТРК авторизована и ожидает снятия пистолета.
     */
    CL2_DS_AUTHORIZED = 0x02,

    /**
     * \brief \c <b>0x03</b> Идёт налив топлива.
     */
    CL2_DS_FUELLING = 0x03,

    /**
     * \brief \c <b>0x04</b> Налив топлива завершён.
     */
    CL2_DS_COMPLETE = 0x04,

    /**
     * \brief \c <b>0x05</b> Налив топлива приостановлен.
     */
    CL2_DS_HALTED = 0x05,

    /**
     * \brief \c <b>0x06</b> ТРК заблокирована.
     */
    CL2_DS_LOCKED = 0x06,

    /**
     * \brief \c <b>0x07</b> ТРК не отвечает.
     */
    CL2_DS_OFFLINE = 0x07,

    /**
     * \brief \c <b>0x08</b> Ошибка в работе ТРК.
     */
    CL2_DS_ERROR = 0x08

#ifdef ITP_COMPILE_CXX
} cl2_dispenser_status_t;
#else // ITP_COMPILE_CXX
} itp_cl2_dispenser_status_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление статусов заказа ТРК.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Заказ размещён, но не детализирован.
     */
    CL2_ORDER_EMPTY = 0x00,

    /**
     * \brief \c <b>0x01</b> Заказ размещён и ожидает авторизации.
     */
    CL2_ORDER_READY = 0x01,

    /**
     * \brief \c <b>0x02</b> Идёт отпуск топлива по заказу.
     */
    CL2_ORDER_DELIVERING = 0x02,

    /**
     * \brief \c <b>0x03</b> Отпуск топлива по заказу завершён.
     */
    CL2_ORDER_DELIVERED = 0x03,

    /**
     * \brief \c <b>0x04</b> Отпуск топлива прерван пользователем.
     */
    CL2_ORDER_ABORTED = 0x04,

    /**
     * \brief \c <b>0x05</b> Заказ завершён и зафиксирован.
     */
    CL2_ORDER_COMPLETE = 0x05,

    /**
     * \brief \c <b>0x06</b> Заказ отменён системой.
     */
    CL2_ORDER_CANCELLED = 0x06

#ifdef ITP_COMPILE_CXX
} cl2_order_status_t;
#else // ITP_COMPILE_CXX
} itp_cl2_order_status_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление способов налива при авторизации.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x01</b> До указанной суммы.
     */
    CL2_FUELING_TYPE_AMOUNT = 0x01,

    /**
     * \brief \c <b>0x02</b> До указанного объёма.
     */
    CL2_FUELING_TYPE_VOLUME = 0x02,

    /**
     * \brief \c <b>0x03</b> До полного бака.
     */
    CL2_FUELING_TYPE_FULL = 0x03

#ifdef ITP_COMPILE_CXX
} cl2_fueling_type_t;
#else // ITP_COMPILE_CXX
} itp_cl2_fueling_type_t;
#endif // ITP_COMPILE_CXX

#ifdef ITP_COMPILE_CXX
}
#endif // ITP_COMPILE_CXX

#endif // ITP_CL2_ENUM_H_INCLUDED
