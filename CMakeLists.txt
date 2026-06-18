cmake_minimum_required(VERSION 3.5)
project(dwin_exmpl)

set(CMAKE_CXX_STANDARD 14)

# ============================================
# Auto-increment build number
# ============================================
if(EXISTS "${CMAKE_SOURCE_DIR}/config/build_number.txt")
    file(READ "${CMAKE_SOURCE_DIR}/config/build_number.txt" BUILD_NUMBER)
    string(STRIP "${BUILD_NUMBER}" BUILD_NUMBER)
else()
    set(BUILD_NUMBER 26)
endif()

math(EXPR BUILD_NUMBER "${BUILD_NUMBER} + 1")

file(WRITE "${CMAKE_SOURCE_DIR}/config/build_number.txt" "${BUILD_NUMBER}")

set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

configure_file(
    "${CMAKE_SOURCE_DIR}/config/version.hpp.in"
    "${CMAKE_SOURCE_DIR}/include/bridge/version.hpp"
    @ONLY
)

message(STATUS "===========================================")
message(STATUS "Build number: ${BUILD_NUMBER}")
message(STATUS "Version: ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${BUILD_NUMBER}")
message(STATUS "===========================================")

# ============================================
# End of version configuration
# ============================================

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
find_package(OpenSSL REQUIRED)
find_package(Poco REQUIRED COMPONENTS Net NetSSL Crypto Util Foundation)
find_package(SQLite3 REQUIRED)
find_package(Iconv REQUIRED)


include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SQLite3_INCLUDE_DIRS})
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
        include/bridge/httpAuth/IHttpAuthenticator.hpp
        include/bridge/httpAuth/DigestAuthenticator.hpp
        include/bridge/httpAuth/BasicAuthenticator.hpp
        src/httpAuth/BasicAuthenticator.cpp
        src/httpAuth/DigestAuthenticator.cpp
        src/parser/BosParser.cpp
        include/bridge/parser/BosParser.hpp
        include/bridge/parser/HttpParser.hpp
        src/parser/HttpParser.cpp
        include/bridge/logic/BosLogic.hpp
        src/logic/BosLogic.cpp
        include/bridge/bos/SalesData.hpp
        include/bridge/bos/FuelLevelData.hpp
        src/database/FuelReceptionRepository.cpp
        src/database/LevelGaugeRepository.cpp
        include/bridge/payment/CardData.hpp
)

configure_file(
        ${CMAKE_SOURCE_DIR}/config/config.json
        ${CMAKE_BINARY_DIR}/config.json
        COPYONLY
)

add_executable(dwin_exmpl src/main.cpp
        # Core
        include/bridge/core/types.hpp
        include/bridge/core/MessageLayer.hpp
        include/bridge/core/constant.hpp
        include/bridge/core/Settings.hpp
        include/bridge/core/Logger.hpp
        include/bridge/core/utility.hpp
        src/core/MessageLayer.cpp
        src/core/constant.cpp
        src/core/Logger.cpp
        src/core/utility.cpp
        
        # Transport
        include/bridge/transport/ITransport.hpp
        include/bridge/transport/SerialTransport.hpp
        include/bridge/transport/HttpTransport.hpp
        include/bridge/transport/ArkaimTransport.hpp
        src/transport/SerialTransport.cpp
        src/transport/HttpTransport.cpp
        src/transport/ArkaimTransport.cpp
        
        # Parser
        include/bridge/parser/IMessageParser.hpp
        include/bridge/parser/DwinParser.hpp
        include/bridge/parser/PrimeParser.hpp
        include/bridge/parser/ArkaimParser.hpp
        src/parser/DwinParser.cpp
        src/parser/PrimeParser.cpp
        src/parser/ArkaimParser.cpp

        #Auth
        include/bridge/httpAuth/BasicAuthenticator.hpp
        include/bridge/httpAuth/DigestAuthenticator.hpp
        include/bridge/httpAuth/IHttpAuthenticator.hpp
        src/httpAuth/DigestAuthenticator.cpp
        src/httpAuth/BasicAuthenticator.cpp
        
        # Logic
        include/bridge/logic/ILogicHandler.hpp
        include/bridge/logic/DwinLogic.hpp
        include/bridge/logic/PrimeLogic.hpp
        include/bridge/logic/ArkaimLogic.hpp
        src/logic/DwinLogic.cpp
        src/logic/PrimeLogic.cpp
        src/logic/ArkaimLogic.cpp
        
        # Database
        include/bridge/database/Database.hpp
        include/bridge/database/Transaction.hpp
        include/bridge/database/BosRepository.hpp
        src/database/Database.cpp
        src/database/Transaction.cpp
        src/database/BosRepository.cpp
        
        # DWIN
        include/bridge/dwin/DwinCommands.hpp
        include/bridge/dwin/PrimeCommands.hpp
        
        # Payment
        include/bridge/payment/PaymentData.hpp
        include/bridge/payment/Printer.hpp
        src/payment/Printer.cpp
        
        # Other
        src/debug_print.cpp
        include/bridge/json.hpp
        
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
        Iconv::Iconv
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        Poco::Net
        Poco::NetSSL
        Poco::Crypto
        Poco::Util
        Poco::Foundation
        SQLite::SQLite3
)

