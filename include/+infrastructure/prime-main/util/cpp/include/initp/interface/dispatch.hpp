#ifndef INITPLUS_INTERFACE_DISPATCH_HPP_INCLUDED
#define INITPLUS_INTERFACE_DISPATCH_HPP_INCLUDED

#include <memory>
#include <functional>

namespace initp {
namespace tools {

class dispatcher {
    typedef dispatcher self_type;
public:
    typedef std::shared_ptr<self_type> ptr;
    typedef std::weak_ptr<self_type> wptr;
    virtual ~dispatcher(void) {}
public:
    /**
     * \brief Выполнить функцию в потоке интерфейса.
     * \param[in] f указатель на функцию.
     */
    virtual void dispatch(const std::function<void(void)>&) = 0;
};

class multithreaded {
    typedef multithreaded self_type;
public:
    typedef std::shared_ptr<self_type> ptr;
    typedef std::weak_ptr<self_type> wptr;
    virtual ~multithreaded(void) {}
public:
    /**
     * \brief Выполнить функцию в одном из потоков логики.
     * \param[in] f указатель на функцию.
     */
    virtual void post(const std::function<void(void)>&) = 0;
};

}}

#endif // INITPLUS_INTERFACE_DISPATCH_HPP_INCLUDED
