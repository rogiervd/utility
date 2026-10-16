// SPDX-FileCopyrightText: Copyright 2006, 2007, 2009, 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_with_allocator
#include <boost/test/unit_test.hpp>

#include <memory>

#include "utility/with_allocator.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_with_allocator)

struct container : utility::with_allocator<std::allocator<int>>
{
    int i;
};

BOOST_AUTO_TEST_CASE(test_utility_with_allocator)
{
    BOOST_CHECK_EQUAL(sizeof(container), sizeof(int));
}

BOOST_AUTO_TEST_SUITE_END()
