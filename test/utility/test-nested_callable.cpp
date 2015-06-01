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

#define BOOST_TEST_MODULE test_utility_nested_callable
#include "utility/test/boost_unit_test.hpp"

#include "utility/nested_callable.hpp"

#include <boost/mpl/assert.hpp>
#include <boost/mpl/placeholders.hpp>

#include "utility/returns.hpp"

/* For result_of. */
struct a {};
struct b {};
struct c {};

struct function1 {
    a operator() (a, a);
    b operator() (a, b);
    c operator() (b, b);
};

struct function2 {
    c operator() (c, a);
    b operator() (c, b);
    a operator() (b, b);
};

/*
For all/any.

Imagine function objects are tagged by their member "apply" deriving from
certain base classes.
In this example, some operations derived from "unimplemented"; some from
"approximate".
Only if all operations in a nested call are implemented is the whole call
implemented.
This is where "all" is useful.
If any operation in the nested call is approximate, then the whole call is.
This is where "any" is useful.
*/
struct unimplemented {};
struct approximate {};

template <class Operation> struct is_apply_implemented;
template <class Function, class ... Arguments>
    struct is_apply_implemented <Function (Arguments ...)>
: boost::mpl::not_ <std::is_base_of <unimplemented,
    typename Function::template apply <Arguments ...>>> {};

template <class Operation> struct is_apply_approximate;
template <class Function, class ... Arguments>
    struct is_apply_approximate <Function (Arguments ...)>
: std::is_base_of <approximate,
    typename Function::template apply <Arguments ...>> {};

template <class Expression> struct is_implemented
: nested_callable::all <is_apply_implemented <boost::mpl::_1>, Expression> {};

template <class Expression> struct is_approximate
: nested_callable::any <is_apply_approximate <boost::mpl::_1>, Expression> {};


namespace operation {

    // plus: for implemented.
    template <class ...  Arguments> struct plus
    : unimplemented {};

    template <> struct plus <int, int>
    { int operator() (int a, int b) const {return a + b; } };

    template <> struct plus <float, float>
    { float operator() (float a, float b) const {return a + b; } };
    template <> struct plus <double, float>
    { double operator() (double a, float b) const {return a + b; } };
    template <> struct plus <float, double>
    { double operator() (float a, double b) const {return a + b; } };
    template <> struct plus <double, double>
    { double operator() (double a, double b) const {return a + b; } };

    // times: for approximate.
    template <class ...  Arguments> struct times {};

    template <> struct times <int, int>
    { int operator() (int a, int b) const {return a + b; } };

    template <> struct times <int, float> : approximate
    { float operator() (int a, float b) const {return a + b; } };

    template <> struct times <float, int> : approximate
    { float operator() (float a, int b) const {return a + b; } };

    template <> struct times <float, float> : approximate
    { float operator() (float a, float b) const {return a + b; } };

    // make_approximate: pretend to make operation approximate.
    template <class ... Arguments> struct make_approximate : unimplemented {};

    template <class Operation> struct make_approximate <Operation>
    : approximate
    { Operation operator() (Operation const & o) { return o; } };

} // namespace operation


namespace callable {

    struct plus {
        template <class ...  Arguments> struct apply
        : operation::plus <Arguments ...> {};

        template <class ...  Arguments>
            auto operator() (Arguments const & ... arguments)
        RETURNS (apply <Arguments ...>() (arguments ...));
    };

    struct times {
        template <class ...  Arguments> struct apply
        : operation::times <Arguments ...> {};

        template <class ...  Arguments>
            auto operator() (Arguments const & ... arguments)
        RETURNS (apply <Arguments ...>() (arguments ...));
    };

    struct make_approximate {
        template <class ...  Arguments> struct apply
        : operation::make_approximate <Arguments ...> {};

        template <class ...  Arguments>
            auto operator() (Arguments const & ... arguments)
        RETURNS (apply <Arguments ...>() (arguments ...));
    };

} // namespace callable

BOOST_AUTO_TEST_SUITE(test_utility_nested_callable)

