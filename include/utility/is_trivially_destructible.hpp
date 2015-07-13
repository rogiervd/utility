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

/**
\file
std::is_trivially_destructible is not available on older standard libraries,
because it used to be called has_trivial_destructor.
This provides a consistent interface.
*/

#ifndef UTILITY_IS_TRIVIALLY_DESTRUCTIBLE_HPP_INCLUDED
#define UTILITY_IS_TRIVIALLY_DESTRUCTIBLE_HPP_INCLUDED

#include <type_traits>

// Declare both names in namespace std.
// This is technically undefined behaviour.
namespace std {

    template <class> struct has_trivial_destructor;
    template <class> struct is_trivially_destructible;

} // namespace std

namespace utility {

    namespace is_trivially_destructible_detail {

        template <bool> struct make_void { typedef void type; };

        template <class Type, class Enable = void>
            struct is_trivially_destructible;

        // Version that works if std::is_trivially_destructible is defined.
        template <class Type>
            struct is_trivially_destructible <Type,
                typename make_void <std::is_trivially_destructible <Type>::type
                    ::value>::type>
        : std::is_trivially_destructible <Type> {};

        // Version that works if std::has_trivial_destructor is defined.
        template <class Type>
            struct is_trivially_destructible <Type,
                typename make_void <std::has_trivial_destructor <Type>::type
                    ::value>::type>
        : std::has_trivial_destructor <Type> {};

    } // namespace is_trivially_destructible_detail


    template <class Type> struct is_trivially_destructible
    : is_trivially_destructible_detail::is_trivially_destructible <Type> {};

} // namespace utility

#endif // UTILITY_IS_TRIVIALLY_DESTRUCTIBLE_HPP_INCLUDED
