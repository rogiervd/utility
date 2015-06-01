/*
Copyright 2014 Rogier van Dalen.

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