BOOST_AUTO_TEST_CASE (test_utility_nested_callable_result_of) {
    using nested_callable::result_of;
    BOOST_MPL_ASSERT ((std::is_same <typename result_of <a>::type, a>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1>::type, function1>));

    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (a, a)>::type, a>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (a, b)>::type, b>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (b, b)>::type, c>));

    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (a, function2 (b, b))>::type, a>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (a, function2 (c, b))>::type, b>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (function2 (c, b), b)>::type, c>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename result_of <function1 (function2 (b, b), function2 (c, b))
        >::type, b>));
}

BOOST_AUTO_TEST_CASE (test_utility_nested_callable_all) {
    // Test the example implementation.
    BOOST_MPL_ASSERT ((is_apply_implemented <callable::plus (int, int)>));
    BOOST_MPL_ASSERT ((is_apply_implemented <callable::plus (float, float)>));
    BOOST_MPL_ASSERT_NOT ((is_apply_implemented <callable::plus (float, int)>));
    BOOST_MPL_ASSERT_NOT ((is_apply_implemented <callable::plus ()>));
    BOOST_MPL_ASSERT_NOT ((is_apply_implemented <callable::plus (int)>));
    BOOST_MPL_ASSERT_NOT ((
        is_apply_implemented <callable::plus (int, int, int)>));

    // Test the same cases as above.
    BOOST_MPL_ASSERT ((is_implemented <callable::plus (int, int)>));
    BOOST_MPL_ASSERT ((is_implemented <callable::plus (float, float)>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <callable::plus (float, int)>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <callable::plus ()>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <callable::plus (int)>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <callable::plus (int, int, int)>));

    // Test nested cases.
    BOOST_MPL_ASSERT ((is_implemented <
        callable::plus (int, callable::plus (int, int))>));
    BOOST_MPL_ASSERT ((is_implemented <
        callable::plus (float, callable::plus (float, float))>));
    BOOST_MPL_ASSERT ((is_implemented <
        callable::plus (int, callable::plus (
            callable::plus (int, int), int))>));
    BOOST_MPL_ASSERT ((is_implemented <
        callable::plus (float, callable::plus (
            callable::plus (float, float), float))>));

    BOOST_MPL_ASSERT_NOT ((is_implemented <
        callable::plus (int, callable::plus (float, int))>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <
        callable::plus (float, callable::plus (int, int))>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <
        callable::plus (callable::plus (float, int),
            callable::plus (float, int))>));
    BOOST_MPL_ASSERT_NOT ((is_implemented <
        callable::plus (callable::plus (float, int),
            callable::plus (int, int))>));
}

BOOST_AUTO_TEST_CASE (test_utility_nested_callable_any) {
    // Test the example implementation.
    BOOST_MPL_ASSERT_NOT ((
        is_apply_approximate <callable::times (int, int)>));
    BOOST_MPL_ASSERT ((
        is_apply_approximate <callable::times (float, int)>));
    BOOST_MPL_ASSERT ((
        is_apply_approximate <callable::times (int, float)>));
    BOOST_MPL_ASSERT ((
        is_apply_approximate <callable::times (float, float)>));

    // Test the same cases as above.
    BOOST_MPL_ASSERT_NOT ((
        is_approximate <callable::times (int, int)>));
    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (float, int)>));
    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (int, float)>));
    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (float, float)>));

    // Test nested cases.
    BOOST_MPL_ASSERT_NOT ((
        is_approximate <callable::times (int, callable::times (int, int))>));
    // plus is not marked as approximate.
    BOOST_MPL_ASSERT_NOT ((
        is_approximate <callable::times (int, callable::plus (int, int))>));
    // The function pointer type for make_approximate (times).
    // Not great, but this does work.
    typedef callable::make_approximate (* approximate_times) (callable::times);
    BOOST_MPL_ASSERT ((
        is_approximate <approximate_times (int, callable::times (int, int))>));
    typedef callable::make_approximate (* approximate_plus) (callable::plus);
    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (int, approximate_plus (int, int))>));

    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (callable::times (float, int), int)>));
    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (int, callable::times (float, int))>));
    BOOST_MPL_ASSERT ((
        is_approximate <callable::times (
            callable::times (float, int), callable::times (float, int))>));
}

BOOST_AUTO_TEST_SUITE_END()
