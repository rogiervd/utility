// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_test_allocator_fail_leak
#include <boost/test/unit_test.hpp>

#include "utility/test/test_allocator.hpp"

#include <vector>

BOOST_AUTO_TEST_SUITE(test_utility_test_allocator)

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_utility_test_allocator, 1)

BOOST_AUTO_TEST_CASE(test_utility_test_allocator)
{
    utility::thrower thrower;
    utility::test_allocator<std::allocator<int>> allocator(thrower);

    int * p = allocator.allocate(1);
    (void) p;
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_utility_test_allocator_size, 1)


BOOST_AUTO_TEST_CASE(test_utility_test_allocator_size)
{
    utility::thrower thrower;
    utility::test_allocator<std::allocator<int>> allocator(thrower);

    int * p = allocator.allocate(4);
    (void) p;
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(
    test_utility_test_allocator_wrong_number, 1)


BOOST_AUTO_TEST_CASE(test_utility_test_allocator_wrong_number)
{
    utility::thrower thrower;
    utility::test_allocator<std::allocator<int>> allocator(thrower);

    int * p = allocator.allocate(4);
    allocator.deallocate(p, 2);
}


BOOST_AUTO_TEST_SUITE_END()
