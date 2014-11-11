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

#define BOOST_TEST_MODULE test_utility_is_default_constructible
#include "utility/test/boost_unit_test.hpp"

#include "utility/is_default_constructible.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_is_default_constructible)

struct default_constructible {
    std::shared_ptr <int> a;

    default_constructible() : a (std::make_shared <int> (6)) {}
};

struct nothrow_default_constructible {
    nothrow_default_constructible() noexcept {}
};

struct not_default_constructible {
    not_default_constructible() = delete;
};

struct abstract {
    virtual void method() = 0;
};

template <template <class> class Predicate> void check_with() {
    static_assert (!Predicate <void>::value, "");
    static_assert (Predicate <int>::value, "");
    static_assert (Predicate <int const>::value, "");
    static_assert (Predicate <void *>::value, "");
    static_assert (!Predicate <int &>::value, "");
    static_assert (!Predicate <int const &>::value, "");
    static_assert (!Predicate <int &&>::value, "");
    static_assert (!Predicate <int const &&>::value, "");

    static_assert (Predicate <int [5]>::value, "");

    typedef int function_type (float);
    function_type f;

    static_assert (Predicate <int (float)>::value, "");
    static_assert (!Predicate <int (&) (float)>::value, "");
    static_assert (Predicate <int (*) (float)>::value, "");

    static_assert (!Predicate <abstract>::value, "");
}

BOOST_AUTO_TEST_CASE (test_utility_is_default_constructible) {
    static_assert (utility::is_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (utility::is_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_default_constructible <
        not_default_constructible>::value, "");

    static_assert (utility::is_default_constructible <
        nothrow_default_constructible [3]>::value, "");
    static_assert (utility::is_default_constructible <
        default_constructible[5]>::value, "");
    static_assert (!utility::is_default_constructible <
        not_default_constructible[8]>::value, "");

    check_with <utility::is_default_constructible> ();
}
/*
BOOST_AUTO_TEST_CASE (test_utility_is_nothrow_default_constructible) {
    static_assert (utility::is_nothrow_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        not_default_constructible>::value, "");

    static_assert (utility::is_nothrow_default_constructible <
        nothrow_default_constructible [12]>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        default_constructible [12]>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        not_default_constructible [12]>::value, "");

    check_with <utility::is_nothrow_default_constructible> ();
}*/

BOOST_AUTO_TEST_SUITE_END()
