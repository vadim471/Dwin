namespace itp
{
    using System;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление групп платёжных карт.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    [Flags]
    public enum Cl2ServiceCardEnum : uint
    {
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
    }
}
