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

#define BOOST_TEST_MODULE test_utility_aligned_union
#include "utility/test/boost_unit_test.hpp"

#include "utility/aligned_union.hpp"

#include "meta/vector.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_aligned_union)

struct s1 {};

struct s2 {
    int i, j, k, l, m;
};

struct s3 {
    std::string s;
    s2 t;
    s1 u;
};

BOOST_AUTO_TEST_CASE (test_utility_aligned_union) {
    {
        typedef utility::aligned_union <meta::vector <s1>>::type u;
        static_assert (sizeof (u) == sizeof (s1), "");
        static_assert (
            std::alignment_of <u>::value == std::alignment_of <s1>::value, "");
    }

    {
        typedef utility::aligned_union <meta::vector <s2>>::type u;
        static_assert (sizeof (u) == sizeof (s2), "");
        static_assert (
            std::alignment_of <u>::value == std::alignment_of <s2>::value, "");
    }
    {
        typedef utility::aligned_union <meta::vector <s1, s2>>::type u;
        static_assert (sizeof (u) == sizeof (s2), "");
        static_assert (
            std::alignment_of <u>::value == std::alignment_of <s2>::value, "");
    }

    {
        typedef utility::aligned_union <meta::vector <s3>>::type u;
        static_assert (sizeof (u) == sizeof (s3), "");
        static_assert (
            std::alignment_of <u>::value == std::alignment_of <s3>::value, "");
    }
    {
        typedef utility::aligned_union <meta::vector <s1, s2, s3>>::type u;
        static_assert (sizeof (u) == sizeof (s3), "");
        static_assert (
            std::alignment_of <u>::value == std::alignment_of <s3>::value, "");
    }
}

BOOST_AUTO_TEST_SUITE_END()
