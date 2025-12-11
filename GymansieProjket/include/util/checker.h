#ifndef CHECKER_H
#define CHECKER_H

//Checker made as C++17 is used not supporting concepts

#include <type_traits>
#include <string>
#include <cstdint>

template <typename, typename = std::void_t<>>
struct has_member_cons : std::false_type {};

template <typename T>
struct has_member_cons<T, std::enable_if_t<
    std::is_constructible_v<T>&& std::is_same_v<void, std::void_t<decltype(std::declval<T>().type)>>
    >> : std::true_type {};


//TODO:Make this general for pointer types and general types
template <typename, typename = void>
struct has_member : std::false_type {};

//template <typename T>
//struct has_member<T, std::enable_if_t<>>

#endif