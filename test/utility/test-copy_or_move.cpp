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

struct base {};

struct derived : base {};

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

struct got_int {};
struct got_int_temporary {};
struct got_base {};
struct got_base_temporary {};

template <class Type> struct got_something_else {};

// Specific overloads.
got_int call (int const &) { return got_int(); }
got_int_temporary call (int &&) { return got_int_temporary(); }

got_base call (base const &) { return got_base(); }
got_base_temporary call (base &&) { return got_base_temporary(); }

/*
General overload.
This will be called, for example, when "call" is called with "int &".
This was not the intention!
*/
/*template <class Type>
    type <Type> call (Type &&) { return type <Type>(); }*/

template <class Type,
    class Enable1
        = typename utility::disable_if_same_or_derived <int, Type>::type,
    class Enable2
        = typename utility::disable_if_same_or_derived <base, Type>::type>
got_something_else <Type> call (Type &&) { return got_something_else <Type>(); }

BOOST_AUTO_TEST_CASE (test_utility_enable_if_same_or_derived) {
    {
        auto t = call (4);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_int_temporary>));
    }
    {
        int i = 4;
        auto t = call (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_int>));
    }
    {
        int const i = 4;
        auto t = call (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_int>));
    }

    {
        auto t = call (4l);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <long>>));
    }
    {
        long l = 4;
        auto t = call (l);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <long &>>));
    }
    {
        long const l = 4;
        auto t = call (l);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <long const &>>));
    }

    // Call with "base".
    {
        base b;
        auto t = call (b);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base>));
    }
    {
        auto t = call (base());
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base_temporary>));
    }
    // The same with derived.
    {
        derived d;
        auto t = call (d);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base>));
    }
    {
        auto t = call (derived());
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base_temporary>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
