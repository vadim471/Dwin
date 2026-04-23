cmake_minimum_required(VERSION 3.5)
project(dwin_exmpl)

set(CMAKE_CXX_STANDARD 14)

set(ARKAIM_INFRASTRUCTURE_DIR "" CACHE PATH "Path to forstandalone/+infrastructure used by terminal")

if(NOT ARKAIM_INFRASTRUCTURE_DIR OR NOT EXISTS "${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cpp/source/itp/root.cpp")
    set(_ARKAIM_INFRA_CANDIDATES
        "${CMAKE_SOURCE_DIR}/include/+infrastructure"
        "C:/Users/vadim.tissen/Desktop/Standalone/forstandalone/+infrastructure"
        "${CMAKE_SOURCE_DIR}/../+infrastructure"
        "${CMAKE_SOURCE_DIR}/../forstandalone/+infrastructure"
        "$ENV{ARKAIM_INFRASTRUCTURE_DIR}"
    )

    foreach(_candidate IN LISTS _ARKAIM_INFRA_CANDIDATES)
        if(_candidate AND EXISTS "${_candidate}/prime-main/lib/itp/cpp/source/itp/root.cpp")
            set(ARKAIM_INFRASTRUCTURE_DIR "${_candidate}" CACHE PATH "Path to forstandalone/+infrastructure used by terminal" FORCE)
            break()
        endif()
    endforeach()
endif()

if(NOT EXISTS "${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cpp/source/itp/root.cpp")
    message(FATAL_ERROR "Arkaim terminal infrastructure not found. Set ARKAIM_INFRASTRUCTURE_DIR to forstandalone/+infrastructure")
endif()

if(WIN32)
    set(ARKAIM_PLATFORM_SUFFIX winapi)
else()
    set(ARKAIM_PLATFORM_SUFFIX posix)
endif()

find_package(Boost REQUIRED COMPONENTS system thread log log_setup)
find_package(Threads REQUIRED)
find_package(OpenSSL)
find_package(Poco REQUIRED COMPONENTS Net Crypto Util Foundation)


include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/include)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cpp/include)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cl2/include)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/cpp/include)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/include)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/include_1)
include_directories(${ARKAIM_INFRASTRUCTURE_DIR}/include_2)

set(ARKAIM_INFRA_SOURCES
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/source/initp/platform/named_pipe_${ARKAIM_PLATFORM_SUFFIX}.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/source/initp/system/time_${ARKAIM_PLATFORM_SUFFIX}.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/source/initp/tools/decimal.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/source/initp/tools/format.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/source/initp/tools/number.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/c/source/initp/tools/crc.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/cpp/source/initp/platform/named_pipe.cpp
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/util/cpp/source/initp/platform/socket.cpp
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/array.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/bus.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/command.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/endpoint.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/error.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/frame.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/handler.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/list.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/node.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/package.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/protocol.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/queue.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/root.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/stack_trace.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/transmit.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/c/source/itp/tunnel.c
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cl2/source/itp/entity.cpp
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cl2/source/itp/tools.cpp
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cpp/source/itp/callback.cpp
    ${ARKAIM_INFRASTRUCTURE_DIR}/prime-main/lib/itp/cpp/source/itp/root.cpp
)

configure_file(
        ${CMAKE_SOURCE_DIR}/include/bridge/config.json
        ${CMAKE_BINARY_DIR}/config.json
        COPYONLY
)

add_executable(dwin_exmpl src/main.cpp
        include/bridge/ITransport.hpp
        include/bridge/SerialTransport.hpp
        include/bridge/types.hpp
        include/bridge/MessageLayer.hpp
        src/MessageLayer.cpp
        src/SerialTransport.cpp
        src/DwinParser.cpp
        include/bridge/HttpTransport.hpp
        include/bridge/IMessageParser.hpp
        include/bridge/ILogicHandler.hpp
        include/bridge/DwinLogic.hpp
        src/DwinLogic.cpp
        include/bridge/DwinParser.hpp
        src/utility.cpp
        include/bridge/utility.hpp
        src/HttpTransport.cpp
        include/bridge/HttpParser.hpp
        include/bridge/HttpLogic.hpp
        src/HttpParser.cpp
        src/HttpLogic.cpp
        include/bridge/config.json
        include/bridge/Settings.hpp
        include/bridge/constant.hpp
        src/constant.cpp
        include/bridge/DwinCommands.hpp
        include/bridge/PrimeCommands.hpp
         include/bridge/Logger.hpp
         src/Logger.cpp
         include/bridge/PaymentData.hpp
        src/debug_print.cpp
        src/ArkaimTransport.cpp
        src/ArkaimParser.cpp
        src/ArkaimLogic.cpp
        src/Printer.cpp
        include/bridge/ArkaimTransport.hpp
        include/bridge/ArkaimParser.hpp
        include/bridge/ArkaimLogic.hpp
        include/bridge/Printer.hpp
        ${ARKAIM_INFRA_SOURCES}
)

if(WIN32)
    target_compile_definitions(dwin_exmpl PRIVATE
        BOOST_LOG_DYN_LINK ITP_COMPILE_CXX ITP_EXTEND_CALLBACKS WIN32
        ITP_DEBUG_CUSTOM SYSTEM_DEBUG_CUSTOM
        ITP_REVISION=5
        ITP_WAIT_SENDING=10000
        ITP_WAIT_RECEIVING=10000
        ITP_WAIT_ACKNOWLEDGE=10000
        ITP_WAIT_CONTROL=30000
        ITP_WAIT_PROXY=120000
        ITP_WAIT_DATA=120000
        ITP_MAX_BUFFER_SIZE=0xFFFF
        ITP_ENABLE_THREAD_SAFETY
    )
else()
    target_compile_definitions(dwin_exmpl PRIVATE
        BOOST_LOG_DYN_LINK ITP_COMPILE_CXX ITP_EXTEND_CALLBACKS POSIX
        ITP_DEBUG_CUSTOM SYSTEM_DEBUG_CUSTOM
        ITP_REVISION=5
        ITP_WAIT_SENDING=10000
        ITP_WAIT_RECEIVING=10000
        ITP_WAIT_ACKNOWLEDGE=10000
        ITP_WAIT_CONTROL=30000
        ITP_WAIT_PROXY=120000
        ITP_WAIT_DATA=120000
        ITP_MAX_BUFFER_SIZE=0xFFFF
        ITP_ENABLE_THREAD_SAFETY
    )
endif()

target_link_libraries(dwin_exmpl
        ${Boost_LIBRARIES}
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        Poco::Net
        Poco::Crypto
        Poco::Util
        Poco::Foundation
)

