#ifndef MODBUS_SYMBOL_H_INCLUDED
#define MODBUS_SYMBOL_H_INCLUDED

/**
 * \file symbol.h
 * \brief Файл содержит управляющие символы протокола.
 */

/**
 * \brief Перечисление поддерживаемых команд протокола.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Чтение дискретных выводов (0x01).
     */
    MBS_CMD_READ_COILS = 0x01,
    /**
     * \brief Чтение дискретных вводов (0x02).
     */
    MBS_CMD_READ_DISCRETE_INPUTS = 0x02,
    /**
     * \brief Чтение аналоговых выводов (0x03).
     */
    MBS_CMD_READ_HOLDING_REGISTERS = 0x03,
    /**
     * \brief Чтение аналоговых вводов (0x04).
     */
    MBS_CMD_READ_INPUT_REGISTERS = 0x04,
    /**
     * \brief Запись дискретного вывода (0x05).
     */
    MBS_CMD_WRITE_SINGLE_COIL = 0x05,
    /**
     * \brief Запись аналогового вывода (0x06).
     */
    MBS_CMD_WRITE_SINGLE_REGISTER = 0x06,
    /**
     * \brief Запись дискретных выводов (0x0F).
     */
    MBS_CMD_WRITE_MULTIPLE_COILS = 0x0F,
    /**
     * \brief Запись аналоговых выводов (0x10).
     */
    MBS_CMD_WRITE_MULTIPLE_REGISTERS = 0x10
} mbs_command_t;

/**
 * \brief Перечисление ошибок протокола.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Ошибка отсутствует.
     */
    MBS_ERR_NONE = 0x00,
    /**
     * \brief Принятый код функции не может быть обработан.
     */
    MBS_ERR_ILLEGAL_FUNCTION = 0x01,
    /**
     * \brief Адрес данных, указанный в запросе, недоступен.
     */
    MBS_ERR_ILLEGAL_DATA_ADDRESS = 0x02,
    /**
     * \brief Значение, содержащееся в поле данных запроса, является недопустимой величиной.
     */
    MBS_ERR_ILLEGAL_DATA_VALUE = 0x03,
    /**
     * \brief Невосстанавливаемая ошибка имела место, пока ведомое устройство пыталось выполнить затребованное действие.
     */
    MBS_ERR_SLAVE_DEVICE_FAILURE = 0x04,
    /**
     * \brief Ведомое устройство приняло запрос и обрабатывает его, но это требует много времени.
     *
     * Этот ответ предохраняет ведущее устройство от генерации ошибки тайм-аута.
     */
    MBS_ERR_ACKNOWLEDGE = 0x05,
    /**
     * \brief Ведомое устройство занято обработкой команды.
     *
     * Ведущее устройство должно повторить сообщение позже, когда ведомое освободится.
     */
    MBS_ERR_SLAVE_DEVICE_BUSY = 0x06,
    /**
     * \brief Ведомое устройство не может выполнить программную функцию, заданную в запросе.
     *
     * Этот код возвращается для неуспешного программного запроса, использующего функции с номерами 13 или 14.
     * Ведущее устройство должно запросить диагностическую информацию или информацию об ошибках от ведомого.
     */
    MBS_ERR_NEGATIVE_ACKNOWLEDGE = 0x07,
    /**
     * \brief Ведомое устройство при чтении расширенной памяти обнаружило ошибку паритета.
     *
     * Ведущее устройство может повторить запрос, но обычно в таких случаях требуется ремонт.
     */
    MBS_ERR_MEMORY_PARITY_ERROR = 0x08,
    /**
     * \brief Запрос отменён. Код ошибки библиотеки.
     */
    MBS_ERR_CANCELLED = 0x81,
    /**
     * \brief Время ожидания истекло. Код ошибки библиотеки.
     */
    MBS_ERR_TIMEOUT = 0x82,
    /**
     * \brief Неверные данные. Код ошибки библиотеки.
     */
    MBS_ERR_BAD_DATA = 0x83,
    /**
     * \brief Внутренняя ошибка. Код ошибки библиотеки.
     */
    MBS_ERR_INTERNAL = 0x84
} mbs_error_t;

/**
 * \brief Перечисление состояний линии передачи данных.
 *
 * Используется для выполнения логики приёма/передачи команд.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Линия не занята.
     */
    MBS_LINE_IDLE = 0,
    /**
     * \brief Необходимо подготовить кадр.
     */
    MBS_LINE_PREPARE_FRAME,
    /**
     * \brief Идёт отправка кадра.
     */
    MBS_LINE_SEND_FRAME,
    /**
     * \brief Ожидается ответ на запрос.
     */
    MBS_LINE_WAIT_RESPONSE
} mbs_line_status_t;

#endif // MODBUS_SYMBOL_H_INCLUDED
