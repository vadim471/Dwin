#ifndef INITPLUS_SOCKET_HPP_INCLUDED
#define INITPLUS_SOCKET_HPP_INCLUDED

#include <initp/system/error_code.hpp>
#include <initp/system/time.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <cstdint>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>

namespace initp {
namespace system {

class socket {
private:
    typedef socket self_type;
public:
    typedef std::unique_ptr<self_type> uptr;

public: // Public classes

    class buffer {
        friend class socket;
        public:
            buffer(void);
            buffer(const buffer&) = delete;
            ~buffer(void) = default;
        public:
            size_t max(void) const;
            size_t size(void) const;
            bool empty(void) const;
            void clear(void);
            void read(size_t count);
            void write(size_t count);
            uint8_t* data(void);
        private:
            uint8_t data_[0x100];
            size_t begin_;
            size_t end_;
    };

    class service:
        public std::enable_shared_from_this<service> {
    private:
        friend class initp::system::socket;
        typedef service self_type;
        typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_type;
    public:
        typedef std::shared_ptr<self_type> ptr;
    public:
        service(void);
        service(const self_type&) = delete;
        virtual ~service(void);
    public:
        void run(void);
        void cancel(void);
        /**
         * \brief Прослушивать указанный порт и принимать все соединения.
         * \param[in] port Прослушиваемый порт.
         * \param[in] listener Функция обратного вызова при удачном подключении.
         */
        template<typename socket_type = initp::system::socket>
        void listen(uint16_t port, const std::function<void(std::unique_ptr<socket_type>&&)>& listener);
    protected:
        void listen(uint16_t port, socket* child);
        boost::asio::io_context& context(void);
    private:
        template<typename socket_type = initp::system::socket>
        void on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket object, const std::function<void(std::unique_ptr<socket_type>&&)>& listener);
        void on_single_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket object, socket* child);
    private:
        boost::asio::io_context context_;
        std::unique_ptr<std::thread> thread_;
        std::unique_ptr<work_guard_type> lock_;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
        std::vector<socket*> children_;
        std::recursive_mutex mutex_;
        bool pending_accept_;
    };
    friend class service;

public: // Construction
    socket(void);
    socket(const self_type&) = delete;
    virtual ~socket(void);
protected:
    socket(service::ptr);

public: // Public methods

    /**
     * \brief Прослушивать указанный порт и принять соединение.
     * \param[in] port Прослушиваемый порт.
     */
    void listen(uint16_t port);

    /**
     * \brief Подключиться к удалённому хосту.
     * \param[in] host Имя хоста.
     * \param[in] port Порт подключения.
     */
    void connect(const std::string& host, uint16_t port);

    /**
     * \brief Подключён ли сокет.
     * \return Если подключён - true, иначе false.
     */
    bool connected(void) const;

    /**
     * \brief Установить время повтора соединения.
     */
    void reconnect_delay(initp::system::time_t);

    /**
     * \brief IP адрес удалённого хоста.
     */
    const boost::asio::ip::address& remote_ip(void) const;

    /**
     * \brief Установить обработчик для события подключения.
     */
    void set_on_connect(const std::function<void(void)>&);

    /**
     * \brief Установить обработчик для события переподключения.
     */
    void set_on_reconnect(const std::function<void(void)>&);

    /**
     * \brief Установить обработчик для события отключения.
     */
    void set_on_disconnect(const std::function<void(void)>&);

public: // Read/write
    virtual size_t read(char* data, size_t size);
    virtual size_t write(const char* data, size_t length);

private: // Private methods
    void do_resolve(void);
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void do_reconnect(void);
    void do_read(void);
    void do_write(void);
    void on_resolve(boost::system::error_code ec, const boost::asio::ip::tcp::resolver::results_type& endpoints);
    void on_connect(boost::system::error_code ec);
    void on_read(boost::system::error_code ec, size_t length);
    void on_write(boost::system::error_code ec, size_t length);

private: // Private fields
    service::ptr service_;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
    std::function<void(void)> on_connect_;
    std::function<void(void)> on_reconnect_;
    std::function<void(void)> on_disconnect_;
    boost::asio::ip::address remote_ip_;
    std::string host_;
    uint16_t port_;
    bool pending_connect_;
    bool pending_read_;
    bool pending_write_;
    bool was_connected_;
    bool connected_;
    bool client_;
    buffer read_buffer_;
    buffer write_buffer_;
    initp::system::time_t last_connect_;
    initp::system::time_t reconnect_delay_;
};

}}

#endif // INITPLUS_SOCKET_HPP_INCLUDED
