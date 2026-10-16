// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UTILITY_ORDER_OVERLOAD_HPP_INCLUDED
#define UTILITY_ORDER_OVERLOAD_HPP_INCLUDED

namespace utility {

/**
Helper class to disambiguate function overloads.
Use a pointer to this class as an argument type, with different values for
"priority" for each overloaded function.
Then call the class with "pick_overload()" for that argument.
*/
template <int priority> struct overload_order : overload_order<priority + 1> {};

template <> struct overload_order<32> {};

inline overload_order<0> * pick_overload() { return nullptr; }

}  // namespace utility

#endif  // UTILITY_ORDER_OVERLOAD_HPP_INCLUDED
