#ifndef TYPES_H
#define TYPES_H

typedef void (*func_ptr_t)(...);
template <typename R, typename... Args>
using func_ptr_t = R(*)(Args...);

class time_t
{

};

#endif