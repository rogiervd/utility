/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_disable_if_same
#include "utility/test/boost_unit_test.hpp"

#include "utility/disable_if_same.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_disable_if_same)

struct base {};

struct derived : base {};

template <class Type> struct type {};

struct got_int {};
struct got_int_temporary {};
struct got_base {};
struct got_base_temporary {};

template <class ... Type> struct got_something_else {};

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

BOOST_AUTO_TEST_CASE (test_utility_disable_if_same_or_derived) {
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

got_base call_multiple (base const &) { return got_base(); }
got_base_temporary call_multiple (base &&) { return got_base_temporary(); }

template <class ... Types,
    class Enable1 = typename
        utility::disable_if_variadic_same_or_derived <base, Types ...>::type>
got_something_else <Types ...> call_multiple (Types && ...)
{ return got_something_else <Types ...>(); }

BOOST_AUTO_TEST_CASE (test_utility_enable_if_variadic_same_or_derived) {
    {
        auto t = call_multiple ();
        BOOST_MPL_ASSERT ((std::is_same < decltype (t), got_something_else<>>));
    }
    {
        auto t = call_multiple (4);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <int>>));
    }
    {
        auto t = call_multiple (4, 5l);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <int, long>>));
    }

    // Call with "base".
    {
        base b;
        auto t = call_multiple (b);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base>));
    }
    {
        auto t = call_multiple (base());
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base_temporary>));
    }
    // The same with derived.
    {
        derived d;
        auto t = call_multiple (d);
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base>));
    }
    {
        auto t = call_multiple (derived());
        BOOST_MPL_ASSERT ((std::is_same <decltype (t), got_base_temporary>));
    }

    // Two parameters, one of which is base.
    {
        base b;
        auto t = call_multiple (b, 2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <base &, int>>));
    }
    {
        base b;
        auto t = call_multiple (2, b);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <int, base &>>));
    }
    // Two parameters, one of which is derived.
    {
        derived d;
        auto t = call_multiple (d, 2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <derived &, int>>));
    }
    {
        derived d;
        auto t = call_multiple (2, d);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (t), got_something_else <int, derived &>>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
