#ifndef ITP_CL1_ENUM_H_INCLUDED
#define ITP_CL1_ENUM_H_INCLUDED

/**
 * \file enum.h
 * \brief Файл содержит набор используемых кодов ошибок.
 */

#ifdef ITP_COMPILE_CXX
namespace itp {
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление статусов ТРК.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> ТРК сброшена.
     */
    CL1_DS_IDLE = 0x00,

    /**
     * \brief \c <b>0x01</b> Пистолет снят, ТРК ожидает авторизации.
     */
    CL1_DS_NOZZLE_UP = 0x01,

    /**
     * \brief \c <b>0x02</b> ТРК авторизована и ожидает снятия пистолета.
     */
    CL1_DS_AUTHORIZED = 0x02,

    /**
     * \brief \c <b>0x03</b> Идёт налив топлива.
     */
    CL1_DS_FUELLING = 0x03,

    /**
     * \brief \c <b>0x04</b> Налив топлива завершён.
     */
    CL1_DS_COMPLETE = 0x04,

    /**
     * \brief \c <b>0x05</b> Налив топлива приостановлен.
     */
    CL1_DS_HALTED = 0x05,

    /**
     * \brief \c <b>0x06</b> ТРК заблокирована.
     */
    CL1_DS_LOCKED = 0x06,

    /**
     * \brief \c <b>0x07</b> ТРК не отвечает.
     */
    CL1_DS_OFFLINE = 0x07,

    /**
     * \brief \c <b>0x08</b> Ошибка в работе ТРК.
     */
    CL1_DS_ERROR = 0x08

#ifdef ITP_COMPILE_CXX
} cl1_dispenser_status_t;
#else // ITP_COMPILE_CXX
} itp_cl1_dispenser_status_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление способов налива при авторизации.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x01</b> До указанной суммы.
     */
    CL1_FUELING_TYPE_AMOUNT = 0x01,

    /**
     * \brief \c <b>0x02</b> До указанного объёма.
     */
    CL1_FUELING_TYPE_VOLUME = 0x02,

    /**
     * \brief \c <b>0x03</b> До полного бака.
     */
    CL1_FUELING_TYPE_FULL = 0x03

#ifdef ITP_COMPILE_CXX
} cl1_fueling_type_t;
#else // ITP_COMPILE_CXX
} itp_cl1_fueling_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление возможных вариантов округления суммы при наливе.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Округлять вниз.
     */
    CL1_ROUND_FLOOR = 0x00,

    /**
     * \brief \c <b>0x01</b> Округлять вверх.
     */
    CL1_ROUND_CEIL = 0x01,

    /**
     * \brief \c <b>0x02</b> Округлять до ближайшего.
     */
    CL1_ROUND_NEAREST = 0x02

#ifdef ITP_COMPILE_CXX
} cl1_round_type_t;
#else // ITP_COMPILE_CXX
} itp_cl1_round_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление возможных вариантов размещения заказа на ТРК.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Размещать заказ без пересчёта.
     */
    CL1_AUTHORIZE_AS_IS = 0x00,

    /**
     * \brief \c <b>0x01</b> Размещать заказ, округлённый до разрядности ТРК.
     */
    CL1_AUTHORIZE_ROUND = 0x01,

    /**
     * \brief \c <b>0x02</b> Размещать округлённые литры.
     */
    CL1_AUTHORIZE_VOLUME = 0x02

