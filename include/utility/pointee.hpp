// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UTILITY_POINTEE_HPP_INCLUDED
#define UTILITY_POINTEE_HPP_INCLUDED

#include <type_traits>

namespace utility {

/** \brief
Compute the pointed-to type from a pointer type.

The resulting type is not reference-qualified, but it may be
const-qualified.
*/
template <class Pointer> struct pointee
: std::remove_reference<decltype(*std::declval<Pointer>())> {};

}  // namespace utility

#endif  // UTILITY_POINTEE_HPP_INCLUDED
