// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_pointee
#include "boost/test/unit_test.hpp"

#include "utility/pointee.hpp"

#include <memory>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(test_utility_pointee)

using utility::pointee;

BOOST_AUTO_TEST_CASE(test_utility_pointee)
{
    static_assert(std::is_same_v<pointee<int *>::type, int>);
    static_assert(std::is_same_v<pointee<int const *>::type, int const>);

    static_assert(std::is_same_v<pointee<bool *>::type, bool>);

    static_assert(std::is_same_v<pointee<std::unique_ptr<float>>::type, float>);

    static_assert(std::is_same_v<
                  pointee<std::shared_ptr<double const>>::type, double const>);
}

BOOST_AUTO_TEST_SUITE_END()
