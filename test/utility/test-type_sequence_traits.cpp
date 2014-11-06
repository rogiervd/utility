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

#define BOOST_TEST_MODULE test_utility_type_sequence_traits_binary
#include "utility/test/boost_unit_test.hpp"

#include "utility/type_sequence_traits.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_type_sequence_traits_binary)

/**
Check that a binary predicate on a sequence corresponds to one on single types.
*/
template <template <class, class> class SinglePredicate,
    template <class, class> class AllPredicate>
struct check_binary_predicate {

    template <class TypeA, class TypeB>
        void test_binary_with_one() const
    {
        static_assert (
            AllPredicate <meta::vector <TypeA>, meta::vector <TypeB>>::value ==
            SinglePredicate <TypeA, TypeB>::value, "");
    }

    template <class TypeA, class TypeB, class TypeC, class TypeD>
        void test_binary_with_two() const
    {
        static_assert (
            AllPredicate <meta::vector <TypeA, TypeB>,
                meta::vector <TypeC, TypeD>>::value ==
            (SinglePredicate <TypeA, TypeC>::value
                && SinglePredicate <TypeB, TypeD>::value), "");
    }

    template <class Type1, class Type2, class Type3> void check() const {
        // Zero length.
        static_assert (AllPredicate <meta::vector<>, meta::vector<>>::value,
            "");

        // Different lengths: always false.
        static_assert (!AllPredicate <
            meta::vector<>, meta::vector <Type1> >::value, "");
        static_assert (!AllPredicate <
            meta::vector<>, meta::vector <Type1, Type2> >::value, "");

        static_assert (!AllPredicate <
            meta::vector <Type1>, meta::vector<> >::value, "");
        static_assert (!AllPredicate <
            meta::vector <Type1>, meta::vector <Type1, Type2> >::value, "");

        static_assert (!AllPredicate <
            meta::vector <Type1, Type2>, meta::vector<> >::value, "");
        static_assert (!AllPredicate <
            meta::vector <Type1, Type2>, meta::vector <Type1> >::value, "");

        /*
        It is probably possible to be clever about this longish list below,
        but I am probably not clever enough to make no mistakes doing this.
        */

        // Same length: actual checks.
        test_binary_with_one <Type1, Type1>();
        test_binary_with_one <Type1, Type2>();
        test_binary_with_one <Type1, Type3>();

        test_binary_with_one <Type2, Type1>();
        test_binary_with_one <Type2, Type2>();
        test_binary_with_one <Type2, Type3>();

        test_binary_with_one <Type3, Type1>();
        test_binary_with_one <Type3, Type2>();
        test_binary_with_one <Type3, Type3>();

        // Type1, Type1.
        test_binary_with_two <Type1, Type1, Type1, Type1>();
        test_binary_with_two <Type1, Type1, Type1, Type2>();
        test_binary_with_two <Type1, Type1, Type1, Type3>();

        test_binary_with_two <Type1, Type1, Type2, Type1>();
        test_binary_with_two <Type1, Type1, Type2, Type2>();
        test_binary_with_two <Type1, Type1, Type2, Type3>();

        test_binary_with_two <Type1, Type1, Type3, Type1>();
        test_binary_with_two <Type1, Type1, Type3, Type2>();
        test_binary_with_two <Type1, Type1, Type3, Type3>();

        // Type1, Type2.
        test_binary_with_two <Type1, Type2, Type1, Type1>();
        test_binary_with_two <Type1, Type2, Type1, Type2>();
        test_binary_with_two <Type1, Type2, Type1, Type3>();

        test_binary_with_two <Type1, Type2, Type2, Type1>();
        test_binary_with_two <Type1, Type2, Type2, Type2>();
        test_binary_with_two <Type1, Type2, Type2, Type3>();

        test_binary_with_two <Type1, Type2, Type3, Type1>();
        test_binary_with_two <Type1, Type2, Type3, Type2>();
        test_binary_with_two <Type1, Type2, Type3, Type3>();

        // Type1, Type3.
        test_binary_with_two <Type1, Type3, Type1, Type1>();
        test_binary_with_two <Type1, Type3, Type1, Type2>();
        test_binary_with_two <Type1, Type3, Type1, Type3>();

        test_binary_with_two <Type1, Type3, Type2, Type1>();
        test_binary_with_two <Type1, Type3, Type2, Type2>();
        test_binary_with_two <Type1, Type3, Type2, Type3>();

        test_binary_with_two <Type1, Type3, Type3, Type1>();
        test_binary_with_two <Type1, Type3, Type3, Type2>();
        test_binary_with_two <Type1, Type3, Type3, Type3>();

        // Type2, Type1.
        test_binary_with_two <Type2, Type1, Type1, Type1>();
        test_binary_with_two <Type2, Type1, Type1, Type2>();
        test_binary_with_two <Type2, Type1, Type1, Type3>();

        test_binary_with_two <Type2, Type1, Type2, Type1>();
        test_binary_with_two <Type2, Type1, Type2, Type2>();
        test_binary_with_two <Type2, Type1, Type2, Type3>();

        test_binary_with_two <Type2, Type1, Type3, Type1>();
        test_binary_with_two <Type2, Type1, Type3, Type2>();
        test_binary_with_two <Type2, Type1, Type3, Type3>();

        // Type2, Type2.
        test_binary_with_two <Type2, Type2, Type1, Type1>();
        test_binary_with_two <Type2, Type2, Type1, Type2>();
        test_binary_with_two <Type2, Type2, Type1, Type3>();

        test_binary_with_two <Type2, Type2, Type2, Type1>();
        test_binary_with_two <Type2, Type2, Type2, Type2>();
        test_binary_with_two <Type2, Type2, Type2, Type3>();

        test_binary_with_two <Type2, Type2, Type3, Type1>();
        test_binary_with_two <Type2, Type2, Type3, Type2>();
        test_binary_with_two <Type2, Type2, Type3, Type3>();

        // Type2, Type3.
        test_binary_with_two <Type2, Type3, Type1, Type1>();
        test_binary_with_two <Type2, Type3, Type1, Type2>();
        test_binary_with_two <Type2, Type3, Type1, Type3>();

        test_binary_with_two <Type2, Type3, Type2, Type1>();
        test_binary_with_two <Type2, Type3, Type2, Type2>();
        test_binary_with_two <Type2, Type3, Type2, Type3>();

        test_binary_with_two <Type2, Type3, Type3, Type1>();
        test_binary_with_two <Type2, Type3, Type3, Type2>();
        test_binary_with_two <Type2, Type3, Type3, Type3>();

        // Type3, Type1.
        test_binary_with_two <Type3, Type1, Type1, Type1>();
        test_binary_with_two <Type3, Type1, Type1, Type2>();
        test_binary_with_two <Type3, Type1, Type1, Type3>();

        test_binary_with_two <Type3, Type1, Type2, Type1>();
        test_binary_with_two <Type3, Type1, Type2, Type2>();
        test_binary_with_two <Type3, Type1, Type2, Type3>();

        test_binary_with_two <Type3, Type1, Type3, Type1>();
        test_binary_with_two <Type3, Type1, Type3, Type2>();
        test_binary_with_two <Type3, Type1, Type3, Type3>();

        // Type3, Type2.
        test_binary_with_two <Type3, Type2, Type1, Type1>();
        test_binary_with_two <Type3, Type2, Type1, Type2>();
        test_binary_with_two <Type3, Type2, Type1, Type3>();

        test_binary_with_two <Type3, Type2, Type2, Type1>();
        test_binary_with_two <Type3, Type2, Type2, Type2>();
        test_binary_with_two <Type3, Type2, Type2, Type3>();

        test_binary_with_two <Type3, Type2, Type3, Type1>();
        test_binary_with_two <Type3, Type2, Type3, Type2>();
        test_binary_with_two <Type3, Type2, Type3, Type3>();

        // Type3, Type3.
        test_binary_with_two <Type3, Type3, Type1, Type1>();
        test_binary_with_two <Type3, Type3, Type1, Type2>();
        test_binary_with_two <Type3, Type3, Type1, Type3>();

        test_binary_with_two <Type3, Type3, Type2, Type1>();
        test_binary_with_two <Type3, Type3, Type2, Type2>();
        test_binary_with_two <Type3, Type3, Type2, Type3>();

        test_binary_with_two <Type3, Type3, Type3, Type1>();
        test_binary_with_two <Type3, Type3, Type3, Type2>();
        test_binary_with_two <Type3, Type3, Type3, Type3>();
    }
};


