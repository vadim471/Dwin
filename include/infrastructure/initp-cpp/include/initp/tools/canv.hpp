#ifndef INITPLUS_TOOLS_CANV_HPP_INCLUDED
#define INITPLUS_TOOLS_CANV_HPP_INCLUDED

#include "cancellable.hpp"
#include "event_proxy.hpp"

#include <functional>

namespace initp {
namespace tools {

template<typename T>
class canv:
    public cancellable {
private: // Private types
    typedef canv<T> self_type;
public: // Public types
    typedef event_proxy<void(void)> on_change_proxy;
    typedef typename T::value_type value_type;
    typedef std::function<bool(const value_type&, const value_type&)> comparator_type;
    typedef std::function<value_type(const value_type&)> builder_type;
public: // Construction
    canv(void) {
        //MEMORY_("shared::tools::canv<T>(void);");
    }
    canv(const comparator_type& c, const builder_type& b):
        comparator_(c),
        builder_(b) {
        //MEMORY_("shared::tools::canv<T>(const comparator_type&);");
    }
    canv(const T& v):
        current_(v),
        previous_(v) {
        //MEMORY_("shared::tools::canv<T>(const T&);");
    }
    canv(const canv& v):
        current_(v.current_),
        previous_(v.previous_),
        comparator_(v.comparator_),
        builder_(v.builder_) {
        //MEMORY_("shared::tools::canv<T>(const canv&);");
    }
    virtual ~canv(void) {
        //MEMORY_("shared::tools::~canv<T>(void);");
    }
public: // Operators
    T& operator*(void) {
        return this->current_;
    }
    T* operator->(void) {
        return &this->current_;
    }
    canv& operator=(const T& value) {
        this->current_ = value;
        return *this;
    }
    bool operator==(const T& value) const {
        return this->equal(this->current_, value);
    }
    bool operator==(const self_type& value) const {
        return this->equal(this->current_, value.current_);
    }
public: // Container methods
    typename T::iterator begin(void) {
        return this->current_.begin();
    }
    typename T::iterator end(void) {
        return this->current_.end();
    }
    typename T::const_iterator cbegin(void) const {
        return this->current_.cbegin();
    }
    typename T::const_iterator cend(void) const {
        return this->current_.cend();
    }
    typename T::size_type size(void) const {
        return this->current_.size();
    }
    bool empty(void) const {
        return this->current_.empty();
    }
public: // Public methods
    virtual void accept(void) {
        if (this->changed()) {
            if (!this->builder_) {
                this->previous_ = this->current_;
            } else {
                this->previous_.clear();
                for (typename T::const_iterator it = this->current_.cbegin(); it != this->current_.cend(); ++it) {
                    this->previous_.push_back(this->builder_(*it));
                }
            }
        }
    }
    virtual void cancel(void) {
        if (this->changed()) {
            if (!this->builder_) {
                this->current_ = this->previous_;
            } else {
                this->current_.clear();
                for (typename T::const_iterator it = this->previous_.cbegin(); it != this->previous_.cend(); ++it) {
                    this->current_.push_back(this->builder_(*it));
                }
            }
        }
    }
    T& get(void) {
        return this->current_;
    }
public: // Public properties
    virtual bool changed(void) const {
        return !this->equal(this->current_, this->previous_);
    }
    on_change_proxy& on_change(void) {
        return this->on_change_;
    }
    const comparator_type& comparator(void) const {
        return this->comparator_;
    }
    void comparator(const comparator_type& value) {
        this->comparator_ = value;
    }
    const builder_type& builder(void) const {
        return this->builder_;
    }
    void builder(const builder_type& value) {
        this->builder_ = value;
    }
private: // Private methods
    bool equal(const T& v1, const T& v2) const {
        for (typename T::const_iterator it1 = v1.cbegin(), it2 = v2.cbegin();; ++it1, ++it2) {
            if (it1 == v1.end())
                return it2 == v2.end();
            if (it2 == v2.end())
                return it1 == v1.end();
            if (!this->comparator_) {
                if (!((*it1) == (*it2))) {
                    return false;
                }
            } else {
                if (!this->comparator_(*it1, *it2)) {
                    return false;
                }
            }
        }
        return true;
    }
private:
    on_change_proxy on_change_;
    T current_;
    T previous_;
    comparator_type comparator_;
    builder_type builder_;
};

}}

#endif // INITPLUS_TOOLS_CANV_HPP_INCLUDED
