namespace itp
{
    using System;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление состояний и флагов устройств.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    [Flags]
    public enum Cl2DeviceStatusEnum : uint
    {
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
    }
}
