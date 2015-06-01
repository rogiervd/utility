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

#define BOOST_TEST_MODULE test_utility_make_unique
#include "utility/test/boost_unit_test.hpp"

#include "utility/make_unique.hpp"

#include <type_traits>

BOOST_AUTO_TEST_SUITE(test_utility_make_unique)

bool exists = false;

struct type {
    type() { exists = true; }
    ~type() { exists = false; }
};

BOOST_AUTO_TEST_CASE (test_utility_make_unique) {
    auto p = utility::make_unique <type>();
    static_assert (std::is_same <decltype (p), std::unique_ptr <type>>::value,
        "");
    BOOST_CHECK (exists);
    p.reset();
    BOOST_CHECK (!exists);
}

BOOST_AUTO_TEST_SUITE_END()
