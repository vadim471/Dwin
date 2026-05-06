# Интеграция SQLite в проект dwin

## Обзор

В проект добавлена поддержка SQLite для хранения двух типов данных:
1. **Транзакции (заказы)** - данные о платежных операциях
2. **Метрологические ведомости** - данные о топливе и измерениях

## Структура

### Файлы базы данных

- `include/bridge/Database.hpp` / `src/Database.cpp` - базовый класс для работы с SQLite
- `include/bridge/Transaction.hpp` / `src/Transaction.cpp` - модель транзакций
- `include/bridge/MetrologicalRecord.hpp` / `src/MetrologicalRecord.cpp` - модель метрологических записей

### База данных

Файл базы данных: `application.db` (создается автоматически при первом запуске)

## Использование

### Инициализация

```cpp
#include "bridge/Database.hpp"
#include "bridge/Transaction.hpp"
#include "bridge/MetrologicalRecord.hpp"

// Создать подключение к БД
auto db = std::make_shared<Database>("application.db");

// Создать репозитории
auto transactionRepo = std::make_shared<TransactionRepository>(db);
auto metrologicalRepo = std::make_shared<MetrologicalRecordRepository>(db);

// Создать таблицы (если не существуют)
transactionRepo->createTable();
metrologicalRepo->createTable();
```

### Работа с транзакциями

```cpp
// Вставка новой транзакции
TransactionData transaction;
transaction.shiftNumber = 5;
transaction.terminalId = "1KA00101";
transaction.mti = "0200";
transaction.year = 2022;
transaction.month = 2;
transaction.day = 22;
transaction.hour = 15;
transaction.minute = 20;
transaction.second = 56;
transaction.amountInKops = 500;
transaction.goodsPumpNumber = 1;
transaction.goodsProductCode = "0001000095";
transaction.goodsProductNameUtf8 = "АИ-95";
transaction.goodsQuantityInMilliliters = 5000;
transaction.goodsPriceInKopsByLiter = 100;
transaction.rrn = "205377107042";
transaction.authCode = "363249";
transaction.responseCode = "00";

int64_t id = transactionRepo->insert(transaction);

// Получить транзакцию по ID
TransactionData t = transactionRepo->getById(id);

// Получить все транзакции
std::vector<TransactionData> all = transactionRepo->getAll();

// Получить транзакции по номеру смены
std::vector<TransactionData> byShift = transactionRepo->getByShiftNumber(5);

// Получить транзакции по RRN
std::vector<TransactionData> byRRN = transactionRepo->getByRRN("205377107042");

// Обновить транзакцию
transaction.responseCode = "01";
transactionRepo->update(transaction);

// Удалить транзакцию
transactionRepo->deleteById(id);

// Получить количество транзакций
int64_t count = transactionRepo->count();
```

### Работа с метрологическими записями

```cpp
// Вставка новой записи
MetrologicalRecordData record;
record.date = "2022-04-29T17:40:16.070";
record.density = "0";
record.filling = "0";
record.fuelName = "ДТ";
record.idTso = "757586";
record.lowerLevel = "0";
record.lowerVolume = "0";
record.namePmp = "101";
record.temperature = "0";
record.totalVolume = "10";
record.upperLevel = "0";
record.upperVolume = "10";
record.weight = "0";

int64_t id = metrologicalRepo->insert(record);

// Получить запись по ID
MetrologicalRecordData r = metrologicalRepo->getById(id);

// Получить все записи
std::vector<MetrologicalRecordData> all = metrologicalRepo->getAll();

// Получить записи по IdTso
std::vector<MetrologicalRecordData> byIdTso = metrologicalRepo->getByIdTso("757586");

// Получить записи по названию топлива
std::vector<MetrologicalRecordData> byFuel = metrologicalRepo->getByFuelName("ДТ");

// Получить записи по названию ТРК
std::vector<MetrologicalRecordData> byPmp = metrologicalRepo->getByNamePmp("101");

// Получить записи за период
std::vector<MetrologicalRecordData> byRange = metrologicalRepo->getByDateRange(
    "2022-04-01T00:00:00", 
    "2022-04-30T23:59:59"
);

// Обновить запись
record.totalVolume = "15";
metrologicalRepo->update(record);

// Удалить запись
metrologicalRepo->deleteById(id);

// Получить количество записей
int64_t count = metrologicalRepo->count();
```

