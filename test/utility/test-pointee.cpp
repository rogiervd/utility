// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_pointee
#include "boost/test/unit_test.hpp"

#include "utility/pointee.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_pointee)

using utility::pointee;

BOOST_AUTO_TEST_CASE(test_utility_pointee)
{
    static_assert(std::is_same<pointee<int *>::type, int>::value, "");
    static_assert(
        std::is_same<pointee<int const *>::type, int const>::value, "");

    static_assert(std::is_same<pointee<bool *>::type, bool>::value, "");

    static_assert(
        std::is_same<pointee<std::unique_ptr<float>>::type, float>::value, "");

    static_assert(
        std::is_same<
            pointee<std::shared_ptr<double const>>::type, double const>::value,
        "");
}

BOOST_AUTO_TEST_SUITE_END()
