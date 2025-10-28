#ifndef TYPES_H
#define TYPES_H

//Types will most likely be reworked in the future based on patterns in behavior and return types

typedef void (*func_ptr_t)(...);
template <typename R, typename... Args>
using func_ptr_t = R(*)(Args...);

class time_t
{

};

#endif