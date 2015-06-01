/*
Copyright 2014 Rogier van Dalen.

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

#include "is_default_constructible.hpp"
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
    Evaluate to \c true iff Predicate returns true for each type in meta range
    Types.
    */
    template <template <class> class Predicate, class Types>
    struct all_unary
    : all_unary <Predicate, typename meta::as_vector <Types>::type> {};

    template <template <class> class Predicate, class ... Types>
    struct all_unary <Predicate, meta::vector <Types ...>>
    : type_sequence_traits_detail::all <Predicate <Types> ...> {};

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
    Evaluate to \c true iff all types in \a Types are default-constructible.
    */
    template <class Types> struct are_default_constructible
    : all_unary <is_default_constructible, Types> {};

    /**
    Evaluate to \c true iff all types in \a Types are default-constructible
    without exceptions.
    */
    template <class Types> struct are_nothrow_default_constructible
    : all_unary <is_nothrow_default_constructible, Types> {};

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
