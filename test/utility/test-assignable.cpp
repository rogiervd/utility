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

#define BOOST_TEST_MODULE test_utility_assignable
#include "utility/test/boost_unit_test.hpp"

#include "utility/assignable.hpp"

#include <iostream>

#include <boost/mpl/assert.hpp>

#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

using utility::assignable;

template <class Target, class ... Sources>
    struct disable_if_same_or_derived
{ typedef void type; };

template <class Target, class Source>
    struct disable_if_same_or_derived <Target, Source>
: std::enable_if <
    !std::is_same <Target, typename std::decay <Source>::type>::value &&
    !std::is_base_of <Target,
        typename std::remove_reference <Source>::type>::value> {};

template <class Content> struct non_assignable {
    Content i;

    template <class ... Arguments, class Enable = typename
        disable_if_same_or_derived <non_assignable, Arguments ...>::type>
    non_assignable (Arguments && ... arguments)
    : i (std::forward <Arguments> (arguments) ...) {}

    non_assignable (non_assignable const &) = default;
    non_assignable (non_assignable &&) = default;

    non_assignable & operator = (non_assignable const &) = delete;
    non_assignable & operator = (non_assignable &&) = delete;
};

BOOST_AUTO_TEST_SUITE(test_utility_assignable)

BOOST_AUTO_TEST_CASE (test_utility_assignable_simple) {
    utility::assignable <non_assignable <int>> a (4);

    BOOST_CHECK_EQUAL (a.content().i, 4);

    a = non_assignable <int> (7);

    BOOST_CHECK_EQUAL (a.content().i, 7);

    utility::assignable <non_assignable <int>> b (27);

    a = std::move (b);
    BOOST_CHECK_EQUAL (a.content().i, 27);
}

template <bool t1, bool t2, bool t3, bool t4, bool t5, bool t6>
    void test_assignable (utility::thrower & thrower)
{
    utility::tracked_registry registry;

    typedef utility::throwing <
        utility::tracked <int>, t1, t2, t3, t4, t5, t6> tracked;
    typedef non_assignable <tracked> test_type;

    test_type i (tracked (thrower, utility::tracked <int> (registry, 4)));
    test_type j (tracked (thrower, utility::tracked <int> (registry, 9)));
    test_type k (tracked (thrower, utility::tracked <int> (registry, 24)));

    utility::assignable <test_type> a (i);
    BOOST_CHECK_EQUAL (a.content().i.content().content(), 4);

    /*
    value_construct, copy, move, copy_assign, move_assign, swap, destruct,
    destruct_moved.
    */
    registry.check_counts (3, 1, 6, 0, 0, 0, 0, 6);

    // Copy-assignment: 1 destruction, 1 copy.
    try {
        a = j;
    } catch (...) {
        // a is now in an invalid state.
        // Reassignment should put this right.
        a = k;
        BOOST_CHECK_EQUAL (a.content().i.content().content(), 24);
        // Propagate the exception anyway.
        throw;
    }
    BOOST_CHECK_EQUAL (a.content().i.content().content(), 9);

    registry.check_counts (3, 2, 6, 0, 0, 0, 1, 6);

    // Copy-assignment: 1 destruction, 1 move.
    a = std::move (k);
    BOOST_CHECK_EQUAL (a.content().i.content().content(), 24);

    registry.check_counts (3, 2, 7, 0, 0, 0, 2, 6);

    utility::assignable <test_type> b (tracked (thrower,
        utility::tracked <int> (registry, 79)));
    BOOST_CHECK_EQUAL (b.content().i.content().content(), 79);

    registry.check_counts (4, 2, 10, 0, 0, 0, 2, 9);

    utility::assignable <test_type> c (b);
    c.content().i.content().content() = 123;

    registry.check_counts (4, 3, 10, 0, 0, 0, 2, 9);

    utility::assignable <test_type> d (std::move (c));
    BOOST_CHECK_EQUAL (d.content().i.content().content(), 123);

    registry.check_counts (4, 3, 11, 0, 0, 0, 2, 9);

    a = b;
    BOOST_CHECK_EQUAL (a.content().i.content().content(), 79);

    registry.check_counts (4, 4, 11, 0, 0, 0, 3, 9);

    a = std::move (d);
    BOOST_CHECK_EQUAL (a.content().i.content().content(), 123);

    registry.check_counts (4, 4, 12, 0, 0, 0, 4, 9);

    utility::assignable <test_type> e (tracked (thrower,
        utility::tracked <int> (registry, 156)));
    BOOST_CHECK_EQUAL (e.content().i.content().content(), 156);

    registry.check_counts (5, 4, 15, 0, 0, 0, 4, 12);

    BOOST_MPL_ASSERT ((std::is_same <
        decltype (e.move_content()), test_type &&>));

    test_type content = e.move_content();

    registry.check_counts (5, 4, 16, 0, 0, 0, 4, 12);

    // Print a list of on which occasions this may have thrown.
    std::cout << "Testing throwing at: " << std::boolalpha
        << t1 << ' ' << t2 << ' ' << t3 << ' ' << t4 << ' ' << t5 << ' ' << t6
        << std::endl;
}

