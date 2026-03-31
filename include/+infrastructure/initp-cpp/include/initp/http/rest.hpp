#ifndef INITPLUS_HTTP_REST_HPP_INCLUDED
#define INITPLUS_HTTP_REST_HPP_INCLUDED

//#include <iostream>

#include <boost/algorithm/string/predicate.hpp>
#include <functional>
#include <iterator>
#include <memory>
#include <vector>
#include <string>

namespace initp {
namespace http {
namespace rest {

/**
 * \brief  ласс, позвол€ющий строить дерево объектов дл€ REST.
 */
template<typename Meta, typename Method, typename T, typename... Args>
class node {
private:
    typedef node<Meta, Method, T, Args...> self_type;
public:
    typedef std::function<T(Args...)> handler_type;
private:
    class pointer_wrapper;
    class group_wrapper;
public:
    node(void) {}
    node(const self_type&) = delete;
    virtual ~node(void) = default;
public:
    virtual self_type* find_rest_node(const Method& method, const std::vector<std::string>& path) {
        if (path.empty()) return NULL;
        //std::cout << "Looking in " << this->rest_id() << std::endl;
        if (!boost::iequals(path.front(), this->rest_id())) return NULL;
        std::vector<std::string> subpath(std::next(path.begin()), path.end());
        if (subpath.empty()) {
            if (this->rest_method() == method) {
                //std::cout << "Node found " << this->rest_id() << std::endl;
                return this;
            } else return NULL;
        }
        self_type* result;
        for (auto& child : this->children_) {
            result = child->find_rest_node(method, subpath);
            if (result) return result;
        }
        return NULL;
    }
    virtual void erase(const std::string& id) {
        for (auto it = this->children_.begin(); it != this->children_.end();) {
            if ((*it)->rest_id() == id) {
                it = this->children_.erase(it);
            } else it++;
        }
    }
    virtual void clear_rest(void) {
        this->children_.clear();
    }
public:
    virtual void create_rest_child(self_type& child);
    virtual void create_rest_function(const Method& method, const std::string& name, const handler_type& f, const Meta& meta);
    virtual self_type& create_rest_group(const Method& method, const std::string& name, const handler_type& f, const Meta& meta);
public:
    virtual const std::string& rest_id(void) const = 0;
    virtual const Method& rest_method(void) const = 0;
    virtual const Meta& rest_meta(void) const = 0;
    virtual T operator()(Args... args) = 0;
private:
    std::vector<std::unique_ptr<self_type>> children_;
};

template<typename Meta, typename Method, typename T, typename... Args>
class node<Meta, Method, T, Args...>::pointer_wrapper:
    public node<Meta, Method, T, Args...> {
public:
    pointer_wrapper(self_type* pointer):
        pointer_(pointer) {}
public:
    virtual self_type* find_rest_node(const Method& method, const std::vector<std::string>& path) {
        return this->pointer_->find_rest_node(method, path);
    }
    virtual void erase(const std::string& id) {
        return this->pointer_->erase(id);
    }
    virtual void clear_rest(void) {
        this->pointer_->clear_rest();
    }
    virtual void create_rest_child(self_type& child) {
        return this->pointer_->create_rest_child(child);
    }
    virtual void create_rest_function(const Method& method, const std::string& name, const handler_type& f, const Meta& meta) {
        return this->pointer_->create_rest_function(method, name, f, meta);
    }
    virtual self_type& create_rest_group(const Method& method, const std::string& name, const handler_type& f, const Meta& meta) {
        return this->pointer_->create_rest_group(method, name, f, meta);
    }
    virtual const std::string& rest_id(void) const {
        return this->pointer_->rest_id();
    }
    virtual const Method& rest_method(void) const {
        return this->pointer_->rest_method();
    }
    virtual const Meta& rest_meta(void) const {
        return this->pointer_->rest_meta();
    }
    virtual T operator()(Args... args) {
        return this->pointer_->operator()(args...);
    }
private:
    self_type* pointer_;
};

template<typename Meta, typename Method, typename T, typename... Args>
class node<Meta, Method, T, Args...>::group_wrapper:
    public node<Meta, Method, T, Args...> {
public:
    group_wrapper(const Method& method, const std::string& rest_id, const handler_type& handler, const Meta& meta):
        rest_id_(rest_id),
        handler_(handler),
        method_(method),
        meta_(meta) {}
public:
    virtual const std::string& rest_id(void) const {
        return this->rest_id_;
    }
    virtual const Method& rest_method(void) const {
        return this->method_;
    }
    virtual const Meta& rest_meta(void) const {
        return this->meta_;
    }
    virtual T operator()(Args... args) {
        return this->handler_(args...);
    }
private:
    std::string rest_id_;
    handler_type handler_;
    Method method_;
    Meta meta_;
};

template<typename Meta, typename Method, typename T, typename... Args>
void node<Meta, Method, T, Args...>::create_rest_child(node& child) {
    this->children_.emplace_back(new pointer_wrapper(&child));
}

template<typename Meta, typename Method, typename T, typename... Args>
void node<Meta, Method, T, Args...>::create_rest_function(const Method& method, const std::string& name, const handler_type& handler, const Meta& meta) {
    this->children_.emplace_back(new group_wrapper(method, name, handler, meta));
}

template<typename Meta, typename Method, typename T, typename... Args>
node<Meta, Method, T, Args...>& node<Meta, Method, T, Args...>::create_rest_group(const Method& method, const std::string& name, const handler_type& handler, const Meta& meta) {
    for (std::unique_ptr<self_type>& child : this->children_)
        if (child->rest_id() == name)
            if (dynamic_cast<group_wrapper*>(child.get()))
                return *child;
    this->children_.emplace_back(new group_wrapper(method, name, handler, meta));
    return *this->children_.back();
}

