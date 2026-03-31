namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление команд считывателя карт.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2ReaderCommandEnum
    {
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
    }
}
