#ifndef INITPLUS_GENERIC_BIND_HPP_INCLUDED
#define INITPLUS_GENERIC_BIND_HPP_INCLUDED

#include <initp/interface/bindable.hpp>

#ifdef SHOW_BIND_CLASS_NAME
#include <initp/generic/enable_class_name.hpp>
#include <initp/system/cdebug>
#endif // SHOW_BIND_CLASS_NAME

#include <iostream>

#define BIND_COMPARE_OPERATOR(OP) \
template<typename A = T> \
bool operator OP(const A& v) const { \
    return this->value_ OP T(v); \
}
#define BIND_POSTFIX_OPERATOR(OP) \
bind& operator OP(int) { \
    this->value_ OP; \
    this->value_changed(); \
    return *this; \
}
#define BIND_PREFIX_OPERATOR(OP) \
bind& operator OP(void) { \
    OP this->value_; \
    this->value_changed(); \
    return *this; \
}
#define BIND_ASSIGN_OPERATOR(OP) \
bind& operator OP(const T& v) { \
    this->value_ OP v; \
    this->value_changed(); \
    return *this; \
}

namespace initp {
namespace tools {

template<typename T>
class bind:
    #ifdef SHOW_BIND_CLASS_NAME
    public virtual enable_class_name<T, true>,
    #endif // SHOW_BIND_CLASS_NAME
    public virtual bindable {
public: // Construction
    bind(void):
        bindable() {
        #ifdef SHOW_BIND_CLASS_NAME
        std::string class_name = this->class_name();
        sys_debug_print(SYSTEM_LEVEL_INFO, "tools::bind<T>(void);", "T = %s", class_name);
        #endif // SHOW_BIND_CLASS_NAME
    }
    bind(const T& v):
        bindable(),
        value_(v) {
        #ifdef SHOW_BIND_CLASS_NAME
        std::string class_name = this->class_name();
        sys_debug_print(SYSTEM_LEVEL_INFO, "tools::bind<T>(const T&);", "T = %s", class_name);
        #endif // SHOW_BIND_CLASS_NAME
    }
    bind(const bind& c):
        bindable(c),
        value_(c.value_) {
        #ifdef SHOW_BIND_CLASS_NAME
        std::string class_name = this->class_name();
        sys_debug_print(SYSTEM_LEVEL_INFO, "tools::bind<T>(const bind&);", "T = %s", class_name);
        #endif // SHOW_BIND_CLASS_NAME
    }
    virtual ~bind(void) {
        #ifdef SHOW_BIND_CLASS_NAME
        std::string class_name = this->class_name();
        sys_debug_print(SYSTEM_LEVEL_INFO, "tools::~bind<T>(void);", "T = %s", class_name);
        #endif // SHOW_BIND_CLASS_NAME
    }
public: // Operators
    const bool operator!(void) const {
        return this->value_ == T();
    }
    bind& operator=(const T& value) {
        this->value_ = value;
        this->value_changed();
        return *this;
    }
    constexpr operator const T&() const {
        return this->value_;
    }
    T& operator*(void) {
        return *this;
    }
    T* operator->(void) {
        return &this->value_;
    }
    BIND_COMPARE_OPERATOR(==);
    BIND_COMPARE_OPERATOR(!=);
    BIND_COMPARE_OPERATOR(>=);
    BIND_COMPARE_OPERATOR(<=);
    BIND_COMPARE_OPERATOR(>);
    BIND_COMPARE_OPERATOR(<);
    BIND_POSTFIX_OPERATOR(++);
    BIND_POSTFIX_OPERATOR(--);
    BIND_PREFIX_OPERATOR(++);
    BIND_PREFIX_OPERATOR(--);
    BIND_ASSIGN_OPERATOR(+=);
    BIND_ASSIGN_OPERATOR(-=);
    BIND_ASSIGN_OPERATOR(*=);
    BIND_ASSIGN_OPERATOR(/=);
    BIND_ASSIGN_OPERATOR(%=);
protected: // Derived methods
    virtual void print(std::ostream& stream) const {
        stream << this->value_;
    }
    virtual void store(std::istream& stream) {
        stream >> this->value_;
    }
public: // Public properties
    const T& value(void) const {
        return this->value_;
    }
    void value(const T& value, bool event = true) {
        this->value_ = value;
        if (event) this->value_changed();
    }
protected: // Private fields
    T value_;
};

}}

#endif // INITPLUS_GENERIC_BIND_HPP_INCLUDED
