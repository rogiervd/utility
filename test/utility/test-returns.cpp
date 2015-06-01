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

#define BOOST_TEST_MODULE test_utility_returns
#include "utility/test/boost_unit_test.hpp"

#include "utility/returns.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

float f;

template <class Type> inline auto return_const_ref (Type const & c)
RETURNS (c);

template <class Type> inline auto return_ref (Type & c)
RETURNS (c);

template <class Type> inline auto return_decayed (Type const & c)
RETURNS_DECAYED (c);

template <class Type> inline auto construct_default() RETURNS (Type());

struct s {
    auto get_int() const RETURNS (3);

private:
    auto dummy1() RETURNS (71);

    static double d;

public:
    auto get_float() const RETURNS (5.6f);

    auto get_double_ref() RETURNS (d);
    auto get_double() const RETURNS_DECAYED (d);
};

double s::d = 81.25;

BOOST_AUTO_TEST_SUITE(test_utility_returns)

BOOST_AUTO_TEST_CASE (test_utility_returns) {
    {
        int i = 7;
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (return_const_ref (i)), int const &>));
        BOOST_CHECK_EQUAL (return_const_ref (i), 7);

        BOOST_MPL_ASSERT ((std::is_same <decltype (return_ref (i)), int &>));
        BOOST_CHECK_EQUAL (return_ref (i), 7);

        BOOST_MPL_ASSERT ((std::is_same <decltype (return_decayed (i)), int>));
        BOOST_CHECK_EQUAL (return_decayed (i), 7);
    }

    BOOST_MPL_ASSERT ((std::is_same <
        decltype (construct_default <float>()), float>));
    BOOST_CHECK_EQUAL (construct_default <float>(), 0);

    s o;
    BOOST_MPL_ASSERT ((std::is_same <decltype (o.get_int()), int>));
    BOOST_CHECK_EQUAL (o.get_int(), 3);
    BOOST_MPL_ASSERT ((std::is_same <decltype (o.get_float()), float>));
    BOOST_CHECK_EQUAL (o.get_float(), 5.6f);

    BOOST_MPL_ASSERT ((std::is_same <decltype (o.get_double()), double>));
    BOOST_CHECK_EQUAL (o.get_double(), 81.25);
    BOOST_MPL_ASSERT ((std::is_same <decltype (o.get_double_ref()), double &>));
    BOOST_CHECK_EQUAL (o.get_double_ref(), 81.25);
}

BOOST_AUTO_TEST_SUITE_END()
