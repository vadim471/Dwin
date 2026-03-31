#ifndef INITPLUS_INTERFACE_CANCELLABLE_HPP_INCLUDED
#define INITPLUS_INTERFACE_CANCELLABLE_HPP_INCLUDED

namespace initp {
namespace tools {

class cancellable {
public: // Public methods
    virtual void accept(void) = 0;
    virtual void cancel(void) = 0;
    virtual bool changed(void) const = 0;
};

}}

#endif // INITPLUS_INTERFACE_CANCELLABLE_HPP_INCLUDED