BOOST_AUTO_TEST_CASE (test_utility_assignable_exception_safety) {
    utility::check_all_throw_points (
        &test_assignable <true, true, true, true, true, true>);
}

template <int throw_bits> void check_assignable_all_throw_points() {
    utility::check_all_throw_points (test_assignable <
        !(throw_bits & 0x01), !(throw_bits & 0x02), !(throw_bits & 0x04),
        !(throw_bits & 0x08), !(throw_bits & 0x10), !(throw_bits & 0x20)>);
    check_assignable_all_throw_points <throw_bits - 1>();
}

template <> void check_assignable_all_throw_points <-1>() {}

BOOST_AUTO_TEST_CASE (test_utility_assignable_exception_safety_all) {
    check_assignable_all_throw_points <64>();
}

BOOST_AUTO_TEST_CASE (test_utility_assignable_types) {
    {
        int i = 7;
        utility::assignable <int &> a (i);
        BOOST_CHECK_EQUAL (&a.content(), &i);

        a.content() = 8;
        BOOST_CHECK_EQUAL (i, 8);

        int j = 9;
        a = j;
        BOOST_CHECK_EQUAL (&a.content(), &j);
        BOOST_CHECK_EQUAL (i, 8);

        // Moving a reference merely returns the reference itself.
        BOOST_MPL_ASSERT ((std::is_same <decltype (a.move_content()), int &>));
    }
}

struct trivial {};
struct pretty_trivial { int i; };
struct pretty_trivial_2 { int i; int j; };
struct pretty_trivial_3 { int & i; };
struct pretty_trivial_4 { int i; int & j; };

struct pretty_trivial_5 {
    int i;
    int & j;

    explicit pretty_trivial_5 (int i) : i (i), j (this->i) {}
};

struct nothrow_struct {
    nothrow_struct (nothrow_struct const &) noexcept {}
    nothrow_struct (nothrow_struct &&) noexcept {}
};

struct throw_struct {
    int i, j, k;
    throw_struct (throw_struct const &);
    throw_struct (throw_struct &&);
};

struct throw_struct_with_bool {
    int i, j, k;
    bool b;
};

struct copyable {
    int i;
    copyable (copyable const &) noexcept {}
};

struct moveable {
    int i;
    moveable (moveable &&) noexcept {}
};

BOOST_AUTO_TEST_CASE (test_object_size) {
    // BOOST_CHECK_EQUAL leaves a more informative trace than static_assert.
    // This is also not important enough to cause failure to compile.
    BOOST_CHECK_EQUAL (sizeof (assignable <int>), sizeof (int));

    BOOST_CHECK_EQUAL (sizeof (assignable <trivial>), sizeof (trivial));
    BOOST_CHECK_EQUAL (sizeof (assignable <pretty_trivial>),
        sizeof (pretty_trivial));
    BOOST_CHECK_EQUAL (sizeof (assignable <pretty_trivial_2>),
        sizeof (pretty_trivial_2));
    BOOST_CHECK_EQUAL (sizeof (assignable <pretty_trivial_3>),
        sizeof (pretty_trivial_3));
    BOOST_CHECK_EQUAL (sizeof (assignable <pretty_trivial_4>),
        sizeof (pretty_trivial_4));
    BOOST_CHECK_EQUAL (sizeof (assignable <pretty_trivial_5>),
        sizeof (pretty_trivial_5));

    BOOST_CHECK_EQUAL (sizeof (assignable <moveable>), sizeof (moveable));
    BOOST_CHECK_EQUAL (sizeof (assignable <copyable>), sizeof (copyable));

    BOOST_CHECK_EQUAL (sizeof (assignable <non_assignable <int>>),
        sizeof (non_assignable <int>));

    BOOST_CHECK_EQUAL (sizeof (assignable <nothrow_struct>),
        sizeof (nothrow_struct));
    BOOST_CHECK_EQUAL (sizeof (assignable <throw_struct>),
        sizeof (throw_struct_with_bool));
}

BOOST_AUTO_TEST_SUITE_END()
