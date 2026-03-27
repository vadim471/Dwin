#include <itp/itp.hpp>
#include <itp/error.hpp>
#include <itp/tunnel.hpp>

extern "C" {

#include <initp/system/time.h>

}

#include <initp/system/time.hpp>
#include <initp/platform/serial_port.hpp>
#include <initp/utils/macro.hpp>

#include <iostream>
#include <memory>
#include <functional>

auto t01(itp::tunnel::join());
auto t02(itp::tunnel::join());
auto t13(itp::tunnel::join());
auto t14(itp::tunnel::join());
auto t45(itp::tunnel::join());
auto t06(itp::tunnel::join());
auto t67(itp::tunnel::join());

class serial_port:
    public itp::endpoint,
    public initp::system::serial_port {
    typedef serial_port self_type;
public:
    using initp::system::serial_port::open;
    using initp::system::serial_port::close;
    typedef std::unique_ptr<self_type> uptr;
public:
    serial_port(void):
        itp::endpoint(),
        initp::system::serial_port()
    {}
public:
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        return (itp_size_t)initp::system::serial_port::read((char*)data, (size_t)size);
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        return (itp_size_t)initp::system::serial_port::write((const char*)data, length);
    }
};

void on_error(itp::root&, uint8_t address, itp_error_code_t error) {
    std::cout << "Node " << (uint32_t)address << " throws error " << error << std::endl;
}

void on_connect(itp::root&, uint8_t address, uint16_t error) {
    if (error) {
        std::cout << "Failed to connect node " << (uint32_t)address << ", error " << error << " - " << itp::get_error_description(error) << std::endl;
    } else std::cout << "Node with address " << (uint32_t)address << " connected" << std::endl;
}

std::tuple<itp::endpoint::uptr, uint16_t> on_create_1(const std::string& path) {
    switch (path.front()) {
        case '3':
            if (!std::get<0>(t13)) {
                return std::make_tuple(nullptr, ITP_ERR_NULL_POINTER);
            } else return std::make_tuple(std::move(std::get<0>(t13)), ITP_ERR_NONE);
        case '4':
            if (!std::get<0>(t14)) {
                return std::make_tuple(nullptr, ITP_ERR_NULL_POINTER);
            } else return std::make_tuple(std::move(std::get<0>(t14)), ITP_ERR_NONE);
        default:
            return std::make_tuple(nullptr, ITP_ERR_WRONG_PATH);
    }
}

std::tuple<itp::endpoint::uptr, uint16_t> on_create_4(const std::string& path) {
    switch (path.front()) {
        case '5':
            if (!std::get<0>(t45)) {
                return std::make_tuple(nullptr, ITP_ERR_NULL_POINTER);
            } else return std::make_tuple(std::move(std::get<0>(t45)), ITP_ERR_NONE);
        default:
            return std::make_tuple(nullptr, ITP_ERR_WRONG_PATH);
    }
}

std::tuple<itp::endpoint::uptr, uint16_t> on_create_7(const std::string& path) {
    serial_port::uptr port = serial_port::uptr(new serial_port());
    if (!port || !port->open(path.c_str(), 9600, 8, 1, 0))
        return std::make_tuple(itp::endpoint::uptr(), uint16_t(ITP_ERR_MALLOC_FAILED));
    return std::make_tuple(std::move(port), uint16_t(ITP_ERR_NONE));
}

uint16_t on_request_0101(itp::root& root, itp::frame& frame) {
    uint16_t value;
    std::tie(value, std::ignore) = frame.read_value<uint16_t>();
    std::cout << "Node " << (uint32_t)root.address() << " received value " << value << " from " << (uint32_t)frame.from() << std::endl;
    value *= 2;
    itp::frame::uptr response(new itp::frame(0x0101));
    response->write_value(value);
    {
        uint16_t a1[] = { 1, 2, 3 };
        std::vector<uint32_t> a2 = { 4, 5, 6 };
        std::string s("It Works!");
        response->write_array(a1, sizeof(a1) / sizeof(uint16_t));
        response->write_array(a2);
        response->write_string(s);
    }
    root.push_response(frame, std::move(response), nullptr);
    return ITP_ERR_NONE;
}

