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

#define BOOST_TEST_MODULE fail_utility_test_tracked_3
#include "utility/test/boost_unit_test.hpp"

#include "utility/test/tracked.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_tracked)

using utility::tracked_registry;
using utility::tracked;

BOOST_AUTO_TEST_CASE (test_tracked) {
    tracked_registry r;

    char memory [sizeof (tracked <int>)];
    tracked <int> * t = reinterpret_cast <tracked <int> *> (&memory [0]);

    new (t) tracked <int> (r, 5);
    t->~tracked <int>();
    // Second destruction.
    t->~tracked <int>();
}

BOOST_AUTO_TEST_SUITE_END()
