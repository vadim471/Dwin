namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление статусов ТРК.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2DispenserStatusEnum
    {
        /**
         * \brief \c <b>0x00</b> ТРК сброшена.
         */
        CL2_DS_IDLE = 0x00,

        /**
         * \brief \c <b>0x01</b> Пистолет снят, ТРК ожидает авторизации.
         */
        CL2_DS_NOZZLE_UP = 0x01,

        /**
         * \brief \c <b>0x02</b> ТРК авторизована и ожидает снятия пистолета.
         */
        CL2_DS_AUTHORIZED = 0x02,

        /**
         * \brief \c <b>0x03</b> Идёт налив топлива.
         */
        CL2_DS_FUELLING = 0x03,

        /**
         * \brief \c <b>0x04</b> Налив топлива завершён.
         */
        CL2_DS_COMPLETE = 0x04,

        /**
         * \brief \c <b>0x05</b> Налив топлива приостановлен.
         */
        CL2_DS_HALTED = 0x05,

        /**
         * \brief \c <b>0x06</b> ТРК заблокирована.
         */
        CL2_DS_LOCKED = 0x06,

        /**
         * \brief \c <b>0x07</b> ТРК не отвечает.
         */
        CL2_DS_OFFLINE = 0x07,

        /**
         * \brief \c <b>0x08</b> Ошибка в работе ТРК.
         */
        CL2_DS_ERROR = 0x08
    }
}