uint16_t on_request_0102(itp::root& root, itp::frame& frame) {
    std::cout << "Node " << (uint32_t)root.address() << " received 0x" << std::hex << frame.command() << std::dec << " from " << (int)frame.from() << std::endl;
    return ITP_ERR_BAD_DATA;
}

template<typename CharT, typename TraitsT, typename ValueT>
inline std::basic_ostream<CharT, TraitsT>& operator<<(
    std::basic_ostream<CharT, TraitsT>& stream,
    const std::vector<ValueT> vector
) {
    stream << "{";
    for (ValueT v : vector)
        stream << " " << v;
    stream << " }";
    return stream;
}

void on_response_0101(itp::root&, uint16_t error, itp::frame& frame) {
    std::cout << "Received result " << error << " from node " << (int)frame.from() << std::endl;
    if (!error) {
        uint16_t value;
        std::tie(value, std::ignore) = frame.read_value<uint16_t>();
        std::cout << "Received value: " << value << std::endl;
        {
            std::vector<uint16_t> a1;
            std::vector<uint32_t> a2;
            std::string s;
            std::tie(a1, std::ignore) = frame.read_array<uint16_t>();
            std::tie(a2, std::ignore) = frame.read_array<uint32_t>();
            std::tie(s, std::ignore) = frame.read_string();
            std::cout << "a1: " << a1 << "; a2: " << a2 << "; str: " << s << std::endl;
        }
    }
}

void on_response_0102(itp::root&, uint16_t error, itp::frame& frame) {
    std::cout << "Received result " << error << " from node " << (int)frame.from() << std::endl;
}

void on_receive_data(itp::root&, itp_byte_t* data, itp_size_t size) {
    std::cout << "Was received data:" << std::hex;
    for (itp_size_t i = 0; i < size; ++i)
        std::cout << " 0x" << (int)data[i];
    std::cout << std::dec << std::endl;
}

void on_transmit_data(itp::root&, uint16_t error) {
    std::cout << "Data was sent with result " << error << std::endl;
}

void on_trace_route(itp::root&, uint16_t error, itp::frame& frame) {
    std::cout << "Trace route result: " << error << std::endl;
    if (!error) {
        std::vector<uint8_t> data;
        std::tie(data, std::ignore) = frame.read_array<uint8_t>();
        std::cout << "Route: ";
        for each_c(std::vector<uint8_t>, it, data) {
            if (it != data.cbegin())
                std::cout << ", ";
            std::cout << (uint32_t)IT;
        }
        std::cout << " (" << data.size() << ")" << std::endl;
    }
}

void on_connect_request(itp::root&, uint16_t error) {
    std::cout << "Connect request result: " << error << std::endl;
}

itp::remote_endpoint::uptr remote = nullptr;

void on_connect_endpoint(itp::root&, uint8_t address, uint16_t error, itp::remote_endpoint::uptr endpoint) {
    if (error) {
        std::cout << "Failed to connect endpoint " << (int)address << ", error " << error << " - " << itp::get_error_description(error) << std::endl;
    } else std::cout << "Endpoint with address " << (int)address << " connected" << std::endl;
    if (endpoint) {
        std::cout << "It Works!" << std::endl;
        remote = std::move(endpoint);
    }
}

