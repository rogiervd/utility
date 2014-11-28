/*
Copyright 2011, 2012 Rogier van Dalen.

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

#ifndef UTILITY_TEST_BOOST_UNIT_TEST_HPP_INCLUDED
#define UTILITY_TEST_BOOST_UNIT_TEST_HPP_INCLUDED

/**
There are some problems compiling the unit tests on various Boost versions.
This file hopefully abstracts away these problems.
Before including this, define BOOST_TEST_MODULE to be the name of the test.
*/

#ifndef BOOST_TEST_DYN_LINK
// Automatically defined in Boost 1.51.
#   define BOOST_TEST_DYN_LINK
#endif

/*
Clang gives a warning that Boost.Test should probably fix:
.../boost/test/floating_point_comparison.hpp:251:25: error: unused variable
    'check_is_close' [-Werror,-Wunused-variable]
*/
#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-variable"
#   include <boost/test/auto_unit_test.hpp>
#   pragma clang diagnostic pop
#else
#   include <boost/test/auto_unit_test.hpp>
#endif

#endif // UTILITY_TEST_BOOST_UNIT_TEST_HPP_INCLUDED
