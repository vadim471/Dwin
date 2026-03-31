#include <algorithm>
#include <iostream>
#include <fstream>
#include <csignal>
#include <thread>

#include <initp/socket/raw/connection.hpp>
#include <initp/socket/raw/basic_connection.hpp>
#include <initp/socket/raw/ssl_connection.hpp>

#include <initp/log/vector/event.pb.h>

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/json_util.h>

bool started = false;

typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_work;

boost::asio::io_context ioc;
boost::asio::ssl::context ssl(boost::asio::ssl::context::sslv23);
initp::socket::raw::connection::ptr con;
std::unique_ptr<io_context_work> lock;

void on_signal(int signal) {
    std::cout << "Dummy received signal " << signal << std::endl;
    ioc.stop();
    started = false;
}

void on_write(initp::system::error_code ec) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    } else {
        std::cout << "OK" << std::endl;
    }
    con->close();
}

void on_connect(initp::system::error_code ec) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    } else {
        std::cout << "Connected" << std::endl;
    }

    /*{
        std::ifstream file("0326.pb", std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size + 4);
        *((int32_t*)buffer.data()) = size;
        std::reverse(buffer.data(), buffer.data() + 4);
        if (!file.read((char*)(buffer.data() + 4), size)) {
            std::cout << "Failed to read file" << std::endl;
            return;
        }
        con->write(buffer, &on_write);
        return;
    }*/

    event::EventArray root;
    event::LogArray* logs = root.mutable_logs();
    event::Log* log = logs->add_logs();
    event::Value* value = log->mutable_value();
    event::ValueMap* content = value->mutable_map();
    google::protobuf::Map<std::string, event::Value>* map = content->mutable_fields();
    {
        event::Value value;
        value.set_raw_bytes("dd010203");
        (*map)["id"] = value;
    }
    {
        event::Value value;
        value.set_raw_bytes("prime");
        (*map)["device"] = value;
    }
    {
        event::Value value;
        value.set_raw_bytes("logs");
        (*map)["type"] = value;
    }
    {
        event::Value value;
        google::protobuf::Timestamp* time = value.mutable_timestamp();
        *time = google::protobuf::util::TimeUtil::GetCurrentTime();
        time->set_nanos(0);
        (*map)["timestamp"] = value;
    }
    {
        event::Value value;
        value.set_raw_bytes("some message");
        (*map)["message"] = value;
    }

    log = logs->add_logs();
    value = log->mutable_value();
    content = value->mutable_map();
    map = content->mutable_fields();
    {
        event::Value value;
        value.set_raw_bytes("dd010203");
        (*map)["id"] = value;
    }
    {
        event::Value value;
        value.set_raw_bytes("prime");
        (*map)["device"] = value;
    }
    {
        event::Value value;
        value.set_raw_bytes("sales");
        (*map)["type"] = value;
    }
    {
        event::Value value;
        google::protobuf::Timestamp* time = value.mutable_timestamp();
        *time = google::protobuf::util::TimeUtil::GetCurrentTime();
        time->set_nanos(0);
        (*map)["timestamp"] = value;
    }
    {
        event::Value value;
        value.set_raw_bytes("some sales data");
        (*map)["data"] = value;
    }

    size_t size = root.ByteSizeLong();
    std::vector<uint8_t> data(size + 4);
    root.SerializeToArray(data.data() + 4, size);
    *((int32_t*)data.data()) = size;
    std::reverse(data.data(), data.data() + 4);

    con->write(data, &on_write);
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);
    //signal(SIGBREAK, &on_signal);

    /*std::ifstream file("0326.pb", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cout << "Faield to read file" << std::endl;
        return 1;
    }
    event::EventArray root;
    root.ParseFromArray(&buffer[0], size);
    std::string json_string;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    MessageToJsonString(root, &json_string, options);*/
    //std::cout << json_string << std::endl;
    //return 0;

    initp::socket::raw::ssl_connection::initialize_ssl(ssl, std::string(), std::string(), std::string());
    bool use_ssl = false;
    if (use_ssl) {
        con = std::make_shared<initp::socket::raw::ssl_connection>(ioc, ssl);
    } else {
        con = std::make_shared<initp::socket::raw::basic_connection>(ioc);
    }
    lock.reset(new io_context_work(ioc.get_executor()));
    std::thread thread([] (void) -> void { ioc.run(); });

    con->connect("10.9.1.213", "9876", &on_connect);

    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    thread.join();
    std::cout << "Terminated" << std::endl;
    return 0;
}
