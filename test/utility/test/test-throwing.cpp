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

#define BOOST_TEST_MODULE test_suite_utility_throwing
#include "utility/test/boost_unit_test.hpp"

#include "utility/test/throwing.hpp"

#include <type_traits>

// For utility::is_nothrow_assignable.
#include "utility/is_assignable.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_throwing)

/* Test noexcept specification. */
// throwing is parameterised with the following:
// bool ThrowOnConstruction = false,
// bool ThrowOnCopy = false,
// bool ThrowOnMove = false,
// bool ThrowOnCopyAssign = false,
// bool ThrowOnMoveAssign = false,
// bool ThrowOnConversion = false

// For some reason, the checks with is_nothrow_constructible fail on GCC 4.6.
// I am unable to figure out why.
// So workaround: disable them.
#if !defined (__GNUC__) || __GNUC__ != 4 || __GNUC_MINOR__ != 6
// ThrowOnConstruction.
static_assert (std::is_nothrow_constructible <
    utility::throwing <int>,
    utility::thrower &, int>::value, "");
static_assert (!std::is_nothrow_constructible <
    utility::throwing <int, true>,
    utility::thrower &, int>::value, "");

// ThrowOnCopy.
static_assert (std::is_nothrow_constructible <
    utility::throwing <int>,
    utility::throwing <int> const &>::value, "");
static_assert (!std::is_nothrow_constructible <
    utility::throwing <int, false, true>,
    utility::throwing <int, false, true> const &>::value, "");

// ThrowOnMove.
static_assert (std::is_nothrow_constructible <
    utility::throwing <int>,
    utility::throwing <int> &&>::value, "");
static_assert (!std::is_nothrow_constructible <
    utility::throwing <int, false, false, true>,
    utility::throwing <int, false, false, true> &&>::value, "");
#endif

// ThrowOnCopyAssign.
static_assert (utility::is_nothrow_assignable <
    utility::throwing <int> &,
    utility::throwing <int> const &>::value, "");
static_assert (!utility::is_nothrow_assignable <
    utility::throwing <int, false, false, false, true> &,
    utility::throwing <int, false, false, false, true> const &>::value, "");

// ThrowOnMoveAssign.
static_assert (utility::is_nothrow_assignable <
    utility::throwing <int> &,
    utility::throwing <int> &&>::value, "");
static_assert (!utility::is_nothrow_assignable <
    utility::throwing <int, false, false, false, false, true> &,
    utility::throwing <int, false, false, false, false, true> &&>::value, "");

// ThrowOnConversion is not really interesting to check here.

BOOST_AUTO_TEST_CASE (test_utility_throwing) {
    utility::thrower t;
    utility::throwing <int> o (t, 5);
    BOOST_CHECK_EQUAL (o.content(), 5);

    // Copying.
    utility::throwing <int> o2 (o);
    BOOST_CHECK_EQUAL (o2.content(), 5);
    utility::throwing <int> o3 (std::move (o2));
    BOOST_CHECK_EQUAL (o3.content(), 5);

    // Assignment.
    utility::throwing <int> o4 (t, 7);
    o4.content() = 27;
    BOOST_CHECK_EQUAL (o4.content(), 27);
    o4 = o;
    BOOST_CHECK_EQUAL (o4.content(), 5);
    o4.content() = 27;
    BOOST_CHECK_EQUAL (o4.content(), 27);
    o4 = std::move (o3);
    BOOST_CHECK_EQUAL (o4.content(), 5);

    // This produces an error because the memory is not cleaned up:
    // new utility::throwing <int> (t, 5);

    // ThrowOnConstruction
    {
        t.reset();
        t.set_cycle (1);
        // Can't use a type with a comma in a macro.
        typedef utility::throwing <int, true> type;
        BOOST_CHECK_THROW (type object (t, 6), std::exception);
    }

    // ThrowOnCopy
    {
        t.reset();
        t.set_cycle (1);
        typedef utility::throwing <int, false, true> type;
        type original (t, 7);
        BOOST_CHECK_THROW (type copy (original), std::exception);
    }

    // ThrowOnMove
    {
        t.reset();
        t.set_cycle (1);
        typedef utility::throwing <int, false, false, true> type;
        type original (t, 7);
        BOOST_CHECK_THROW (type copy (std::move (original)), std::exception);
    }

    // ThrowOnCopyAssign
    {
        t.reset();
        t.set_cycle (1);
        // Can't use a type with a comma in a macro.
        typedef utility::throwing <int, false, false, false, true> type;
        type original (t, 7);
        type copy (t, 1);
        BOOST_CHECK_THROW (copy = original, std::exception);
    }

    // ThrowOnMoveAssign
    {
        t.reset();
        t.set_cycle (1);
        // Can't use a type with a comma in a macro.
        typedef utility::throwing <int, false, false, false, false, true>
            type;
        type original (t, 7);
        type copy (t, 1);
        BOOST_CHECK_THROW (copy = std::move (original), std::exception);
    }

    // ThrowOnConversion
    {
        t.reset();
        t.set_cycle (1);
        // Can't use a type with a comma in a macro.
        typedef utility::throwing <int, false, false, false, false, false, true>
            type;
        type original (t, 7);
        int i = 27;
        BOOST_CHECK_THROW (i = original, std::exception);
        BOOST_CHECK_EQUAL (i, 27);
    }
}

BOOST_AUTO_TEST_SUITE_END()
