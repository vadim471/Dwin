//
// Created by vadim.tissen on 26.02.2026.
//

#include "bridge/Logger.hpp"

// Инициализируем логгеры и привязываем их к конкретным каналам
SystemLogger uartLogger(boost::log::keywords::channel = "UART");
SystemLogger httpLogger(boost::log::keywords::channel = "HTTP");
