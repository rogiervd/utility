/*
Copyright 2006, 2007, 2009, 2014 Rogier van Dalen.

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
