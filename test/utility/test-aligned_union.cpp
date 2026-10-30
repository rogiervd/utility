// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_aligned_union
#include "boost/test/unit_test.hpp"

#include "utility/aligned_union.hpp"

#include "meta/vector.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_aligned_union)

struct s1 {};

struct s2
{
    int i, j, k, l, m;
};

struct s3
{
    std::string s;
    s2 t;
    s1 u;
};

BOOST_AUTO_TEST_CASE(test_utility_aligned_union)
{
    {
        typedef utility::aligned_union<meta::vector<s1>>::type u;
        static_assert(sizeof(u) == sizeof(s1), "");
        static_assert(
            std::alignment_of<u>::value == std::alignment_of<s1>::value, "");
    }

    {
        typedef utility::aligned_union<meta::vector<s2>>::type u;
        static_assert(sizeof(u) == sizeof(s2), "");
        static_assert(
            std::alignment_of<u>::value == std::alignment_of<s2>::value, "");
    }
    {
        typedef utility::aligned_union<meta::vector<s1, s2>>::type u;
        static_assert(sizeof(u) == sizeof(s2), "");
        static_assert(
            std::alignment_of<u>::value == std::alignment_of<s2>::value, "");
    }

    {
        typedef utility::aligned_union<meta::vector<s3>>::type u;
        static_assert(sizeof(u) == sizeof(s3), "");
        static_assert(
            std::alignment_of<u>::value == std::alignment_of<s3>::value, "");
    }
    {
        typedef utility::aligned_union<meta::vector<s1, s2, s3>>::type u;
        static_assert(sizeof(u) == sizeof(s3), "");
        static_assert(
            std::alignment_of<u>::value == std::alignment_of<s3>::value, "");
    }
}

BOOST_AUTO_TEST_SUITE_END()
