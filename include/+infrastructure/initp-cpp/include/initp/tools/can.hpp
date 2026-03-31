#ifndef INITPLUS_TOOLS_CAN_HPP_INCLUDED
#define INITPLUS_TOOLS_CAN_HPP_INCLUDED

#include "bind.hpp"
#include "cancellable.hpp"

namespace initp {
namespace tools {

template<typename T>
class can:
    public bind<T>,
    public cancellable {
public: // Construction
    can(void):
        bind<T>() {
        //MEMORY_("shared::tools::can<T>(void);");
    }
    can(const T& v):
        bind<T>(v),
        previous_(v) {
        //MEMORY_("shared::tools::can<T>(const T&);");
    }
    can(const can& v):
        bindable(v),
        bind<T>(v.value_),
        previous_(v.previous_) {
        //MEMORY_("shared::tools::can<T>(const can&);");
    }
    virtual ~can(void) {
        //MEMORY_("shared::tools::~can<T>(void);");
    }
public: // Operators
    constexpr operator const T&() const {
        return this->value_;
    }
    using bind<T>::operator!;
    using bind<T>::operator=;
    using bind<T>::operator*;
    using bind<T>::operator->;
    using bind<T>::operator==;
    using bind<T>::operator!=;
    using bind<T>::operator>=;
    using bind<T>::operator<=;
    using bind<T>::operator>;
    using bind<T>::operator<;
    using bind<T>::operator++;
    using bind<T>::operator--;
    using bind<T>::operator+=;
    using bind<T>::operator-=;
    using bind<T>::operator*=;
    using bind<T>::operator/=;
    using bind<T>::operator%=;
public: // Public methods
    virtual void accept(void) {
        if (this->previous_ != this->value_) {
            this->previous_ = this->value_;
        }
    }
    virtual void cancel(void) {
        if (this->previous_ != this->value_) {
            this->value_ = this->previous_;
            this->value_changed();
        }
    }
public: // Public properties
    virtual bool changed(void) const {
        //TRACE_("Current: ", this->value_, "; Previous: ", this->previous_);
        return this->previous_ != this->value_;
    }
protected: // Private fields
    T previous_;
};

}}

#endif // INITPLUS_TOOLS_CAN_HPP_INCLUDED
