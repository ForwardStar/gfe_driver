//
// Created by sjf on 8/24/2022.
//
#include "platform_atomics.h"

#include <cstdint>

//template<typename T, typename U>
//T fetch_and_add(T &x, U inc) {
//    return __sync_fetch_and_add(&x, inc);
//}
//
//template<typename T>
//bool compare_and_swap(T &x, const T &old_val, const T &new_val) {
//    return __sync_bool_compare_and_swap(&x, old_val, new_val);
//}

bool compare_and_swap(float &x, const float &old_val, const float &new_val) {
    return __sync_bool_compare_and_swap(reinterpret_cast<uint32_t*>(&x),
                                        reinterpret_cast<const uint32_t&>(old_val),
                                        reinterpret_cast<const uint32_t&>(new_val));
}

bool compare_and_swap(double &x, const double &old_val, const double &new_val) {
    return __sync_bool_compare_and_swap(reinterpret_cast<uint64_t*>(&x),
                                        reinterpret_cast<const uint64_t&>(old_val),
                                        reinterpret_cast<const uint64_t&>(new_val));
}
