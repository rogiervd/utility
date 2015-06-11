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

#define BOOST_TEST_MODULE test_suite_utility_test_allocator_fail_leak_3
#include "utility/test/boost_unit_test.hpp"

#include "utility/test/test_allocator.hpp"

#include <vector>

BOOST_AUTO_TEST_SUITE(test_suite_utility_test_allocator)

BOOST_AUTO_TEST_CASE (test_utility_test_allocator) {
    utility::thrower thrower;
    utility::test_allocator <std::allocator <int>> allocator (thrower);

    int * p = allocator.allocate (4);
    allocator.deallocate (p, 2);
}

BOOST_AUTO_TEST_SUITE_END()
