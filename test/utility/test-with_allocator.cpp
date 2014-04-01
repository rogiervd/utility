/*
Copyright 2006, 2007, 2009, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_with_allocator
#include "utility/test/boost_unit_test.hpp"

#include <memory>

#include "utility/with_allocator.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_with_allocator)

struct container : utility::with_allocator <std::allocator <int>> {
    int i;
};

BOOST_AUTO_TEST_CASE (test_utility_with_allocator) {
    BOOST_CHECK_EQUAL (sizeof (container), sizeof (int));
}

BOOST_AUTO_TEST_SUITE_END()
