namespace itp
{
    using System;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление всех возможных типов API.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    [Flags]
    public enum Cl2ApiEnum : uint
    {
        /**
         * \brief \c <b>0x00</b> Пустой интерфейс, не поддерживает никаких команд.
         */
        CL2_API_NONE = 0x00,

        /**
         * \brief \c <b>0x01</b> Базовый интерфейс.
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
         * \brief \c <b>0x02</b> Основной сервис контроллера платежей.
         *
         * Поддерживает следующие команды:
         * <ul>
         * <li><code>\link CL2_CMD_CTR_RESOLVE_CARD_TYPE\endlink</code>.</li>
         * </ul>
         */
        CL2_API_CONTROLLER = 0x02,

        /**
         * \brief \c <b>0x04</b> Считыватель магнитных карт.
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
         * \brief \c <b>0x08</b> Бесконтактный считыватель.
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
         * \brief \c <b>0x10</b> Считыватель SAM-карт.
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
         * \brief \c <b>0x20</b> Пин-клавиатура.
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
         * \brief \c <b>0x40</b> Принтер чеков.
         *
         * Поддерживает следующие команды:
         * <ul>
         * <li><code>\link CL2_CMD_PRT_PRINT_TEXT\endlink</code>;</li>
         * <li><code>\link CL2_CMD_PRT_CUT_PAPER\endlink</code>.</li>
         * </ul>
         */
        CL2_API_PRINTER = 0x40,

        /**
         * \brief \c <b>0x80</b> Сервис работы с различными картами.
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
         * \brief \c <b>0x100</b> Поддержка банковских операций.
         */
        CL2_API_BANK_PROCESSING = 0x100,

        /**
         * \brief \c <b>0x200</b> Активный терминал (размещение заказов).
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
         * \brief \c <b>0x400</b> Сканер штрих-кодов.
         *
         * Поддерживает следующие команды:
         * <ul>
         * <li><code>\link CL2_CMD_BARCODE_READ\endlink</code>;</li>
         * </ul>
         */
        CL2_API_BARCODE_SCANNER = 0x400

    }
}
