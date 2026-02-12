cmake_minimum_required(VERSION 3.5)
project(dwin_exmpl)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Threads REQUIRED)
find_package(OpenSSL)
find_package(Poco REQUIRED COMPONENTS Net Crypto Util Foundation)


include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

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
        include/bridge/PipeTransport.hpp
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
)

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



