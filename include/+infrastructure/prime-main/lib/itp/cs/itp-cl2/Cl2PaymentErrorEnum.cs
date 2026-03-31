namespace itp
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    // ReSharper disable InconsistentNaming
    // ReSharper disable CommentTypo

    /**
     * \brief Перечисление используемых платёжными сервисами кодов ошибок.
     * \ingroup errors_list
     */
    [JsonConverter(typeof(StringEnumConverter))]
    public enum Cl2PaymentErrorEnum
    {
        /**
         * \brief \c <b>0x00</b> Нет ошибки.
         */
        CL2_PAY_ERR_NONE = 0x00,

        /**
         * \brief \c <b>0x01</b> Универсальный код ошибки.
         *
         * Операция была отменена, подтверждение не требуется.
         */
        CL2_PAY_ERR_GENERIC_ERROR = 0x01,

        /**
         * \brief \c <b>0x02</b> Неверный пин-код.
         *
         * Операция была отменена, подтверждение не требуется.
         */
        CL2_PAY_ERR_INVALID_PIN = 0x02,

        /**
         * \brief \c <b>0x03</b> Карта заблокирована.
         *
         * Операция была отменена, подтверждение не требуется.
         */
        CL2_PAY_ERR_CARD_BLOCKED = 0x03,

        /**
         * \brief \c <b>0x04</b> Операция выполнена частично.
         *
         * Это код означает, что услуга выполнена не в полной мере. Например, транзакция одобрена лишь на часть средств.<br>
         * При возврате этого кода требуется дальнейшее подтверждение или отмена операции.
         */
        CL2_PAY_ERR_PARTIAL_OPERATION = 0x04,

        /**
         * \brief \c <b>0x05</b> Операция будет выполнена позже.
         *
         * При возврате этого кода операция считается выполненной, но результат не может быть получен из ответа.
         */
        CL2_PAY_ERR_DELAYED_OPERATION = 0x05
    }
}
