#ifndef ITP_CL2_ENTITY_HPP_INCLUDED
#define ITP_CL2_ENTITY_HPP_INCLUDED

#include "command.h"

#include <itp/itp.hpp>
#include <itp/cl2.h>

#include <initp/system/time.hpp>

#include <cstdint>
#include <functional>
#include <memory>

namespace itp {

class entity {

private:
    typedef entity self_type;

public:
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;
    typedef std::weak_ptr<self_type> wptr;

public: // Static methods
    static const char* get_device_status_description(uint8_t status);

public: // Construction
    entity(uint32_t api, uint8_t type);
    entity(const self_type&) = delete;
    virtual ~entity(void) = default;

public: // Public methods
    itp::endpoint::uptr make_tunnel(void);
    void open_pipe(const std::string& name);
    void listen_port(uint16_t port);
    virtual void poll(initp::system::time_t time);

public: // Pure virtual methods
    virtual uint32_t status(void) const = 0;

protected: // Request handlers
    virtual uint16_t on_get_type(itp::root& root, itp::frame& request);
    virtual uint16_t on_get_status(itp::root& root, itp::frame& request);
    virtual void status_changed(void);

public: // Public properties
    itp::root& node(void);
    virtual uint32_t api(void) const;
    virtual uint8_t type(void) const;

protected:
    uint32_t api_;
    uint8_t type_;
    itp::root node_;
};

}

#endif // ITP_CL2_ENTITY_HPP_INCLUDED
