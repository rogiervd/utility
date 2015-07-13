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

#define BOOST_TEST_MODULE test_utility_is_trivially_destructible
#include "utility/test/boost_unit_test.hpp"

#include "utility/is_trivially_destructible.hpp"

#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(test_utility_is_trivially_destructible)

struct empty {};

BOOST_AUTO_TEST_CASE (test_utility_is_trivially_destructible) {
    static_assert (utility::is_trivially_destructible <int>::value, "");
    static_assert (utility::is_trivially_destructible <bool>::value, "");
    static_assert (utility::is_trivially_destructible <empty>::value, "");

    static_assert (
        !utility::is_trivially_destructible <std::string>::value, "");
    static_assert (
        !utility::is_trivially_destructible <std::vector <int>>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
