namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
	 * \brief Перечисление типов бесконтактных карт.
	 * \ingroup enums_list
	 */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2ContactlessCardEnum
    {
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
        CL2_CARD_BARCODE = 0x05
    }
}