struct convertible_from {};

struct convertible_to {
    convertible_to (convertible_from const &) {}
};

struct explicitly_convertible_to {
    explicit explicitly_convertible_to (convertible_from const &)
    noexcept (false);
};

struct explicitly_nothrow_convertible_to {
    explicit explicitly_nothrow_convertible_to (convertible_from const &)
    noexcept;
};

BOOST_AUTO_TEST_CASE (test_utility_are_convertible) {
    static_assert (std::is_convertible <
        convertible_from, convertible_to>::value, "");
    static_assert (!std::is_convertible <
        convertible_from, explicitly_convertible_to>::value, "");
    check_binary_predicate <std::is_convertible, utility::are_convertible> test;
    test.check <convertible_from, convertible_to, explicitly_convertible_to>();
}

// is_constructible that takes only two parameters.
template <class Type1, class Type2> struct is_constructible
: std::is_constructible <Type1, Type2> {};

// is_nothrow_constructible that takes only two parameters.
template <class Type1, class Type2> struct is_nothrow_constructible
: std::is_nothrow_constructible <Type1, Type2> {};

BOOST_AUTO_TEST_CASE (test_utility_are_constructible) {
    // Check that the example types are exciting enough.
    static_assert (is_constructible <
        explicitly_convertible_to, convertible_from>::value, "");
    static_assert (!is_constructible <
        convertible_from, explicitly_convertible_to>::value, "");

    check_binary_predicate <is_constructible, utility::are_constructible> test;
    test.check <convertible_from, convertible_to, explicitly_convertible_to>();
}

