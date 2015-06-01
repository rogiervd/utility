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

#define BOOST_TEST_MODULE test_suite_utility_test_thrower
#include "utility/test/boost_unit_test.hpp"

#include "utility/test/thrower.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_thrower)

BOOST_AUTO_TEST_CASE (test_utility_thrower) {
    utility::thrower t;
    t.throw_point();
    t.throw_point();

    t.set_cycle (2);
    t.reset();

    t.throw_point();
    BOOST_CHECK_THROW (t.throw_point(), std::exception);
}

void test_check_all_throw_points (utility::thrower & t) {
    static int time = 0;

    // The first time this shouldn't throw at all.

    int position = 1;
    try {
        // The second time it should throw here.
        t.throw_point();
        position = 2;
        // The third time it should throw here.
        t.throw_point();
    } catch (...) {
        BOOST_CHECK_EQUAL (position, time);
        ++ time;
        throw;
    }

    BOOST_CHECK_EQUAL (time, 0);
    ++ time;
}

BOOST_AUTO_TEST_CASE (test_utility_check_all_throw_points) {
    utility::check_all_throw_points (&test_check_all_throw_points);
}

BOOST_AUTO_TEST_SUITE_END()
