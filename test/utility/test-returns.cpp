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

// Standard case.
template <class Type> inline auto return_decayed (Type c)
RETURNS (c);

// More complicated case that seems to expose a bug on GCC 4.6 when the
// argument to return is in parentheses.
template <class Type> struct construct_default_helper {
    Type operator() () const { return Type(); }
};

template <class Type> inline auto construct_default()
RETURNS (construct_default_helper <Type>()());

struct s {
    auto get_int() const RETURNS (3);

    auto get_float() const RETURNS (5.6f);
};

BOOST_AUTO_TEST_SUITE(test_utility_returns)

BOOST_AUTO_TEST_CASE (test_utility_returns) {
    BOOST_MPL_ASSERT ((std::is_same <decltype (return_decayed (1)), int>));

    BOOST_MPL_ASSERT ((std::is_same <
        decltype (construct_default <float>()), float>));
    BOOST_CHECK_EQUAL (construct_default <float>(), 0);

    s o;
    BOOST_MPL_ASSERT ((std::is_same <decltype (o.get_int()), int>));
    BOOST_CHECK_EQUAL (o.get_int(), 3);
    BOOST_MPL_ASSERT ((std::is_same <decltype (o.get_float()), float>));
    BOOST_CHECK_EQUAL (o.get_float(), 5.6f);
}

BOOST_AUTO_TEST_SUITE_END()
