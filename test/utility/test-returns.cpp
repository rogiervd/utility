/*
Copyright 2013 Rogier van Dalen.

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
