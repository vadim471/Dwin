#ifndef INITPLUS_GENERIC_ENABLE_CLASS_NAME_HPP_INCLUDED
#define INITPLUS_GENERIC_ENABLE_CLASS_NAME_HPP_INCLUDED

#include <string>
#include <cxxabi.h>

namespace initp {
namespace tools {

template<typename T>
inline std::string get_class_name(void) {
    int status;
    char* name = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
    if (name && !status) {
        std::string result(name);
        free(name);
        return result;
    } else return std::string();
}

template<typename T, bool dynamic = false>
class enable_class_name;

template<typename T>
class enable_class_name<T, false> {
protected:
    enable_class_name(void):
        class_name_(get_class_name<T>()) {}
public:
    virtual ~enable_class_name(void) {}
public:
    virtual const std::string& class_name(void) const {
        return this->class_name_;
    }
protected:
    std::string class_name_;
};

template<typename T>
class enable_class_name<T, true> {
protected:
    enable_class_name(void) {}
public:
    virtual ~enable_class_name(void) {}
public:
    virtual std::string class_name(void) const {
        return get_class_name<T>();
    }
};

}}

#endif // INITPLUS_GENERIC_ENABLE_CLASS_NAME_HPP_INCLUDED
