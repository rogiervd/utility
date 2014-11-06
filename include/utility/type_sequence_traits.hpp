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

/** \file
Define type traits for pairs of sequences of types.
This essentially duplicates a number of traits from type_traits.
Instead of starting with "is_", they start with "are_".

Unary traits are not provided; they are probably easy enough to write as, e.g.,
<c> meta::all \<meta::vector \<std::is_default_constructible \<Types> ...>>
</c>.
*/

#ifndef UTILITY_TYPE_SEQUENCE_TRAITS_HPP_INCLUDED
#define UTILITY_TYPE_SEQUENCE_TRAITS_HPP_INCLUDED

#include <type_traits>

#include "meta/vector.hpp"

#include "is_assignable.hpp"

namespace utility {

    namespace type_sequence_traits_detail {

        /**
        \return true iff all of the boolean predicates are true.
        */
        template <class ... Booleans> struct all;

        template <class First, class ... Rest> struct all <First, Rest ...>
        : std::conditional <First::value, all <Rest ...>, std::false_type>::type
        {};

        template <> struct all<>
        : std::true_type {};

        // Make a two-argument version.
        template <class Type1, class Type2> struct is_constructible
        : std::is_constructible <Type1, Type2> {};

        template <class Type1, class Type2> struct is_nothrow_constructible
        : std::is_nothrow_constructible <Type1, Type2> {};

    } // namespace type_sequence_traits_detail

    /**
    Evaluate to \c true iff the two meta ranges are of the same length, and
    Predicate returns true for each pair of types.
    */
    template <template <class, class> class Predicate,
        class Types1, class Types2, class Enable = void>
    struct all_binary
    : all_binary <Predicate, typename meta::as_vector <Types1>::type,
        typename meta::as_vector <Types2>::type> {};

    // Different lengths: always false.
    template <template <class, class> class Predicate,
        class ... Types1, class ... Types2>
    struct all_binary <Predicate,
        meta::vector <Types1 ...>, meta::vector <Types2 ...>, typename
        std::enable_if <sizeof ... (Types1) != sizeof ... (Types2)>::type>
    : std::false_type {};

    // The same length.
    template <template <class, class> class Predicate,
        class ... Types1, class ... Types2>
    struct all_binary <Predicate,
        meta::vector <Types1 ...>, meta::vector <Types2 ...>, typename
        std::enable_if <sizeof ... (Types1) == sizeof ... (Types2)>::type>
    : type_sequence_traits_detail::all <
        Predicate <Types1, Types2> ...> {};

    /**
    Evaluate to \c true iff the two meta ranges are of the same length, and
    their types are pairwise convertible.
    */
    template <class Types1, class Types2> struct are_convertible
    : all_binary <std::is_convertible, Types1, Types2> {};

    /**
    Evaluate to \c true iff the two meta ranges are of the same length, and
    their types are constructible.
    */
    template <class Types1, class Types2> struct are_constructible
    : all_binary <type_sequence_traits_detail::is_constructible, Types1, Types2>
    {};

    /**
    Evaluate to \c true iff the two meta ranges are of the same length, and
    their types are nothrow constructible.
    */
    template <class Types1, class Types2> struct are_nothrow_constructible
    : all_binary <type_sequence_traits_detail::is_nothrow_constructible,
        Types1, Types2> {};

    /**
    Evaluate to \c true iff the two meta ranges are of the same length, and
    their types are assignable.
    */
    template <class Types1, class Types2> struct are_assignable
    : all_binary <utility::is_assignable, Types1, Types2> {};

    /**
    Evaluate to \c true iff the two meta ranges are of the same length, and
    their types are nothrow assignable.
    */
    template <class Types1, class Types2> struct are_nothrow_assignable
    : all_binary <utility::is_nothrow_assignable, Types1, Types2> {};

} // namespace math

#endif // UTILITY_TYPE_SEQUENCE_TRAITS_HPP_INCLUDED