    // For Meta=void method rest_meta() not defined

template<typename Method, typename T, typename... Args>
class node<void, Method, T, Args...> {
private:
    typedef node<void, Method, T, Args...> self_type;
public:
    typedef std::function<T(Args...)> handler_type;
private:
    class pointer_wrapper;
    class group_wrapper;
public:
    node(void) {}
    node(const self_type&) = delete;
    virtual ~node(void) = default;
public:
    virtual self_type* find_rest_node(const Method& method, const std::vector<std::string>& path) {
        if (path.empty()) return NULL;
        if (path.front() != this->rest_id()) return NULL;
        std::vector<std::string> subpath(std::next(path.begin()), path.end());
        if (subpath.empty()) {
            if (this->rest_method() == method) {
                return this;
            } else return NULL;
        }
        self_type* result;
        for (auto& child : this->children_) {
            result = child->find_rest_node(method, subpath);
            if (result) return result;
        }
        return NULL;
    }
    virtual void erase(const std::string& id) {
        for (auto it = this->children_.begin(); it != this->children_.end();) {
            if ((*it)->rest_id() == id) {
                it = this->children_.erase(it);
            } else it++;
        }
    }
    virtual void clear_rest(void) {
        this->children_.clear();
    }
public:
    virtual void create_rest_child(self_type&);
    virtual void create_rest_function(const Method&, const std::string&, const handler_type&);
    virtual self_type& create_rest_group(const Method&, const std::string&, const handler_type&);
public:
    virtual const std::string& rest_id(void) const = 0;
    virtual const Method& rest_method(void) const = 0;
    virtual T operator()(Args... args) = 0;
private:
    std::vector<std::unique_ptr<self_type>> children_;
};

template<typename Method, typename T, typename... Args>
class node<void, Method, T, Args...>::pointer_wrapper:
    public node<void, Method, T, Args...> {
public:
    pointer_wrapper(self_type* pointer):
        pointer_(pointer) {}
public:
    virtual self_type* find_rest_node(const Method& method, const std::vector<std::string>& path) {
        return this->pointer_->find_rest_node(method, path);
    }
    virtual void erase(const std::string& id) {
        return this->pointer_->erase(id);
    }
    virtual void clear_rest(void) {
        this->pointer_->clear_rest();
    }
    virtual void create_rest_child(self_type& child) {
        return this->pointer_->create_rest_child(child);
    }
    virtual void create_rest_function(const Method& method, const std::string& name, const handler_type& handler) {
        return this->pointer_->create_rest_function(method, name, handler);
    }
    virtual self_type& create_rest_group(const Method& method, const std::string& name, const handler_type& handler) {
        return this->pointer_->create_rest_group(method, name, handler);
    }
    virtual const std::string& rest_id(void) const {
        return this->pointer_->rest_id();
    }
    virtual const Method& rest_method(void) const {
        return this->pointer_->rest_method();
    }
    virtual T operator()(Args... args) {
        return this->pointer_->operator()(args...);
    }
private:
    self_type* pointer_;
};

template<typename Method, typename T, typename... Args>
class node<void, Method, T, Args...>::group_wrapper:
    public node<void, Method, T, Args...> {
public:
    group_wrapper(const Method& method, const std::string& rest_id, const handler_type& handler):
        rest_id_(rest_id),
        method_(method),
        handler_(handler) {}
public:
    virtual const std::string& rest_id(void) const {
        return this->rest_id_;
    }
    virtual const Method& rest_method(void) const {
        return this->method_;
    }
    virtual T operator()(Args... args) {
        return this->handler_(args...);
    }
private:
    std::string rest_id_;
    Method method_;
    handler_type handler_;
};

template<typename Method, typename T, typename... Args>
void node<void, Method, T, Args...>::create_rest_child(node& child) {
    this->children_.emplace_back(new pointer_wrapper(&child));
}

template<typename Method, typename T, typename... Args>
void node<void, Method, T, Args...>::create_rest_function(const Method& method, const std::string& name, const handler_type& handler) {
    this->children_.emplace_back(new group_wrapper(method, name, handler));
}

template<typename Method, typename T, typename... Args>
node<void, Method, T, Args...>& node<void, Method, T, Args...>::create_rest_group(const Method& method, const std::string& name, const handler_type& handler) {
    for (std::unique_ptr<self_type>& child : this->children_)
        if (child->rest_id() == name)
            if (dynamic_cast<group_wrapper*>(child.get()))
                return *child;
    this->children_.emplace_back(new group_wrapper(method, name, handler));
    return *this->children_.back();
}

}}}

#endif // INITPLUS_HTTP_REST_HPP_INCLUDED
