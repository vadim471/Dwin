# Инструкция по сборке проекта на ARM Debian 9 x32

## Дата: 06.05.2026

---

## Шаг 1: Проверка системы

Подключитесь к ARM устройству и проверьте архитектуру:

```bash
uname -m
# Должно показать: armv7l, armhf или похожее

cat /etc/debian_version
# Должно показать: 9.x

dpkg --print-architecture
# Должно показать: armhf или armel
```

---

## Шаг 2: Установка базовых инструментов

```bash
# Обновить список пакетов
sudo apt-get update

# Установить компилятор и CMake
sudo apt-get install -y build-essential cmake git

# Проверить версии
gcc --version
g++ --version
cmake --version
```

---

## Шаг 3: Установка зависимостей

### 3.1 Boost
```bash
sudo apt-get install -y libboost-all-dev

# Или конкретные компоненты:
sudo apt-get install -y \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-log-dev \
    libboost-filesystem-dev
```

### 3.2 Poco
```bash
sudo apt-get install -y \
    libpoco-dev \
    libpococrypto50 \
    libpoconet50 \
    libpocoutil50 \
    libpocofoundation50
```

**Если пакеты не найдены**, попробуйте:
```bash
apt-cache search poco | grep dev
# Установите найденные пакеты
```

### 3.3 OpenSSL
```bash
sudo apt-get install -y libssl-dev libssl1.0.2
```

### 3.4 SQLite3
```bash
sudo apt-get install -y libsqlite3-dev sqlite3
```

### 3.5 Другие зависимости
```bash
sudo apt-get install -y \
    libpthread-stubs0-dev \
    pkg-config
```

---

## Шаг 4: Копирование исходников

### Вариант A: Через SCP (с вашей машины)
```bash
# На вашей x86 машине
cd /path/to/dwin_exmpl
tar -czf dwin_exmpl.tar.gz .

# Копировать на ARM
scp dwin_exmpl.tar.gz user@arm-device:/home/user/

# На ARM устройстве
cd /home/user
tar -xzf dwin_exmpl.tar.gz
cd dwin_exmpl
```

### Вариант B: Через Git
```bash
# На ARM устройстве
cd /home/user
git clone <your-repo-url> dwin_exmpl
cd dwin_exmpl
```

### Вариант C: Через rsync
```bash
# На вашей x86 машине
rsync -avz --exclude 'cmake-build-*' \
    /path/to/dwin_exmpl/ \
    user@arm-device:/home/user/dwin_exmpl/
```

---

## Шаг 5: Проверка Arkaim Infrastructure

Проект использует Arkaim infrastructure. Проверьте наличие:

```bash
cd /home/user/dwin_exmpl

# Проверить пути в CMakeLists.txt
grep "ARKAIM_INFRASTRUCTURE_DIR" CMakeLists.txt

# Если infrastructure в проекте:
ls include/+infrastructure/

# Если нет, нужно скопировать или указать путь
```

**Если infrastructure отсутствует:**
```bash
# Установить переменную окружения
export ARKAIM_INFRASTRUCTURE_DIR=/path/to/infrastructure

# Или при запуске cmake:
cmake -B build -DARKAIM_INFRASTRUCTURE_DIR=/path/to/infrastructure
```

---

## Шаг 6: Настройка конфигурации

### 6.1 Обновить пути в config/config.json
```bash
nano config/config.json
```

Измените:
```json
{
    "serial": {
        "port": "/dev/ttyUSB0",  // Проверьте на ARM: ls /dev/tty*
        "baud_rate": 115200
    }
}
```

### 6.2 Обновить путь к config в constant.cpp
```bash
nano src/core/constant.cpp
```

Измените:
```cpp
const std::string CONFIG = "/home/user/dwin_exmpl/config/config.json";
// Или используйте относительный путь
const std::string CONFIG = "config.json";
```

---

## Шаг 7: Сборка проекта

### 7.1 Создать build директорию
```bash
cd /home/user/dwin_exmpl
mkdir -p build
cd build
```

### 7.2 Запустить CMake
```bash
cmake ..

# Если нужно указать Arkaim infrastructure:
cmake .. -DARKAIM_INFRASTRUCTURE_DIR=/path/to/infrastructure

# Если нужно указать компилятор:
cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

**Возможные ошибки:**

#### Ошибка: "Could not find Boost"
```bash
# Установить Boost
sudo apt-get install libboost-all-dev

