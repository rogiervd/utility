/*
Copyright 2013-2015 Rogier van Dalen.

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
        // Check comparison operator.
        BOOST_CHECK_EQUAL (c.value_construct_count(), 4);
        BOOST_CHECK (!(object2 == copy_assigned));
        BOOST_CHECK (object2 != copy_assigned);
        BOOST_CHECK (move_assigned == copy_assigned);
        BOOST_CHECK (!(move_assigned != copy_assigned));

        // This would generate a run-time assert to fail:
        //copy_assigned = object1;
        copy_assigned = object2;
        BOOST_CHECK_EQUAL (copy_assigned.content(), 7);
        BOOST_CHECK_EQUAL (c.copy_assign_count(), 1);
        c.check_counts (4, 1, 1, 1, 0, 0, 0, 0);

        object2.content() = -7;
        move_assigned = std::move (object2);
        // This causes an error:
        // object2.content();
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

void swallow (tracked <int> && t)
{ tracked <int> black_hole (std::move (t)); }

BOOST_AUTO_TEST_CASE (test_tracked_move) {
    tracked_registry c;

    {
        tracked <int> moved (c, 7);
        swallow (std::move (moved));
        // This causes an error because "moved" is not in a valid state:
        // moved.content();

        // It should be possible to assign to moved objects, like object2, and
        // make them valid again.
        tracked <int> moved2 (c, 14);
        moved = std::move (moved2);
        BOOST_CHECK_EQUAL (moved.content(), 14);
        tracked <int> copied (c, 127);
        moved2 = copied;
        BOOST_CHECK_EQUAL (moved2.content(), 127);
    }
}

BOOST_AUTO_TEST_SUITE_END()
