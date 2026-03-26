#include <itp/error.h>

const itp_error_description_t itp_errors[] = {
    { ITP_ERR_NONE,             "Ошибок не обнаружено" },
    { ITP_ERR_INTERNAL,         "Внутренняя ошибка на устройстве" },
    { ITP_ERR_CRC_FAILED,       "Контрольная сумма не совпала" },
    { ITP_ERR_TRANSMIT_FAILED,  "Ошибка приёма/передачи" },
    { ITP_ERR_TIMEOUT,          "Истекло время ожидания" },
    { ITP_ERR_WRONG_STATUS,     "Неверный статус запроса" },
    { ITP_ERR_WRONG_ADDRESS,    "Неверный виртуальный адрес" },
    { ITP_ERR_WRONG_COMMAND,    "Неверная команда" },
    { ITP_ERR_WRONG_SEQUENCE,   "Нарушение последовательности отправки" },
    { ITP_ERR_WRONG_PATH,       "Неверный путь к устройству" },
    { ITP_ERR_BAD_DATA,         "Неверные данные в пакете" },
    { ITP_ERR_NOT_CONFIGURED,   "Неверная конфигурация узла" },
    { ITP_ERR_UNKNOWN,          "Неизвестная ошибка в пакете" }
};

const char* itp_get_error_description(uint16_t error_code) {
    itp_size_t size = (itp_size_t)(sizeof(itp_errors) / sizeof(itp_errors[0]));
    for (itp_size_t i = 0; i < size; ++i) {
        if (itp_errors[i].error_code == error_code) {
            return itp_errors[i].description;
        }
    }
    return "Неизвестный код ошибки";
}

uint16_t itp_cast_error_code(itp_error_code_t error_code) {
    switch (error_code) {
        case ITP_ERRC_INTERNAL:
            return ITP_ERR_INTERNAL;
        case ITP_ERRC_TIMEOUT:
            return ITP_ERR_TIMEOUT;
        case ITP_ERRC_MALLOC_FAILED:
            return ITP_ERR_MALLOC_FAILED;
        case ITP_ERRC_NULL_POINTER:
            return ITP_ERR_NULL_POINTER;
        case ITP_ERRC_OUT_OF_BOUNDS:
            return ITP_ERR_BAD_DATA;
        default:
            return ITP_ERR_INTERNAL;
    }
}
