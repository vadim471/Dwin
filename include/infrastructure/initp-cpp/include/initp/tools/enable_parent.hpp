#ifndef INITPLUS_TOOLS_ENABLE_PARENT_HPP_INCLUDED
#define INITPLUS_TOOLS_ENABLE_PARENT_HPP_INCLUDED

#include <memory>
#include <type_traits>

namespace initp {
namespace tools {

template<typename T>
class enable_parent {
protected: // Constructors
    enable_parent(T& parent):
        parent_(parent) {}
    enable_parent(const enable_parent<T>& v):
        parent_(v.parent_) {}
    enable_parent(enable_parent<T>&& v):
        parent_(v.parent_) {}
public: // Destructor
    virtual ~enable_parent(void) {}
public: // Public properties
    T& parent(void) const {
        return this->parent_;
    }
protected: // Private fields
    T& parent_;
};

template<typename T>
class enable_ptr_parent {
protected: // Constructors
    enable_ptr_parent(void):
        parent_() {}
    enable_ptr_parent(const std::shared_ptr<T>& parent):
        parent_(parent) {}
    enable_ptr_parent(const enable_ptr_parent<T>& v):
        parent_(v.parent_) {}
public: // Destructor
    virtual ~enable_ptr_parent(void) {}
public: // Public properties
    virtual std::shared_ptr<T> parent(void) const {
        return this->parent_.lock();
    }
    virtual void parent(const std::shared_ptr<T>& parent) {
        this->parent_ = parent;
    }
protected: // Private fields
    std::weak_ptr<T> parent_;
};

template<typename T>
class enable_raw_parent {
protected: // Constructors
    enable_raw_parent(void):
        parent_() {}
    enable_raw_parent(T* parent):
        parent_(parent) {}
public: // Destructor
    virtual ~enable_raw_parent(void) {}
public: // Public properties
    template<class T2 = T>
    typename std::enable_if<!std::is_base_of<std::enable_shared_from_this<T2>, T2>::value, T*>::type
    parent(void) const {
        return this->parent_;
    }
    template<class T2 = T>
    typename std::enable_if<std::is_base_of<std::enable_shared_from_this<T2>, T2>::value, std::shared_ptr<T>>::type
    parent(void) const {
        return this->parent_->shared_from_this();
    }
    virtual void parent(T* parent) {
        this->parent_ = parent;
    }
protected: // Private fields
    T* parent_;
};

}}

#endif // INITPLUS_TOOLS_ENABLE_PARENT_HPP_INCLUDED
