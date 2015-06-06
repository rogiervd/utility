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

#define BOOST_TEST_MODULE test_utility_pointee
#include "utility/test/boost_unit_test.hpp"

#include "utility/pointee.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_pointee)

using utility::pointee;

BOOST_AUTO_TEST_CASE (test_utility_pointee) {
    static_assert (std::is_same <
        pointee <int *>::type, int>::value, "");
    static_assert (std::is_same <
        pointee <int const *>::type, int const>::value, "");

    static_assert (std::is_same <
        pointee <bool *>::type, bool>::value, "");

    static_assert (std::is_same <
        pointee <std::unique_ptr <float>>::type, float>::value, "");

    static_assert (std::is_same <
        pointee <std::shared_ptr <double const>>::type, double const>::value,
        "");
}

BOOST_AUTO_TEST_SUITE_END()