## Схема базы данных

### Таблица transactions

| Поле | Тип | Описание |
|------|-----|----------|
| id | INTEGER PRIMARY KEY | Уникальный идентификатор |
| shift_number | INTEGER | Номер смены |
| is_reversal_transaction | INTEGER | Флаг отмены транзакции |
| open_way_card_type | INTEGER | Тип карты |
| card_id_hash | TEXT | Хеш ID карты |
| card_id_hash_salt | TEXT | Соль для хеша |
| terminal_id | TEXT | ID терминала |
| mti | TEXT | Message Type Indicator |
| transaction_year | INTEGER | Год транзакции |
| transaction_month | INTEGER | Месяц транзакции |
| transaction_day | INTEGER | День транзакции |
| transaction_hour | INTEGER | Час транзакции |
| transaction_minute | INTEGER | Минута транзакции |
| transaction_second | INTEGER | Секунда транзакции |
| amount_in_kops | INTEGER | Сумма в копейках |
| goods_pump_number | INTEGER | Номер колонки |
| goods_product_code | TEXT | Код товара |
| goods_product_name_utf8 | TEXT | Название товара |
| goods_quantity_in_milliliters | INTEGER | Количество в миллилитрах |
| goods_price_in_kops_by_liter | INTEGER | Цена за литр в копейках |
| rrn | TEXT | Retrieval Reference Number |
| auth_code | TEXT | Код авторизации |
| response_code | TEXT | Код ответа |
| secure_data | TEXT | Зашифрованные данные |
| secure_data_initialization_vector | TEXT | Вектор инициализации |
| secure_data_hash | TEXT | Хеш данных |
| secure_data_hash_salt | TEXT | Соль для хеша |
| created_at | TIMESTAMP | Время создания записи |

**Индексы:**
- idx_shift_number (shift_number)
- idx_rrn (rrn)
- idx_created_at (created_at)

### Таблица metrological_records

| Поле | Тип | Описание |
|------|-----|----------|
| id | INTEGER PRIMARY KEY | Уникальный идентификатор |
| date | TEXT | Дата записи |
| density | TEXT | Плотность |
| filling | TEXT | Заполнение |
| fuel_name | TEXT | Название топлива |
| id_tso | TEXT | ID ТСО |
| lower_level | TEXT | Нижний уровень |
| lower_volume | TEXT | Нижний объем |
| name_pmp | TEXT | Название ТРК |
| temperature | TEXT | Температура |
| total_volume | TEXT | Общий объем |
| upper_level | TEXT | Верхний уровень |
| upper_volume | TEXT | Верхний объем |
| weight | TEXT | Вес |
| created_at | TIMESTAMP | Время создания записи |

**Индексы:**
- idx_id_tso (id_tso)
- idx_fuel_name (fuel_name)
- idx_name_pmp (name_pmp)
- idx_date (date)
- idx_created_at (created_at)

## Обработка ошибок

Все методы могут выбросить исключение `DatabaseException` при ошибках:

```cpp
try {
    transactionRepo->insert(transaction);
} catch (const DatabaseException& e) {
    LOG_SYSTEM_ERROR << "Database error: " << e.what();
}
```

## Требования для сборки

1. **SQLite3** должен быть установлен в системе
2. CMake должен найти SQLite3 через `find_package(SQLite3 REQUIRED)`

### Установка SQLite3 на Windows

Через vcpkg:
```bash
vcpkg install sqlite3:x64-windows
```

Или скачать с официального сайта: https://www.sqlite.org/download.html

### Установка SQLite3 на Linux

```bash
sudo apt-get install libsqlite3-dev
```

## Тестирование

В меню приложения добавлены опции для тестирования базы данных:
- Опция 3: Вставить тестовую транзакцию
- Опция 4: Вставить тестовую метрологическую запись
- Опция 5: Показать все транзакции
- Опция 6: Показать все метрологические записи