# Или указать путь:
cmake .. -DBOOST_ROOT=/usr/include/boost
```

#### Ошибка: "Could not find Poco"
```bash
# Проверить наличие
dpkg -l | grep poco

# Если нет, собрать из исходников:
cd /tmp
wget https://pocoproject.org/releases/poco-1.9.0/poco-1.9.0-all.tar.gz
tar -xzf poco-1.9.0-all.tar.gz
cd poco-1.9.0-all
./configure --prefix=/usr/local
make -j4
sudo make install
```

#### Ошибка: "Could not find SQLite3"
```bash
sudo apt-get install libsqlite3-dev
```

### 7.3 Собрать проект
```bash
make -j4

# Или с подробным выводом:
make VERBOSE=1
```

---

## Шаг 8: Проверка сборки

```bash
# Проверить бинарник
ls -lh dwin_exmpl

# Проверить архитектуру
file dwin_exmpl
# Должно показать: ELF 32-bit LSB executable, ARM

# Проверить зависимости
ldd dwin_exmpl
```

---

## Шаг 9: Запуск

### 9.1 Скопировать config.json
```bash
cd /home/user/dwin_exmpl/build
cp ../config/config.json .
```

### 9.2 Создать директорию для логов
```bash
mkdir -p logs
```

### 9.3 Проверить права на serial порт
```bash
ls -l /dev/ttyUSB0
# Если нет прав:
sudo usermod -a -G dialout $USER
# Перелогиниться или:
sudo chmod 666 /dev/ttyUSB0
```

### 9.4 Запустить приложение
```bash
./dwin_exmpl

# Или с sudo (если нужны права на порт):
sudo ./dwin_exmpl
```

---

## Шаг 10: Отладка

### Проверить логи
```bash
tail -f logs/*.log
```

### Проверить базу данных
```bash
ls -lh application.db
sqlite3 application.db "SELECT * FROM transactions;"
```

### Проверить версию
```bash
./dwin_exmpl --version
# Или посмотреть в логах
```

---

## Возможные проблемы и решения

### Проблема 1: CMake не находит библиотеки
**Решение:**
```bash
# Установить pkg-config
sudo apt-get install pkg-config

# Проверить пути
pkg-config --list-all | grep -E "boost|poco|sqlite"
```

### Проблема 2: Ошибки компиляции C++14
**Решение:**
```bash
# Обновить компилятор (если возможно)
sudo apt-get install g++-6

# Или изменить стандарт в CMakeLists.txt:
set(CMAKE_CXX_STANDARD 11)
```

### Проблема 3: Недостаточно памяти при сборке
**Решение:**
```bash
# Собирать в один поток
make -j1

# Или добавить swap
sudo dd if=/dev/zero of=/swapfile bs=1M count=1024
sudo mkswap /swapfile
sudo swapon /swapfile
```

### Проблема 4: Arkaim infrastructure не найден
**Решение:**
```bash
# Скопировать infrastructure на ARM
# Или закомментировать в CMakeLists.txt, если не используется
```

---

## Оптимизация для ARM

### Флаги компиляции для ARM
Добавьте в `CMakeLists.txt`:
```cmake
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon")
endif()
```

### Уменьшение размера бинарника
```bash
# Собрать с оптимизацией размера
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make

# Strip символов
strip dwin_exmpl
```

---

## Автоматизация

### Скрипт для полной установки
Создайте `install_arm.sh`:
```bash
#!/bin/bash
set -e

echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y build-essential cmake \
    libboost-all-dev libpoco-dev libssl-dev libsqlite3-dev

echo "Building project..."
mkdir -p build
cd build
cmake ..
make -j4

echo "Setup complete!"
./dwin_exmpl --version
```

Запустите:
```bash
chmod +x install_arm.sh
./install_arm.sh
```

---

## Проверочный чеклист

- [ ] Архитектура ARM проверена
- [ ] Все зависимости установлены
- [ ] Исходники скопированы
- [ ] CMake успешно выполнен
- [ ] Проект собран без ошибок
- [ ] config.json настроен
- [ ] Serial порт доступен
- [ ] Приложение запускается
- [ ] База данных создается
- [ ] Логи пишутся

---

## Контакты и поддержка

Если возникли проблемы:
1. Проверьте логи: `tail -f logs/*.log`
2. Проверьте версии библиотек: `dpkg -l | grep -E "boost|poco|sqlite"`
3. Проверьте ошибки компиляции: `make VERBOSE=1`

---

**Удачи с запуском на ARM!** 🚀
