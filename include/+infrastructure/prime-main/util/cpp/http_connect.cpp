#include <iostream>
#include <csignal>
#include <thread>

/*#include <initp/http/string/ssl_connection.hpp>

bool started = false;

typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_work;

boost::asio::io_context ioc;
boost::asio::ssl::context ssl(boost::asio::ssl::context::sslv23_client);
initp::http::string::connection::ptr con;
std::unique_ptr<io_context_work> lock;

void on_signal(int signal) {
    std::cout << "Dummy received signal " << signal << std::endl;
    ioc.stop();
    started = false;
}

void on_get(initp::system::error_code ec, boost::beast::http::status status, const initp::http::string::connection::header_list_t& headers, const std::string& body) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    }
    std::cout << "Status: " << status << std::endl;
    if (status != boost::beast::http::status::ok) return;
    if (auto c = std::dynamic_pointer_cast<initp::http::string::ssl_connection>(con)) {
        c->disconnect(nullptr);
    }
}

void on_connect(initp::system::error_code ec) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    } else {
        std::cout << "Connected" << std::endl;
    }
    initp::http::string::connection::header_list_t headers;
    con->get("/prime/0x111721d2/update.list", headers, &on_get);
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    initp::http::string::ssl_connection::initialize_ssl(ssl, std::string(), std::string(), std::string());
    con = std::make_shared<initp::http::string::ssl_connection>(ioc, ssl);
    lock.reset(new io_context_work(ioc.get_executor()));
    std::thread thread([] (void) -> void { ioc.run(); });

    con->connect("sc-update.init-plus.com", "443", &on_connect);

    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    thread.join();
    std::cout << "Terminated" << std::endl;
    return 0;
}*/

/*#include <initp/http/binary/ssl_connection.hpp>

bool started = false;

typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_work;

boost::asio::io_context ioc;
boost::asio::ssl::context ssl(boost::asio::ssl::context::sslv23_client);
initp::http::binary::connection::ptr con;
std::unique_ptr<io_context_work> lock;

void on_signal(int signal) {
    std::cout << "Dummy received signal " << signal << std::endl;
    ioc.stop();
    started = false;
}

void on_get(initp::system::error_code ec, boost::beast::http::status status, const initp::http::binary::connection::header_list_t& headers, const uint8_t* body, size_t size) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    }
    std::cout << "Status: " << status << std::endl;
    if (status != boost::beast::http::status::ok) return;
    std::cout << "Body size: " << size << std::endl;
    if (auto c = std::dynamic_pointer_cast<initp::http::binary::ssl_connection>(con)) {
        c->disconnect(nullptr);
    }
}

void on_connect(initp::system::error_code ec) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    } else {
        std::cout << "Connected" << std::endl;
    }
    initp::http::binary::connection::header_list_t headers;
    con->get("/prime/0x2b0921d2/firmware.uff", headers, &on_get);
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    initp::http::binary::ssl_connection::initialize_ssl(ssl, std::string(), std::string(), std::string());
    con = std::make_shared<initp::http::binary::ssl_connection>(ioc, ssl);
    lock.reset(new io_context_work(ioc.get_executor()));
    std::thread thread([] (void) -> void { ioc.run(); });

    con->connect("sc-update.init-plus.com", "443", &on_connect);

    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    thread.join();
    std::cout << "Terminated" << std::endl;
    return 0;
}*/

/*#include <initp/http/file/ssl_connection.hpp>

bool started = false;

typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_work;

boost::asio::io_context ioc;
boost::asio::ssl::context ssl(boost::asio::ssl::context::sslv23_client);
initp::http::file::connection::ptr con;
std::unique_ptr<io_context_work> lock;

void on_signal(int signal) {
    std::cout << "Dummy received signal " << signal << std::endl;
    ioc.stop();
    started = false;
}

void on_get(initp::system::error_code ec, boost::beast::http::status status, const initp::http::file::connection::header_list_t& headers, size_t size) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    }
    std::cout << "Status: " << status << std::endl;
    if (status != boost::beast::http::status::ok) return;
    std::cout << "Content size: " << size << std::endl;
    if (auto c = std::dynamic_pointer_cast<initp::http::file::ssl_connection>(con)) {
        c->disconnect(nullptr);
    }
}

void on_connect(initp::system::error_code ec) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    } else {
        std::cout << "Connected" << std::endl;
    }
    initp::http::file::connection::header_list_t headers;
    con->get("/prime/0x111721d2/update.list", headers, "test/update.list", &on_get);
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    initp::http::file::ssl_connection::initialize_ssl(ssl, std::string(), std::string(), std::string());
    con = std::make_shared<initp::http::file::ssl_connection>(ioc, ssl);
    lock.reset(new io_context_work(ioc.get_executor()));
    std::thread thread([] (void) -> void { ioc.run(); });

    con->connect("sc-update.init-plus.com", "443", &on_connect);

    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    thread.join();
    std::cout << "Terminated" << std::endl;
    return 0;
}*/

#include <initp/http/ranged/connection.hpp>
#include <initp/http/ranged/basic_connection.hpp>
#include <initp/http/ranged/ssl_connection.hpp>

bool started = false;

typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_work;

boost::asio::io_context ioc;
boost::asio::ssl::context ssl(boost::asio::ssl::context::sslv23);
initp::http::ranged::connection::ptr con;
std::unique_ptr<io_context_work> lock;

void on_signal(int signal) {
    std::cout << "Dummy received signal " << signal << std::endl;
    ioc.stop();
    started = false;
}

void on_progress(size_t complete, size_t total) {
    size_t progress = size_t((double)complete / (double)total * 100.0);
    std::cout << "Progress: " << progress << "%" << std::endl;
}

void on_get(initp::system::error_code ec, boost::beast::http::status status, const initp::http::ranged::connection::header_list_t& headers, size_t size) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    }
    std::cout << "Status: " << status << std::endl;
    if (status != boost::beast::http::status::ok) return;
    std::cout << "Body size: " << size << std::endl;
    if (auto c = std::dynamic_pointer_cast<initp::http::ranged::ssl_connection>(con)) {
        c->disconnect(nullptr);
    }
}

void on_connect(initp::system::error_code ec) {
    if (ec) {
        std::cout << ec.message() << std::endl;
        return;
    } else {
        std::cout << "Connected" << std::endl;
    }
    initp::http::ranged::connection::header_list_t headers;
    //con->get("/prime/0x2b0921d2/firmware.uff", headers, "./firmware.uff", &on_progress, &on_get);
    //con->get("/prime/0x182f29d2/enabled", headers, "./enabled", nullptr, &on_get);
    con->get("/test/windows/enabled", headers, "./enabled", nullptr, &on_get);
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    initp::http::ranged::ssl_connection::initialize_ssl(ssl, std::string(), std::string(), std::string());
    bool use_ssl = true;
    if (use_ssl) {
        con = std::make_shared<initp::http::ranged::ssl_connection>(ioc, ssl, 1024);
    } else {
        con = std::make_shared<initp::http::ranged::basic_connection>(ioc, 1024);
    }
    lock.reset(new io_context_work(ioc.get_executor()));
    std::thread thread([] (void) -> void { ioc.run(); });

    //con->connect("sc-update.init-plus.com", "443", &on_connect);
    con->connect("10.10.1.95", "443", &on_connect);

    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    thread.join();
    std::cout << "Terminated" << std::endl;
    return 0;
}
