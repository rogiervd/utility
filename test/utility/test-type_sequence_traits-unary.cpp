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

#define BOOST_TEST_MODULE test_utility_type_sequence_traits_unary
#include "utility/test/boost_unit_test.hpp"

#include "utility/type_sequence_traits.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_type_sequence_traits_unary)

/**
Check that a unary predicate on a sequence corresponds to one on single types.
*/
template <template <class> class SinglePredicate,
    template <class> class AllPredicate,
    class Type1, class Type2, class Type3>
void check()
{
    static_assert (AllPredicate <meta::vector<>>::value, "");
    static_assert (AllPredicate <meta::vector <Type1>>::value ==
        SinglePredicate <Type1>::value, "");
    static_assert (AllPredicate <meta::vector <Type1, Type2>>::value ==
        (SinglePredicate <Type1>::value && SinglePredicate <Type2>::value), "");
    static_assert (AllPredicate <meta::vector <Type1, Type2, Type3>>::value ==
        (SinglePredicate <Type1>::value && SinglePredicate <Type2>::value
            && SinglePredicate <Type3>::value), "");
}

struct default_constructible {
    std::shared_ptr <int> a;

    default_constructible() : a (std::make_shared <int> (6)) {}
};

struct nothrow_default_constructible {
    int a;

    nothrow_default_constructible() noexcept : a (4) {}
};

struct not_default_constructible {
    std::shared_ptr <int> a;

    not_default_constructible (int i) : a (std::make_shared <int> (i)) {}
};

BOOST_AUTO_TEST_CASE (test_utility_are_default_constructible) {
    // Check that the example types are interesting enough.
    static_assert (utility::is_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (utility::is_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_default_constructible <
        not_default_constructible>::value, "");

    check <utility::is_default_constructible,
        utility::are_default_constructible,
        int, int &, void>();
    check <utility::is_default_constructible,
        utility::are_default_constructible,
        int, default_constructible, not_default_constructible>();
    check <utility::is_default_constructible,
        utility::are_default_constructible,
        default_constructible, not_default_constructible,
        nothrow_default_constructible>();
}

BOOST_AUTO_TEST_CASE (test_utility_are_nothrow_default_constructible) {
    // Check that the example types are interesting enough.
    static_assert (utility::is_nothrow_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        not_default_constructible>::value, "");

    check <utility::is_nothrow_default_constructible,
        utility::are_nothrow_default_constructible,
        int, int &, void>();
    check <utility::is_nothrow_default_constructible,
        utility::are_nothrow_default_constructible,
        int, default_constructible, not_default_constructible>();
    check <utility::is_nothrow_default_constructible,
        utility::are_nothrow_default_constructible,
        default_constructible, not_default_constructible,
        nothrow_default_constructible>();
}

BOOST_AUTO_TEST_SUITE_END()
