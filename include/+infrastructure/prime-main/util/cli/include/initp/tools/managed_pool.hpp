#ifndef INITPLUS_CLI_TOOLS_MANAGED_POOL_HPP_INCLUDED
#define INITPLUS_CLI_TOOLS_MANAGED_POOL_HPP_INCLUDED

#include <cstddef>

namespace initp {
namespace tools {

using namespace System::Collections::Generic;

template<class T>
ref class managed_pool {
private:
    typedef managed_pool<T> self_type;
public:
    typedef T^ type;
public:
    static void* push(type object) {
        size_t index = ++index_;
		pool_.Add(index, object);
        return (void*)index;
    }
    static type get(void* object) {
        if (pool_.ContainsKey((size_t)object))
			return pool_[(size_t)object];
        return nullptr;
    }
    static void erase(void* object) {
		if (pool_.ContainsKey((size_t)object)) {
			pool_.Remove((size_t)object);
		}
	}
private:
    static Dictionary<size_t, type> pool_;
    static size_t index_ = 0;
};

}}

#endif // INITPLUS_CLI_TOOLS_MANAGED_POOL_HPP_INCLUDED
