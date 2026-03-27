#ifndef INITPLUS_NAMED_PIPE_HPP_INCLUDED
#define INITPLUS_NAMED_PIPE_HPP_INCLUDED

extern "C" {

#include <initp/platform/named_pipe.h>

}

#include <functional>
#include <string>

namespace initp {
namespace system {

class named_pipe {
private:
    typedef named_pipe self_type;
public:
    typedef std::function<void(void)> on_connect_f;
public: // Construction
    named_pipe(const char* path);
    named_pipe(const std::string& path);
    named_pipe(const self_type&) = delete;
    virtual ~named_pipe(void);
public: // Public methods
    virtual bool create(void);
    virtual bool open(void);
    virtual void close(void);
public: // Endpoint methods
    virtual size_t read(char* data, size_t size);
    virtual size_t write(const char* data, size_t length);
private: // Private methods
    static void call_on_connect(size_t eparam);
public: // Public properties
    sys_named_pipe_tp c_object(void);
    bool opened(void) const;
    void on_connect(on_connect_f handler);
    void on_disconnect(on_connect_f handler);
protected: // Private fields
    sys_named_pipe_t c_object_;
    on_connect_f on_connect_;
    on_connect_f on_disconnect_;
    bool opened_;
};

}}

#endif // INITPLUS_NAMED_PIPE_HPP_INCLUDED
