
	Запуск вектора

> Редактировать конфиг
vim /etc/vector/vector.toml

> Запуск/остановка сервиса
systemctl [start|stop|restart] vector

> Посмотреть логи
journalctl -fu vector

> Посмотреть состояние
vector top

	Запуск кликхауса

> Редактировать конфиг
vim /etc/clickhouse-server/users.xml

> Разрешить SQL настройку пользователей в конфиге
access_management = 0/1

> Запуск/остановка сервиса
systemctl [start|stop|restart] clickhouse-server

> Работа с базами данных
clickhouse-client

	База данных

CREATE DATABASE prime;

CREATE TABLE prime.logs
(
    `device_id` String,
    `timestamp` DateTime,
    `level` Enum8('trace'=0, 'debug'=1, 'info'=2, 'warning'=3, 'error'=4, 'fatal'=5),
    `error` Nullable(String),
    `what` Nullable(String),
    `message` Nullable(String),
    INDEX idx_device_id(device_id) TYPE ngrambf_v1(5, 512, 6, 0) GRANULARITY 32768,
    INDEX idx_timestamp(timestamp) TYPE minmax GRANULARITY 32768
) ENGINE = MergeTree()
PARTITION BY toYYYYMMDD(timestamp)
ORDER BY (device_id, timestamp)
TTL timestamp + INTERVAL 3 MONTH
SETTINGS index_granularity = 8192;

CREATE TABLE prime.sales
(
    `device_id` String,
    `id` String,
    `dispenser` String,
    `time_begin` DateTime,
    `time_end` DateTime,
    `status` Enum8('empty'=0, 'ready'=1, 'delivering'=2, 'delivered'=3, 'aborted'=4, 'complete'=5, 'cancelled'=6),
    `product` String,
    `product_type` String,
    `order_type` Enum8('amount'=1, 'volume'=2, 'full'=3),
    `order_price` Decimal32(2),
    `order_value` Decimal32(3),
    `order_amount` Decimal32(2),
    `order_volume` Decimal32(3),
    `amount` Decimal32(2),
    `volume` Decimal32(3),
    `counter_begin` Nullable(Decimal64(3)),
    `counter_end` Nullable(Decimal64(3)),
    `product_begin` Nullable(Decimal64(3)),
    `product_end` Nullable(Decimal64(3)),
    INDEX idx_device_id(device_id) TYPE ngrambf_v1(5, 512, 6, 0) GRANULARITY 1024,
    INDEX idx_timestamp(time_begin) TYPE minmax GRANULARITY 1024
) ENGINE = MergeTree()
PARTITION BY toYYYYMMDD(time_begin)
ORDER BY (device_id, time_begin)
TTL time_begin + INTERVAL 12 MONTH
SETTINGS index_granularity = 8192;

CREATE TABLE prime.sensors
(
    `device_id` String,
    `timestamp` DateTime,
    `temperature` Nullable(Decimal32(1)),
    `humidity` Nullable(Decimal32(1)),
    `temperature_ext` Nullable(Decimal32(1)),
    INDEX idx_device_id(device_id) TYPE ngrambf_v1(5, 512, 6, 0) GRANULARITY 16384,
    INDEX idx_timestamp(timestamp) TYPE minmax GRANULARITY 16384
) ENGINE = MergeTree()
PARTITION BY toYYYYMMDD(timestamp)
ORDER BY (device_id, timestamp)
TTL timestamp + INTERVAL 6 MONTH
SETTINGS index_granularity = 8192;

CREATE TABLE prime.dido
(
    `device_id` String,
    `timestamp` DateTime,
    `di_0` Nullable(Bool),
    `di_1` Nullable(Bool),
    `di_2` Nullable(Bool),
    `di_3` Nullable(Bool),
    `di_4` Nullable(Bool),
    `di_5` Nullable(Bool),
    `di_6` Nullable(Bool),
    `di_7` Nullable(Bool),
    `do_0` Nullable(Bool),
    `do_1` Nullable(Bool),
    `do_2` Nullable(Bool),
    `do_3` Nullable(Bool),
    `do_4` Nullable(Bool),
    `do_5` Nullable(Bool),
    `do_6` Nullable(Bool),
    `do_7` Nullable(Bool),
    INDEX idx_device_id(device_id) TYPE ngrambf_v1(5, 512, 6, 0) GRANULARITY 16384,
    INDEX idx_timestamp(timestamp) TYPE minmax GRANULARITY 16384
) ENGINE = MergeTree()
PARTITION BY toYYYYMMDD(timestamp)
ORDER BY (device_id, timestamp)
TTL timestamp + INTERVAL 6 MONTH
SETTINGS index_granularity = 8192;

	Пользователи и доступы

CREATE ROLE read_tables;
GRANT SELECT ON prime.logs TO read_tables WITH GRANT OPTION;
GRANT SELECT ON prime.sales TO read_tables WITH GRANT OPTION;
GRANT SELECT ON prime.sensors TO read_tables WITH GRANT OPTION;
GRANT SELECT ON prime.dido TO read_tables WITH GRANT OPTION;
CREATE USER grafana IDENTIFIED WITH sha256_password BY 'grafana' DEFAULT ROLE read_tables;
DROP USER grafana;
DROP ROLE read_tables;

CREATE ROLE write_tables;
GRANT INSERT ON prime.logs TO write_tables WITH GRANT OPTION;
GRANT INSERT ON prime.sales TO write_tables WITH GRANT OPTION;
GRANT INSERT ON prime.sensors TO write_tables WITH GRANT OPTION;
GRANT INSERT ON prime.dido TO write_tables WITH GRANT OPTION;
CREATE USER vector IDENTIFIED WITH sha256_password BY 'vector' DEFAULT ROLE write_tables;
DROP USER vector;
DROP ROLE write_tables;
