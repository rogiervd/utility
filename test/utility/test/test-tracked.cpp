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
using utility::tracked_counts;
using utility::value_construct_count;
using utility::copy_count;
using utility::move_count;
using utility::copy_assign_count;
using utility::move_assign_count;
using utility::destruct_count;
using utility::destruct_moved_count;
using utility::swap_count;

BOOST_AUTO_TEST_CASE (test_tracked) {
    tracked_registry c;

    BOOST_CHECK_EQUAL (c.counts().copy_count, 0);
    BOOST_CHECK_EQUAL (c.counts().move_count, 0);
    BOOST_CHECK_EQUAL (c.counts().copy_assign_count, 0);
    BOOST_CHECK_EQUAL (c.counts().move_assign_count, 0);
    BOOST_CHECK_EQUAL (c.counts().destruct_count, 0);

    BOOST_CHECK_EQUAL (c.counts(), tracked_counts());
    BOOST_CHECK_EQUAL (c.counts(), tracked_counts (0, 0, 0, 0, 0, 0, 0, 0));
    BOOST_CHECK (c.finished());

    auto before = c.counts();
    {
        tracked <int> object1 (c, 5);
        BOOST_CHECK (!c.finished());
        BOOST_CHECK_EQUAL (c.counts().value_construct_count, 1);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (1, 0, 0, 0, 0, 0, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), value_construct_count (1));

        // This causes an error:
        // new (&object1) tracked <int> (c, 5);

        before = c.counts();
        tracked <int> copied (object1);
        BOOST_CHECK_EQUAL (copied.content(), 5);
        BOOST_CHECK_EQUAL (c.counts().copy_count, 1);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (1, 1, 0, 0, 0, 0, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), copy_count (1));

        before = c.counts();
        tracked <int> moved (std::move (object1));
        BOOST_CHECK (object1.is_moved());
        BOOST_CHECK_EQUAL (moved.content(), 5);
        BOOST_CHECK_EQUAL (c.counts().move_count, 1);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (1, 1, 1, 0, 0, 0, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), move_count (1));


        before = c.counts();
        tracked <int> object2 (c, 7);
        tracked <int> copy_assigned (c, -1);
        tracked <int> move_assigned (c, -1);
        // Check comparison operator.
        BOOST_CHECK_EQUAL (c.counts().value_construct_count, 4);
        BOOST_CHECK (!(object2 == copy_assigned));
        BOOST_CHECK (object2 != copy_assigned);
        BOOST_CHECK (move_assigned == copy_assigned);
        BOOST_CHECK (!(move_assigned != copy_assigned));
        BOOST_CHECK_EQUAL (c.since (before), value_construct_count (3));

        before = c.counts();
        // This would generate a run-time assert to fail:
        //copy_assigned = object1;
        copy_assigned = object2;
        BOOST_CHECK_EQUAL (copy_assigned.content(), 7);
        BOOST_CHECK_EQUAL (c.counts().copy_assign_count, 1);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 1, 1, 1, 0, 0, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), copy_assign_count (1));

        before = c.counts();
        object2.content() = -7;
        move_assigned = std::move (object2);
        // This causes an error:
        // object2.content();
        BOOST_CHECK_EQUAL (move_assigned.content(), -7);
        BOOST_CHECK_EQUAL (c.counts().move_assign_count, 1);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 1, 1, 1, 1, 0, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), move_assign_count (1));

        before = c.counts();
        // std::swap should not be picked up here.
        using std::swap;
        swap (copy_assigned, move_assigned);
        BOOST_CHECK_EQUAL (copy_assigned.content(), -7);
        BOOST_CHECK_EQUAL (move_assigned.content(), 7);
        BOOST_CHECK_EQUAL (c.counts().swap_count, 1);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 1, 1, 1, 1, 1, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), swap_count (1));

        before = c.counts();
        // Convertible objects.
        tracked <long> copied_long (copied);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 2, 1, 1, 1, 1, 0, 0));
        tracked <long> moved_long (std::move (copied));
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 2, 2, 1, 1, 1, 0, 0));
        tracked <void> copied_void (copied_long);
        BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 3, 2, 1, 1, 1, 0, 0));
        BOOST_CHECK_EQUAL (c.since (before), copy_count (2) + move_count (1));

        BOOST_CHECK (!c.finished());
        before = c.counts();
    }
    BOOST_CHECK (c.finished());
    BOOST_CHECK_EQUAL (c.counts(), tracked_counts (4, 3, 2, 1, 1, 1, 6, 3));
    BOOST_CHECK_EQUAL (c.since (before),
        destruct_count (6) + destruct_moved_count (3));

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
