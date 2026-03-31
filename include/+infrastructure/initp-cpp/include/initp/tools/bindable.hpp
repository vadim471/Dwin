#ifndef INITPLUS_TOOLS_BINDABLE_HPP_INCLUDED
#define INITPLUS_TOOLS_BINDABLE_HPP_INCLUDED

#include "event_proxy_c11.hpp"

#include <iostream>
#include <vector>
#include <functional>

namespace initp {
namespace tools {

class bindable {
public: // Public types
    typedef event_proxy<void(void)> events;
protected: // Constructors
    bindable(void):
        update_(false) {}
    bindable(const bindable& v):
        on_change_(v.on_change_),
        on_delete_(v.on_delete_),
        update_(false) {}
public: // Destructor
    virtual ~bindable(void) {
        this->deleted();
    }
public: // Operators
    friend std::ostream& operator<<(std::ostream& stream, const bindable& instance) {
        instance.print(stream);
        return stream;
    }
    friend std::istream& operator>>(std::istream& stream, bindable& instance) {
        instance.store(stream);
        instance.value_changed();
        return stream;
    }
protected: // Pure virtual
    virtual void print(std::ostream&) const = 0;
    virtual void store(std::istream&) = 0;
public: // Public properties
    template<typename T>
    T parse(void) const {
        T value;
        std::stringstream string;
        this->print(string);
        string >> value;
        return value;
    }
    template<typename T>
    void store(const T& value, void* object) {
        if (this->update_) return;
        this->update_ = true;
        std::stringstream string;
        string << value;
        this->store(string);
        this->on_change_.rise_except(object);
        this->update_ = false;
    }
    events& on_change(void) {
        return this->on_change_;
    }
    events& on_delete(void) {
        return this->on_delete_;
    }
protected: // Private methods
    void value_changed(void) {
        if (this->update_) return;
        this->update_ = true;
        this->on_change_.rise();
        this->update_ = false;
    }
    void deleted(void) {
        this->on_delete_.rise();
    }
protected: // Private fields
    events on_change_;
    events on_delete_;
    bool update_;
};

}}

#endif // INITPLUS_TOOLS_BINDABLE_HPP_INCLUDED
