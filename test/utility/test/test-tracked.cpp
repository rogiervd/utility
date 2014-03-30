/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_test_tracked
#include "utility/test/boost_unit_test.hpp"

#include "utility/test/tracked.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_tracked)

using utility::tracked_registry;
using utility::tracked;

BOOST_AUTO_TEST_CASE (test_tracked) {
    tracked_registry c;

    assert (c.copy_count() == 0);
    assert (c.move_count() == 0);
    assert (c.copy_assign_count() == 0);
    assert (c.move_assign_count() == 0);
    assert (c.destruct_count() == 0);

    c.check_counts (0, 0, 0, 0, 0, 0, 0, 0);
    BOOST_CHECK (c.consistent());

    {
        tracked <int> object1 (c, 5);
        BOOST_CHECK (!c.consistent());
        BOOST_CHECK_EQUAL (c.value_construct_count(), 1);
        c.check_counts (1, 0, 0, 0, 0, 0, 0, 0);

        // This causes an error:
        // new (&object1) tracked <int> (c, 5);

        tracked <int> copied (object1);
        BOOST_CHECK_EQUAL (copied.content(), 5);
        BOOST_CHECK_EQUAL (c.copy_count(), 1);
        c.check_counts (1, 1, 0, 0, 0, 0, 0, 0);

        tracked <int> moved (std::move (object1));
        BOOST_CHECK_EQUAL (moved.content(), 5);
        BOOST_CHECK_EQUAL (c.move_count(), 1);
        c.check_counts (1, 1, 1, 0, 0, 0, 0, 0);


        tracked <int> object2 (c, 7);
        tracked <int> copy_assigned (c, -1);
        tracked <int> move_assigned (c, -1);
        BOOST_CHECK_EQUAL (c.value_construct_count(), 4);

        // This would generate a run-time assert to fail:
        //copy_assigned = object1;
        copy_assigned = object2;
        BOOST_CHECK_EQUAL (copy_assigned.content(), 7);
        BOOST_CHECK_EQUAL (c.copy_assign_count(), 1);
        c.check_counts (4, 1, 1, 1, 0, 0, 0, 0);

        object2.content() = -7;
        move_assigned = std::move (object2);
        BOOST_CHECK_EQUAL (move_assigned.content(), -7);
        BOOST_CHECK_EQUAL (c.move_assign_count(), 1);
        c.check_counts (4, 1, 1, 1, 1, 0, 0, 0);

        // std::swap should not be picked up here.
        using std::swap;
        swap (copy_assigned, move_assigned);
        BOOST_CHECK_EQUAL (copy_assigned.content(), -7);
        BOOST_CHECK_EQUAL (move_assigned.content(), 7);
        BOOST_CHECK_EQUAL (c.swap_count(), 1);
        c.check_counts (4, 1, 1, 1, 1, 1, 0, 0);

        // Convertible objects.
        tracked <long> copied_long (copied);
        c.check_counts (4, 2, 1, 1, 1, 1, 0, 0);
        tracked <long> moved_long (std::move (copied));
        c.check_counts (4, 2, 2, 1, 1, 1, 0, 0);
        tracked <void> copied_void (copied_long);
        c.check_counts (4, 3, 2, 1, 1, 1, 0, 0);

        BOOST_CHECK (!c.consistent());
    }
    BOOST_CHECK (c.consistent());
    c.check_counts (4, 3, 2, 1, 1, 1, 6, 3);

    // This causes an error:
    // new tracked <int> (c, 4);
}

BOOST_AUTO_TEST_SUITE_END()