#ifdef ITP_COMPILE_CXX
} cl1_authorization_type_t;
#else // ITP_COMPILE_CXX
} itp_cl1_authorization_type_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление параметров универсальных датчиков.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x01</b> Признак успешного обмена данными с устройством.
     */
    CL1_USD_ONLINE = 0x01,

    /**
     * \brief \c <b>0x02</b> Уровень основного поплавка (м).
     */
    CL1_USD_UPPER_LEVEL = 0x02,

    /**
     * \brief \c <b>0x03</b> Уровень нижнего поплавка (м).
     */
    CL1_USD_LOWER_LEVEL = 0x03,

    /**
     * \brief \c <b>0x04</b> Объём основного продукта (м³).
     */
    CL1_USD_UPPER_VOLUME = 0x04,

    /**
     * \brief \c <b>0x05</b> Объём подтоварной жидкости (м³).
     */
    CL1_USD_LOWER_VOLUME = 0x05,

    /**
     * \brief \c <b>0x06</b> Общий объём (м³).
     */
    CL1_USD_TOTAL_VOLUME = 0x06,

    /**
     * \brief \c <b>0x07</b> Температура (ºC).
     */
    CL1_USD_TEMPERATURE = 0x07,

    /**
     * \brief \c <b>0x08</b> Масса (кг).
     */
    CL1_USD_WEIGHT = 0x08,

    /**
     * \brief \c <b>0x09</b> Плотность (кг/м³).
     */
    CL1_USD_DENSITY = 0x09,

    /**
     * \brief \c <b>0x0A</b> Заполнение (%).
     */
    CL1_USD_FILLING = 0x0A,

    /**
     * \brief \c <b>0x0B</b> Концентрация (%).
     */
    CL1_USD_CONCENTRATION = 0x0B,

    /**
     * \brief \c <b>0x0C</b> Наличие заземления <code>\link itp_cl1_usd_ground_status_t \endlink</code>.
     */
    CL1_USD_GROUND = 0x0C,

    /**
     * \brief \c <b>0x0D</b> Контрольный ток (мА).
     */
    CL1_USD_CONTROL_CURRENT = 0x0D,

    /**
     * \brief \c <b>0x0E</b> Идентификатор устройства.
     */
    CL1_USD_DEVICE_ID = 0x0E,

    /**
     * \brief \c <b>0x0F</b> Статус устройства.
     */
    CL1_USD_DEVICE_STATUS = 0x0F,

    /**
     * \brief \c <b>0x10</b> Ошибка на устройстве.
     */
    CL1_USD_DEVICE_ERROR = 0x10,

    /**
     * \brief \c <b>0x11</b> Значение сумматора жидкости.
     */
    CL1_USD_FLUID_COUNTER = 0x11,

    /**
     * \brief \c <b>0x12</b> Значение сумматора жидкости (несбрасываемый).
     */
    CL1_USD_FLUID_COUNTER_NON_RESET = 0x12,

    /**
     * \brief \c <b>0x13</b> Текущее значение потока жидкости.
     */
    CL1_USD_CURRENT_FLUID_FLOW = 0x13,

    /**
     * \brief \c <b>0x14</b> Время пребывания во включённом состоянии.
     */
    CL1_USD_DEVICE_UP_TIME = 0x14,

    /**
     * \brief \c <b>0x15</b> Время жизни устройства.
     */
    CL1_USD_DEVICE_LIVE_TIME = 0x15,

    /**
     * \brief \c <b>0x16</b> Количество включений за время жизни.
     */
    CL1_USD_DEVICE_UPS_PER_LIVE = 0x16,

    /**
     * \brief \c <b>0x17</b> Частота событий.
     */
    CL1_USD_FREQUENCY = 0x17,

    /**
     * \brief \c <b>0x18</b> Влажность (%).
     */
    CL1_USD_HUMIDITY = 0x18,

    /**
     * \brief \c <b>0x19</b> Давление.
     */
    CL1_USD_PRESSURE = 0x19,

    /**
     * \brief \c <b>0x21</b> Мощность в нагрузке (кВт*ч).
     */
    CL1_USD_ELECTRIC_POWER = 0x21,

    /**
     * \brief \c <b>0x22-0x26</b> Потреблённая активная энергия по тарифам (кВт*ч).
     */
    CL1_USD_ACTIVE_ENERGY_T1 = 0x22,
    CL1_USD_ACTIVE_ENERGY_T2 = 0x23,
    CL1_USD_ACTIVE_ENERGY_T3 = 0x24,
    CL1_USD_ACTIVE_ENERGY_T4 = 0x25,
    CL1_USD_ACTIVE_ENERGY_T5 = 0x26,

    /**
     * \brief \c <b>0x27-0x2B</b> Потреблённая реактивная энергия по тарифам (кВт*ч).
     */
    CL1_USD_REACTIVE_ENERGY_T1 = 0x27,
    CL1_USD_REACTIVE_ENERGY_T2 = 0x28,
    CL1_USD_REACTIVE_ENERGY_T3 = 0x29,
    CL1_USD_REACTIVE_ENERGY_T4 = 0x2A,
    CL1_USD_REACTIVE_ENERGY_T5 = 0x2B,

    /**
     * \brief \c <b>0x2C-0x2E</b> Текущая активная мощность по фазам (кВт).
     */
    CL1_USD_ACTIVE_POWER_P1 = 0x2C,
    CL1_USD_ACTIVE_POWER_P2 = 0x2D,
    CL1_USD_ACTIVE_POWER_P3 = 0x2E,

    /**
     * \brief \c <b>0x2F-0x31</b> Текущая реактивная мощность по фазам (кВт).
     */
    CL1_USD_REACTIVE_POWER_P1 = 0x2F,
    CL1_USD_REACTIVE_POWER_P2 = 0x30,
    CL1_USD_REACTIVE_POWER_P3 = 0x31,

    /**
     * \brief \c <b>0x32-0x34</b> Текущий ток по фазам (А).
     */
    CL1_USD_ELECTRIC_CURRENT_P1 = 0x32,
    CL1_USD_ELECTRIC_CURRENT_P2 = 0x33,
    CL1_USD_ELECTRIC_CURRENT_P3 = 0x34,

    /**
     * \brief \c <b>0x35-0x37</b> Текущее напряжение по фазам (В).
     */
    CL1_USD_VOLTAGE_P1 = 0x35,
    CL1_USD_VOLTAGE_P2 = 0x36,
    CL1_USD_VOLTAGE_P3 = 0x37,

    /**
     * \brief \c <b>0x41-0x80</b> Дискретные вводы 001-064.
     */
    CL1_USD_DI_001 = 0x41,
    CL1_USD_DI_002 = 0x42,
    CL1_USD_DI_003 = 0x43,
    CL1_USD_DI_004 = 0x44,
    CL1_USD_DI_005 = 0x45,
    CL1_USD_DI_006 = 0x46,
    CL1_USD_DI_007 = 0x47,
    CL1_USD_DI_008 = 0x48,
    CL1_USD_DI_009 = 0x49,
    CL1_USD_DI_010 = 0x4A,
    CL1_USD_DI_011 = 0x4B,
    CL1_USD_DI_012 = 0x4C,
    CL1_USD_DI_013 = 0x4D,
    CL1_USD_DI_014 = 0x4E,
    CL1_USD_DI_015 = 0x4F,
    CL1_USD_DI_016 = 0x50,
    CL1_USD_DI_017 = 0x51,
    CL1_USD_DI_018 = 0x52,
    CL1_USD_DI_019 = 0x53,
    CL1_USD_DI_020 = 0x54,
    CL1_USD_DI_021 = 0x55,
    CL1_USD_DI_022 = 0x56,
    CL1_USD_DI_023 = 0x57,
    CL1_USD_DI_024 = 0x58,
    CL1_USD_DI_025 = 0x59,
    CL1_USD_DI_026 = 0x5A,
    CL1_USD_DI_027 = 0x5B,
    CL1_USD_DI_028 = 0x5C,
    CL1_USD_DI_029 = 0x5D,
    CL1_USD_DI_030 = 0x5E,
    CL1_USD_DI_031 = 0x5F,
    CL1_USD_DI_032 = 0x60,
    CL1_USD_DI_033 = 0x61,
    CL1_USD_DI_034 = 0x62,
    CL1_USD_DI_035 = 0x63,
    CL1_USD_DI_036 = 0x64,
    CL1_USD_DI_037 = 0x65,
    CL1_USD_DI_038 = 0x66,
    CL1_USD_DI_039 = 0x67,
    CL1_USD_DI_040 = 0x68,
    CL1_USD_DI_041 = 0x69,
    CL1_USD_DI_042 = 0x6A,
    CL1_USD_DI_043 = 0x6B,
    CL1_USD_DI_044 = 0x6C,
    CL1_USD_DI_045 = 0x6D,
    CL1_USD_DI_046 = 0x6E,
    CL1_USD_DI_047 = 0x6F,
    CL1_USD_DI_048 = 0x70,
    CL1_USD_DI_049 = 0x71,
    CL1_USD_DI_050 = 0x72,
    CL1_USD_DI_051 = 0x73,
    CL1_USD_DI_052 = 0x74,
    CL1_USD_DI_053 = 0x75,
    CL1_USD_DI_054 = 0x76,
    CL1_USD_DI_055 = 0x77,
    CL1_USD_DI_056 = 0x78,
    CL1_USD_DI_057 = 0x79,
    CL1_USD_DI_058 = 0x7A,
    CL1_USD_DI_059 = 0x7B,
    CL1_USD_DI_060 = 0x7C,
    CL1_USD_DI_061 = 0x7D,
    CL1_USD_DI_062 = 0x7E,
    CL1_USD_DI_063 = 0x7F,
    CL1_USD_DI_064 = 0x80,

    /**
     * \brief \c <b>0x81-0x90</b> Дискретные выводы 001-016.
     */
    CL1_USD_DO_001 = 0x81,
    CL1_USD_DO_002 = 0x82,
    CL1_USD_DO_003 = 0x83,
    CL1_USD_DO_004 = 0x84,
    CL1_USD_DO_005 = 0x85,
    CL1_USD_DO_006 = 0x86,
    CL1_USD_DO_007 = 0x87,
    CL1_USD_DO_008 = 0x88,
    CL1_USD_DO_009 = 0x89,
    CL1_USD_DO_010 = 0x8A,
    CL1_USD_DO_011 = 0x8B,
    CL1_USD_DO_012 = 0x8C,
    CL1_USD_DO_013 = 0x8D,
    CL1_USD_DO_014 = 0x8E,
    CL1_USD_DO_015 = 0x8F,
    CL1_USD_DO_016 = 0x90

