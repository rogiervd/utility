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

#define BOOST_TEST_MODULE test_utility_enable_if_compiles
#include "utility/test/boost_unit_test.hpp"

#include "utility/enable_if_compiles.hpp"

#include <vector>

/**
Return the object times two.

This is implemented either as <c>2*object</c> if it is an \c int,
or <c>object.twice()</c>.
*/
int times_two (int object)
{ return 2 * object; }

template <class Type>
    typename utility::enable_if_compiles <
        decltype (std::declval <Type const &>().twice()), Type>::type
    times_two (Type const & object)
{ return object.twice(); }

/**
Evaluate to \c true if \a Type has a member \c begin().
*/
template <class Type, class Enable = void> struct has_begin
: std::false_type {};

template <class Type>
    struct has_begin <Type, typename utility::enable_if_compiles <
        decltype (std::declval <Type>().begin())>::type>
: std::true_type {};

BOOST_AUTO_TEST_SUITE(test_utility_enable_if_compiles)

struct twiceable {
    int i;
    twiceable (int i) : i (i) {}
    twiceable twice() const { return twiceable (2*i); }
};

BOOST_AUTO_TEST_CASE (enable_if_compiles) {
    BOOST_CHECK_EQUAL (times_two (5), 10);
    twiceable a (27);
    BOOST_CHECK_EQUAL (times_two (a).i, 54);

    static_assert (!has_begin <int>::value, "");
    static_assert (has_begin <std::vector <int>>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
