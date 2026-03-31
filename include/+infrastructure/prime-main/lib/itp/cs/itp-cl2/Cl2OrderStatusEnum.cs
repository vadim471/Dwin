namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление статусов заказа ТРК.
     * \ingroup enums_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2OrderStatusEnum
    {
        /**
         * \brief \c <b>0x00</b> Заказ размещён, но не детализирован.
         */
        CL2_ORDER_EMPTY = 0x00,

        /**
         * \brief \c <b>0x01</b> Заказ размещён и ожидает авторизации.
         */
        CL2_ORDER_READY = 0x01,

        /**
         * \brief \c <b>0x02</b> Идёт отпуск топлива по заказу.
         */
        CL2_ORDER_DELIVERING = 0x02,

        /**
         * \brief \c <b>0x03</b> Отпуск топлива по заказу завершён.
         */
        CL2_ORDER_DELIVERED = 0x03,

        /**
         * \brief \c <b>0x04</b> Отпуск топлива прерван пользователем.
         */
        CL2_ORDER_ABORTED = 0x04,

        /**
         * \brief \c <b>0x05</b> Заказ завершён и зафиксирован.
         */
        CL2_ORDER_COMPLETE = 0x05,

        /**
         * \brief \c <b>0x06</b> Заказ отменён системой.
         */
        CL2_ORDER_CANCELLED = 0x06
    }
}
