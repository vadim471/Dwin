namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление всех возможных типов устройств.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2DevEnum
    {
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
    }
}
