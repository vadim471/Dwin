namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление дорожек на магнитной полосе.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2MagTrackTypeEnum
    {
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
    }
}
