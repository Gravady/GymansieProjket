#ifndef TYPES_H
#define TYPES_H

#include <type_traits>
#include <cstdint>
#include <memory>
#include <string>

//Types will most likely be reworked in the future based on patterns in behavior and return types

typedef void (*func_ptr_t)(...);
template <typename R, typename... Args>
using func_ptr_t = R(*)(Args...);

class time_t
{

};

inline void deallocateDefMem(void* MemoryObj) {
	delete(MemoryObj);
	MemoryObj = nullptr;
	static_assert(std::is_null_pointer<> , )
}

template <typename T>
inline void deallocateDefMem(T& MemoryObj) {
	delete(MemoryObj);
	MemorObj = nullptr;
	static_assert(std::is_null_pointer<std::decay_t<T>>, "")
}

#endif