// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_test_tracked
#include "boost/test/unit_test.hpp"

#include "utility/test/tracked.hpp"

#include <type_traits>

#include "boost/mpl/assert.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_tracked)

using utility::tracked;
using utility::tracked_registry;

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_tracked, 2)


BOOST_AUTO_TEST_CASE(test_tracked)
{
    tracked_registry r;

    char memory[sizeof(tracked<int>)];
    tracked<int> * t = reinterpret_cast<tracked<int> *>(&memory[0]);

    new (t) tracked<int>(r, 5);

    // Don't destruct.
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_tracked_2, 2)

BOOST_AUTO_TEST_CASE(test_tracked_2)
{
    tracked_registry r;

    char memory[sizeof(tracked<int>)];
    tracked<int> * t = reinterpret_cast<tracked<int> *>(&memory[0]);

    new (t) tracked<int>(r, 5);
    // New object where one already existed.
    new (t) tracked<int>(r, 7);

    t->~tracked<int>();
}


BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_tracked_3, 3)

BOOST_AUTO_TEST_CASE(test_tracked_3)
{
    tracked_registry r;

    char memory[sizeof(tracked<int>)];
    tracked<int> * t = reinterpret_cast<tracked<int> *>(&memory[0]);

    new (t) tracked<int>(r, 5);
    t->~tracked<int>();
    // Second destruction.
    t->~tracked<int>();
}
BOOST_AUTO_TEST_SUITE_END()
