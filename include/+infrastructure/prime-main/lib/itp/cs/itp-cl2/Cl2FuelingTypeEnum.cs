namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление способов налива при авторизации.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2FuelingTypeEnum
    {
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
    }
}
