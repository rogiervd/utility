/*
Copyright 2014 Rogier van Dalen.

This file is part of Rogier van Dalen's Utility library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
\file
std::is_default_constructible is not available on GCC 4.6.
This provides an equivalent class.
*/

#ifndef UTILITY_IS_DEFAULT_CONSTRUCTIBLE_HPP_INCLUDED
#define UTILITY_IS_DEFAULT_CONSTRUCTIBLE_HPP_INCLUDED

#include <type_traits>

namespace utility {

    namespace is_default_constructible_detail {

        /* Fixes where GCC 4.6 goes wrong for is_*_default_constructible. */

        template <template <class ...> class Predicate, class Type,
            class Enable = void>
        struct compute
        : Predicate <Type> {};

        // References.
        // std::is_constructible on GCC 4.6 gives a compile error if these are
        // not specialised.
        template <template <class ...> class Predicate, class Type>
            struct compute <Predicate, Type &>
        : std::integral_constant <bool, false> {};
        template <template <class ...> class Predicate, class Type>
            struct compute <Predicate, Type &&>
        : std::integral_constant <bool, false> {};

        // Function types.
        template <template <class ...> class Predicate,
            class Return, class ... Arguments>
        struct compute <Predicate, Return (Arguments ...)>
        : std::integral_constant <bool, true> {};

        // void.
        template <template <class ...> class Predicate>
            struct compute <Predicate, void>
        : std::integral_constant <bool, false> {};

        // Abstract classes.
        template <template <class ...> class Predicate, class Type>
            struct compute <Predicate, Type,
                typename std::enable_if <std::is_abstract <Type>::value>::type>
        : std::integral_constant <bool, false> {};

        // Arrays.
        template <template <class ...> class Predicate,
            class Type, std::size_t N>
        struct compute <Predicate, Type [N]>
        : compute <Predicate, Type> {};

    } // namespace is_default_constructible_detail

    /**
    Implementation of std::is_default_constructible in case the standard library
    does not provide it.
    */
    template <class Type> struct is_default_constructible
    : is_default_constructible_detail::compute <std::is_constructible, Type> {};

    /**
    Implementation of std::is_nothrow_default_constructible in case the standard
    library does not provide it.
    */
    template <class Type> struct is_nothrow_default_constructible
    : is_default_constructible_detail::compute <
        std::is_nothrow_constructible, Type> {};

} // namespace utility

#endif // UTILITY_IS_DEFAULT_CONSTRUCTIBLE_HPP_INCLUDED
