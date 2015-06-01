/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_is_assignable
#include "utility/test/boost_unit_test.hpp"

#include "utility/is_assignable.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_is_assignable)

struct unassignable {
    unassignable & operator = (unassignable const &) = delete;
};

struct assignable {
    template <class Type>
    assignable & operator = (Type const &) { return *this; }
};

struct nothrow_assignable {
    template <class Type>
    nothrow_assignable & operator = (Type const &) noexcept { return *this; }
};

BOOST_AUTO_TEST_CASE (test_utility_is_assignable) {
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, int>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, void>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, unassignable>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, assignable>));

    // 6 = 7 does not compile.
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <int, int>));
    BOOST_MPL_ASSERT ((utility::is_assignable <int &, int>));

    BOOST_MPL_ASSERT ((utility::is_assignable <int &, int const &>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <int const &, int const &>));

    // This does compile.
    assignable() = 5;

    BOOST_MPL_ASSERT_NOT ((
        utility::is_assignable <unassignable &, unassignable>));
    BOOST_MPL_ASSERT ((utility::is_assignable <assignable, int>));
    BOOST_MPL_ASSERT ((utility::is_assignable <assignable &, int>));
    BOOST_MPL_ASSERT ((utility::is_assignable <assignable &, unassignable>));
}

BOOST_AUTO_TEST_CASE (test_utility_is_nothrow_assignable) {
    // If something is not assignable, it is also not nothrow-assignable.
    BOOST_MPL_ASSERT_NOT ((utility::is_nothrow_assignable <void, int>));
    BOOST_MPL_ASSERT_NOT ((utility::is_nothrow_assignable <void, void>));

    // nothrow_assignable::operator= is not declared noexcept.
    BOOST_MPL_ASSERT_NOT ((utility::is_nothrow_assignable <assignable, int>));
    BOOST_MPL_ASSERT_NOT ((utility::is_nothrow_assignable <assignable &, int>));
    BOOST_MPL_ASSERT_NOT ((
        utility::is_nothrow_assignable <assignable &, unassignable>));

    BOOST_MPL_ASSERT ((utility::is_nothrow_assignable <int &, double>));

    // nothrow_assignable::operator= is declared noexcept.
    BOOST_MPL_ASSERT ((
        utility::is_nothrow_assignable <nothrow_assignable, int>));
    BOOST_MPL_ASSERT ((
        utility::is_nothrow_assignable <nothrow_assignable &, int>));
    BOOST_MPL_ASSERT ((
        utility::is_nothrow_assignable <nothrow_assignable &, unassignable>));
}

BOOST_AUTO_TEST_SUITE_END()