BOOST_AUTO_TEST_CASE (test_utility_are_nothrow_constructible) {
    // Check that the example types are exciting enough.
    static_assert (!is_nothrow_constructible <
        explicitly_convertible_to, convertible_from>::value, "");
    static_assert (is_nothrow_constructible <
        explicitly_nothrow_convertible_to, convertible_from>::value, "");

    check_binary_predicate <is_nothrow_constructible,
        utility::are_nothrow_constructible> test;
    test.check <convertible_from, explicitly_convertible_to,
        explicitly_nothrow_convertible_to>();
}

/* are_*assignable. */

struct assignable_from {};

struct assignable_to {
    assignable_to & operator = (assignable_from const &) noexcept (false);
};

struct nothrow_assignable_to {
    nothrow_assignable_to & operator = (assignable_from const &) noexcept;
};

BOOST_AUTO_TEST_CASE (test_utility_are_assignable) {
    // Check that the example types are exciting enough.
    static_assert (!utility::is_assignable <
        assignable_from &, assignable_to const &>::value, "");
    static_assert (utility::is_assignable <
        assignable_to &, assignable_from const &>::value, "");
    static_assert (utility::is_assignable <
        nothrow_assignable_to &, assignable_from const &>::value, "");

    check_binary_predicate <utility::is_assignable, utility::are_assignable>
        test;
    test.check <assignable_from, assignable_to, nothrow_assignable_to>();
}

BOOST_AUTO_TEST_CASE (test_utility_are_nothrow_assignable) {
    // Check that the example types are exciting enough.
    static_assert (!utility::is_nothrow_assignable <
        assignable_from &, assignable_to const &>::value, "");
    static_assert (!utility::is_nothrow_assignable <
        assignable_to &, assignable_from const &>::value, "");
    static_assert (utility::is_nothrow_assignable <
        nothrow_assignable_to &, assignable_from const &>::value, "");

    check_binary_predicate <
        utility::is_nothrow_assignable, utility::are_nothrow_assignable> test;
    test.check <assignable_from, assignable_to, nothrow_assignable_to>();
}

BOOST_AUTO_TEST_SUITE_END()
