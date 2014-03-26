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

utility::thrower t;

void test_check_all_throw_points() {
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
    utility::check_all_throw_points (t, &test_check_all_throw_points);
}

BOOST_AUTO_TEST_SUITE_END()
