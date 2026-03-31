namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление типов штрих-кодов.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2BarcodeTypeEnum : uint
    {
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
    }
}
