# Система логирования

## Описание

Проект использует Boost.Log для логирования всех событий в файлы и консоль.

## Структура логов

Логи сохраняются в папку `logs/` в корне проекта:

- `app_YYYY-MM-DD_NNN.log` - основной лог всех событий
- `errors_YYYY-MM-DD_NNN.log` - только ошибки и критические события

## Ротация логов

- **По размеру**: каждый файл до 10 МБ (основной) / 5 МБ (ошибки)
- **По времени**: новый файл каждый день в полночь
- **Автоматическая нумерация**: `_001.log`, `_002.log` и т.д.

## Уровни логирования

- `TRACE` - детальная отладочная информация
- `DEBUG` - отладочная информация
- `INFO` - информационные сообщения
- `WARN` - предупреждения
- `ERROR` - ошибки
- `FATAL` - критические ошибки

## Каналы логирования

- `UART` - события UART/DWIN экрана
- `HTTP` - HTTP запросы к PRIME
- `ARKAIM` - события платежного процессинга
- `SYSTEM` - системные события

## Использование в коде

### Базовое использование

```cpp
#include "bridge/Logger.hpp"

// UART канал
LOG_UART_INFO << "Touch event: VP=0x" << std::hex << vp;
LOG_UART_ERROR << "Failed to parse message";
LOG_UART_DEBUG << "Received " << bytes << " bytes";

// HTTP канал
LOG_HTTP_INFO << "Sending request to PRIME";
LOG_HTTP_ERROR << "HTTP request failed: " << error;

// ARKAIM канал
LOG_ARKAIM_INFO << "Payment started";
LOG_ARKAIM_ERROR << "Payment failed";

// Системный канал
LOG_SYSTEM_INFO << "Application started";
LOG_SYSTEM_FATAL << "Critical error: " << e.what();
```

### Все доступные макросы

```cpp
// UART
LOG_UART_TRACE, LOG_UART_DEBUG, LOG_UART_INFO
LOG_UART_WARN, LOG_UART_ERROR, LOG_UART_FATAL

// HTTP
LOG_HTTP_TRACE, LOG_HTTP_DEBUG, LOG_HTTP_INFO
LOG_HTTP_WARN, LOG_HTTP_ERROR, LOG_HTTP_FATAL

// ARKAIM
LOG_ARKAIM_TRACE, LOG_ARKAIM_DEBUG, LOG_ARKAIM_INFO
LOG_ARKAIM_WARN, LOG_ARKAIM_ERROR, LOG_ARKAIM_FATAL

// SYSTEM
LOG_SYSTEM_TRACE, LOG_SYSTEM_DEBUG, LOG_SYSTEM_INFO
LOG_SYSTEM_WARN, LOG_SYSTEM_ERROR, LOG_SYSTEM_FATAL
```

## Формат записи

```
[2026-04-30 12:34:56.789123] [UART] [info] Touch event: VP=0x1234
[2026-04-30 12:34:56.790456] [HTTP] [debug] Sending request to PRIME
[2026-04-30 12:34:56.791789] [SYSTEM] [error] Configuration error
```

## Инициализация

Логгер автоматически инициализируется в `main()`:

```cpp
int main() {
    initLogger("logs");  // Указываем папку для логов
    
    LOG_SYSTEM_INFO << "Application started";
    // ...
}
```

## Настройка уровня логирования

По умолчанию минимальный уровень: `DEBUG`

Чтобы изменить, отредактируйте в `Logger.cpp`:

```cpp
// Для production - только INFO и выше
logging::core::get()->set_filter(
    logging::trivial::severity >= logging::trivial::info
);

// Для отладки - все сообщения включая TRACE
logging::core::get()->set_filter(
    logging::trivial::severity >= logging::trivial::trace
);
```

## Примеры логирования

### Логирование нажатий на экран

```cpp
void DwinLogic::handleDwinEvent(const Message &message, MessageLayer &core) {
    uint16_t vp = (message.payload[0] << 8) | message.payload[1];
    uint16_t value = (message.payload[3] << 8) | message.payload[4];
    
    LOG_UART_INFO << "Touch event: VP=0x" << std::hex << vp 
                  << " Value=0x" << value << std::dec;
}
```

### Логирование HTTP запросов

```cpp
void HttpLogic::sendRequest(const std::string& endpoint) {
    LOG_HTTP_INFO << "Sending request to: " << endpoint;
    
    try {
        // ... отправка запроса
        LOG_HTTP_DEBUG << "Response received: " << response;
    } catch (std::exception& e) {
        LOG_HTTP_ERROR << "Request failed: " << e.what();
    }
}
```

### Логирование ошибок

```cpp
try {
    // ... код
} catch (std::exception& e) {
    LOG_SYSTEM_ERROR << "Exception caught: " << e.what();
    throw;
}
```

## Просмотр логов

### Linux/Mac
```bash
# Последние 100 строк
tail -n 100 logs/app_2026-04-30_001.log

# Следить за логом в реальном времени
tail -f logs/app_2026-04-30_001.log

# Поиск ошибок
grep ERROR logs/app_*.log

# Фильтр по каналу
grep "\[UART\]" logs/app_*.log
```

### Windows
```powershell
# Последние 100 строк
Get-Content logs\app_2026-04-30_001.log -Tail 100

# Следить за логом
Get-Content logs\app_2026-04-30_001.log -Wait -Tail 50

# Поиск ошибок
Select-String -Path logs\app_*.log -Pattern "ERROR"
```

## Производительность

- Логирование асинхронное (не блокирует основной поток)
- Автоматический flush после каждой записи
- Потокобезопасность (можно логировать из разных потоков)

## Отключение логирования в консоль

Если нужно логировать только в файл, закомментируйте в `Logger.cpp`:

```cpp
// auto consoleSink = logging::add_console_log(...);
```