int main(int argc, char** argv) {
    using namespace std::placeholders;
    {
        itp_time_t time;
        itp::root::ptr n0 = std::make_shared<itp::root>();
        itp::root::ptr n1 = std::make_shared<itp::root>();
        itp::root::ptr n2 = std::make_shared<itp::root>();
        itp::root::ptr n3 = std::make_shared<itp::root>();
        itp::root::ptr n4 = std::make_shared<itp::root>();
        itp::root::ptr n5 = std::make_shared<itp::root>();
        itp::root::ptr n6 = std::make_shared<itp::root>();
        itp::root::ptr n7 = std::make_shared<itp::root>();
        n1->set_parent(std::move(std::get<1>(t01)), &on_error);
        n2->set_parent(std::move(std::get<1>(t02)), &on_error);
        n3->set_parent(std::move(std::get<1>(t13)), &on_error);
        n4->set_parent(std::move(std::get<1>(t14)), &on_error);
        n5->set_parent(std::move(std::get<1>(t45)), &on_error);
        n6->set_parent(std::move(std::get<1>(t06)), &on_error);
        n7->set_parent(std::move(std::get<1>(t67)), &on_error);
        n1->on_create(&on_create_1);
        n4->on_create(&on_create_4);
        //
        for (uint32_t i = 0; i < 1; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            n6->poll(time);
            n7->poll(time);
            sys_sleep_for(3);
        }
        //
        std::cout << "Initialization done." << std::endl;
        //goto FREE_MEMORY;
        //
        time = sys_clock_ms();
        uint8_t a_n0 = 0xFF;
        n0->address(a_n0);
        uint8_t a_n1 = std::get<0>(n0->get_free_address());
        n0->connect_local_node(time, std::move(std::get<0>(t01)), a_n1, &on_connect);
        uint8_t a_n2 = std::get<0>(n0->get_free_address());
        n0->connect_local_node(time, std::move(std::get<0>(t02)), a_n2, &on_connect);
        //
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            sys_sleep_for(3);
        }
        //
        std::cout << "Stage 1 done." << std::endl;
        //goto FREE_MEMORY;
        //
        time = sys_clock_ms();
        uint8_t a_n3 = std::get<0>(n0->get_free_address());
        n0->connect_remote_node(time, a_n1, a_n3, "3", &on_connect);
        uint8_t a_n4 = std::get<0>(n0->get_free_address());
        n0->connect_remote_node(time, a_n1, a_n4, "4", &on_connect);
        //
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            sys_sleep_for(3);
        }
        //
        printf("Stage 2 done.\r\n");
        //goto FREE_MEMORY;
        //
        time = sys_clock_ms();
        uint8_t a_n5 = std::get<0>(n0->get_free_address());
        n0->connect_remote_node(time, a_n4, a_n5, "5", &on_connect);
        //
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 3 done.\r\n");
        //goto FREE_MEMORY;
        //
        n5->register_handler(a_n0, 0x0101, &on_request_0101);
        {
            uint16_t value = 111;
            itp::frame::uptr request(new itp::frame(0x0101));
            request->write_value(value);
            n0->push_request(std::move(request), a_n5, &on_response_0101);
        }
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 4 done.\r\n");
        //goto FREE_MEMORY;
        //
        n5->trace_route(2, &on_trace_route);
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 5 done.\r\n");
        //goto FREE_MEMORY;
        //
        n1->register_handler(a_n0, 0x0102, &on_request_0102);
        {
            itp::frame::uptr request(new itp::frame(0x0102));
            n0->push_request(std::move(request), a_n1, &on_response_0102);
        }
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 6 done.\r\n");
        //goto FREE_MEMORY;
        //
        n0->set_connection_listener(std::move(std::get<0>(t06)));
        n6->request_connect(time, &on_connect_request);
        for (uint32_t i = 0; i < 30; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            n6->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 7 done.\r\n");
        //goto FREE_MEMORY;
        //
        n6->set_connection_listener(std::move(std::get<0>(t67)));
        n7->request_connect(time, &on_connect_request);
        for (uint32_t i = 0; i < 40; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            n6->poll(time);
            n7->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 8 done.\r\n");
        //goto FREE_MEMORY;
        //
        n1->trace_route(7, &on_trace_route);
        for (uint32_t i = 0; i < 40; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            n6->poll(time);
            n7->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 9 done.\r\n");
        //goto FREE_MEMORY;
        //
        n7->on_create(&on_create_7);
        n0->connect_remote_endpoint(time, 7, "COM6", &on_connect_endpoint);
        for (uint32_t i = 0; i < 40; i++) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            n6->poll(time);
            n7->poll(time);
            sys_sleep_for(3);
        }
        printf("Stage 10 done.\r\n");
        //goto FREE_MEMORY;
        //
        for (;remote;) {
            time = sys_clock_ms();
            n0->poll(time);
            n1->poll(time);
            n2->poll(time);
            n3->poll(time);
            n4->poll(time);
            n5->poll(time);
            n6->poll(time);
            n7->poll(time);
            {
                itp_byte_t data[0x100];
                itp_size_t bytes = remote->read(data, 0x100);
                if (bytes > 0) {
                    remote->write(data, bytes);
                }
            }
            sys_sleep_for(3);
        }
        goto FREE_MEMORY;
    }
    FREE_MEMORY:
    if (remote) remote.reset();
    std::cout << "Free memory done." << std::endl;
    return 0;
}