#ifdef ITP_COMPILE_CXX
} cl1_usd_param_t;
#else // ITP_COMPILE_CXX
} itp_cl1_usd_param_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление состояний заземления.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Состояние заземления неизвестно.
     */
    CL1_GROUND_UNKNOWN = 0x00,

    /**
     * \brief \c <b>0x01</b> Заземления нет.
     */
    CL1_GROUND_NO = 0x01,

    /**
     * \brief \c <b>0x02</b> Заземление есть.
     */
    CL1_GROUND_YES = 0x02,

    /**
     * \brief \c <b>0x03</b> Короткое замыкание.
     */
    CL1_GROUND_SHORT = 0x03,

    /**
     * \brief \c <b>0x04</b> Обрыв.
     */
    CL1_GROUND_BREAK = 0x04

#ifdef ITP_COMPILE_CXX
} cl1_usd_ground_status_t;
#else // ITP_COMPILE_CXX
} itp_cl1_usd_ground_status_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление типов параметров универсальных датчиков.
 * \ingroup enums_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Десятичный 32-х разрядный тип.
     */
    CL1_USD_TYPE_DECIMAL32 = 0x00,

    /**
     * \brief \c <b>0x01</b> Десятичный 64-х разрядный тип.
     */
    CL1_USD_TYPE_DECIMAL64 = 0x01,

    /**
     * \brief \c <b>0x02</b> Булевый тип.
     */
    CL1_USD_TYPE_BOOLEAN = 0x02

#ifdef ITP_COMPILE_CXX
} cl1_usd_param_type_t;
#else // ITP_COMPILE_CXX
} itp_cl1_usd_param_type_t;
#endif // ITP_COMPILE_CXX

#ifdef ITP_COMPILE_CXX
}
#endif // ITP_COMPILE_CXX

#endif // ITP_CL1_ENUM_H_INCLUDED
