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

#define BOOST_TEST_MODULE test_suite_utility_test_allocator
#include "utility/test/boost_unit_test.hpp"

#include "utility/test/test_allocator.hpp"

#include <vector>

BOOST_AUTO_TEST_SUITE(test_suite_utility_test_allocator)

BOOST_AUTO_TEST_CASE (test_utility_test_allocator) {
    utility::thrower thrower;
    utility::test_allocator <std::allocator <int>> allocator (thrower);

    {
        utility::test_allocator <std::allocator <int>> allocator2 (thrower);
        BOOST_CHECK (allocator != allocator2);
        BOOST_CHECK (!(allocator == allocator2));
    }

    std::vector <int, utility::test_allocator <std::allocator <int>>>
        v (allocator);
    BOOST_CHECK_EQUAL (thrower.count(), 0);
    int i;
    for (i = 0; i != 10; ++ i)
        v.push_back (i);
    BOOST_CHECK_EQUAL (v.front(), 0);
    size_t allocation_count_10 = thrower.count();
    BOOST_CHECK (allocation_count_10 != 0);

    for (; i != 100; ++ i)
        v.push_back (i);
    size_t allocation_count_100 = thrower.count();
    BOOST_CHECK (allocation_count_100 != allocation_count_10);
    size_t difference_100 = allocation_count_100 - allocation_count_10;

    for (; i != 1000; ++ i)
        v.push_back (i);
    BOOST_CHECK_EQUAL (v.back(), 999);
    size_t allocation_count_1000 = thrower.count();
    size_t difference_1000 = allocation_count_1000 - allocation_count_100;
    // Allocating the extra 900 should cause in the same order of allocations
    // as allocating the extra 90.
    BOOST_CHECK (difference_100 / 2 < difference_1000);
    BOOST_CHECK (difference_1000 < difference_100 * 2);

    for (; i != 10000; ++ i)
        v.push_back (i);
    size_t allocation_count_10000 = thrower.count();
    size_t difference_10000 = allocation_count_10000 - allocation_count_1000;
    // Allocating the extra 900 should cause in the same order of allocations
    // as allocating the extra 90.
    BOOST_CHECK (difference_1000 / 2 < difference_10000);
    BOOST_CHECK (difference_10000 < difference_1000 * 2);

    // Playground for creating error messages:
    int * p = allocator.allocate (4);
    allocator.deallocate (p, 4);
}

BOOST_AUTO_TEST_SUITE_END()
