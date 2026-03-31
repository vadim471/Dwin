#ifndef INITPLUS_HTTP_SERVER_SERVER_HPP_INCLUDED
#define INITPLUS_HTTP_SERVER_SERVER_HPP_INCLUDED

#include <memory>

namespace initp {
namespace http {

// initp/http/server/config.proto
class server_config;

class server {
public: // Construction
    virtual ~server(void);
public: // Public methods
    virtual bool serve(int threads = 1) = 0;
    virtual void stop(void) = 0;
public: // Factory methods
    static std::unique_ptr<server> create(void);
    static std::unique_ptr<server> create(const server_config&);
};

}}

#endif // INITPLUS_HTTP_SERVER_SERVER_HPP_INCLUDED
