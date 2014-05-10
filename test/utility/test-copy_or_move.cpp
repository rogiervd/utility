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

#define BOOST_TEST_MODULE test_utility_copy_or_move
#include "utility/test/boost_unit_test.hpp"

#include "utility/copy_or_move.hpp"

#include "utility/returns.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_copy_or_move)

template <class Type> struct type {};

template <class Argument> inline
    type <Argument> destination (Argument &&)
{ return type <Argument> (); }

BOOST_AUTO_TEST_CASE (test_utility_copy_or_move) {
    // Lvalue reference.
    {
        int i = 4;
        auto t = destination (utility::copy_or_move <int> (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int const &>>));
    }
    // Rvalue reference.
    {
        auto t = destination (utility::copy_or_move <int> (4));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int>>));
    }
    // Const lvalue reference.
    {
        int const i = 4;
        auto t = destination (
            utility::copy_or_move <int> (static_cast <int const &&> (i)));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int const &>>));
    }
    // Const rvalue reference: becomes an lvalue reference.
    {
        int i = 4;
        auto t = destination (
            utility::copy_or_move <int> (static_cast <int const &&> (i)));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int const &>>));
    }
    // Lvalue reference to a different type: generate a temporary of type long.
    {
        int i = 4;
        auto t = destination (utility::copy_or_move <long> (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <long>>));
    }
}

BOOST_AUTO_TEST_CASE (test_utility_forward_copy_or_move) {
    {
        int i = 4;
        auto t = destination (utility::forward_copy_or_move <long, int> (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <long>>));

        int const & i2 = i;
        auto t2 = destination (
            utility::forward_copy_or_move <long, int const &> (i2));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t2), type <long>>));

        auto t3 = destination (
            utility::forward_copy_or_move <int, int const &> (i2));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t3), type <int const &>>));
    }
}

struct structure {
    int a;
    long & b;
    int const & c;

    structure (int a, long & b, int const & c)
    : a (a), b (b), c (c) {}
};

BOOST_AUTO_TEST_CASE (test_utility_forward_member) {
    int i = 5;
    long l = 9;
    structure s (2, l, i);
    structure const s2 (4, l, i);

    {
        auto t = destination (utility::forward_member <structure> (s.a));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int>>));
        BOOST_CHECK_EQUAL (utility::forward_member <structure> (s.a), 2);
    }
    {
        auto t = destination (utility::forward_member <structure &> (s.a));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int &>>));
        BOOST_CHECK_EQUAL (utility::forward_member <structure &> (s.a), 2);
    }
    {
        auto t = destination (
            utility::forward_member <structure const &> (s2.a));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int const &>>));
        BOOST_CHECK_EQUAL (
            utility::forward_member <structure const &> (s2.a), 4);
    }
    // It is currently possible to set Container to structure const & while
    // passing in a reference to another member.
    // Should that be possible?
    /*{
        auto t = destination (
            utility::forward_member <structure const &> (s.a));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int &>>));
    }*/

    {
        auto t = destination (utility::forward_member <structure &> (s.b));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <long &>>));
        BOOST_CHECK_EQUAL (utility::forward_member <structure &> (s.b), 9l);
    }
    {
        auto t = destination (
            utility::forward_member <structure const &> (s2.b));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <long &>>));
        BOOST_CHECK_EQUAL (
            utility::forward_member <structure const &> (s2.b), 9l);
    }

    {
        auto t = destination (
            utility::forward_member <structure &> (s.c));
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), type <int const &>>));
        BOOST_CHECK_EQUAL (
            utility::forward_member <structure &> (s.c), 5);
    }
}

BOOST_AUTO_TEST_SUITE_END()
