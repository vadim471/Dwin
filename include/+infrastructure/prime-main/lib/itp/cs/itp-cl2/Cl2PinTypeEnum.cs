namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление способов ввода пин-кода.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2PinTypeEnum
    {
        /**
         * \brief \c <b>0x00</b> Без шифрования.
         */
        CL2_PIN_TYPE_PLAIN = 0x00,

        /**
         * \brief \c <b>0x01</b> Шифрование DUKPT.
         */
        CL2_PIN_TYPE_DUKPT = 0x01
    }
}
