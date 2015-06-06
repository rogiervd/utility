/*
Copyright 2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
    : std::remove_reference <decltype (*std::declval <Pointer>())> {};

} // namespace utility

#endif // UTILITY_POINTEE_HPP_INCLUDED
