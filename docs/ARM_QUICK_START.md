# 🚀 Быстрый старт на ARM Debian 9

## Копирование проекта на ARM

```bash
# На вашей x86 машине
cd C:\CLionProjects\dwin_exmpl
tar -czf dwin_exmpl.tar.gz --exclude='cmake-build-*' --exclude='.git' .

# Копировать на ARM (замените user и ip)
scp dwin_exmpl.tar.gz user@192.168.1.100:/home/user/
```

## На ARM устройстве

### 1. Распаковать
```bash
cd /home/user
tar -xzf dwin_exmpl.tar.gz
cd dwin_exmpl
```

### 2. Установить зависимости (один раз)
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake \
    libboost-system-dev libboost-thread-dev libboost-log-dev \
    libpoco-dev libssl-dev libsqlite3-dev
```

### 3. Собрать
```bash
mkdir -p build && cd build
cmake ..
make -j2
```

### 4. Запустить
```bash
cp ../config/config.json .
./dwin_exmpl
```

## Если что-то не работает

### Проверить архитектуру
```bash
uname -m  # Должно быть armv7l или похожее
file dwin_exmpl  # Должно быть ARM
```

### Проверить зависимости
```bash
ldd dwin_exmpl  # Все библиотеки должны быть найдены
```

### Проверить serial порт
```bash
ls -l /dev/ttyUSB0
sudo chmod 666 /dev/ttyUSB0  # Если нет прав
```

### Посмотреть логи
```bash
tail -f logs/*.log
```

## Полная инструкция

См. `docs/ARM_BUILD_GUIDE.md` для детальной информации.

---

**Готово!** Проект должен работать на ARM. 🎉
