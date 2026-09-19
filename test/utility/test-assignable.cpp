/*
Copyright 2014, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_assignable
#include <boost/test/unit_test.hpp>

#include "utility/assignable.hpp"

#include <iostream>

#include <boost/mpl/assert.hpp>

#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

using utility::assignable;

template <class Target, class... Sources> struct disable_if_same_or_derived
{
    typedef void type;
};

template <class Target, class Source>
struct disable_if_same_or_derived<Target, Source>
: std::enable_if<
      !std::is_same<Target, typename std::decay<Source>::type>::value
      && !std::is_base_of<
          Target, typename std::remove_reference<Source>::type>::value>
{};

template <class Content> struct non_assignable
{
    Content i;

    template <
        class... Arguments,
        class Enable = typename disable_if_same_or_derived<
            non_assignable, Arguments...>::type>
    non_assignable(Arguments &&... arguments)
    : i(std::forward<Arguments>(arguments)...)
    {}

    non_assignable(non_assignable const &) = default;
    non_assignable(non_assignable &&) = default;

    non_assignable & operator=(non_assignable const &) = delete;
    non_assignable & operator=(non_assignable &&) = delete;
};

BOOST_AUTO_TEST_SUITE(test_utility_assignable)

BOOST_AUTO_TEST_CASE(test_utility_assignable_simple)
{
    utility::assignable<non_assignable<int>> a(4);

    BOOST_CHECK_EQUAL(a.content().i, 4);

    a = non_assignable<int>(7);

    BOOST_CHECK_EQUAL(a.content().i, 7);

    utility::assignable<non_assignable<int>> b(27);

    a = std::move(b);
    BOOST_CHECK_EQUAL(a.content().i, 27);
}

struct test_assignable
{
    template <class ThrowToggles> void operator()(
        utility::thrower & thrower, ThrowToggles throw_toggles) const
    {
        using utility::copy_count;
        using utility::destruct_count;
        using utility::destruct_moved_count;
        using utility::move_count;
        using utility::value_construct_count;
        utility::tracked_registry registry;

        typedef utility::throwing<utility::tracked<int>, ThrowToggles> tracked;
        typedef non_assignable<tracked> test_type;

        test_type i(tracked(thrower, utility::tracked<int>(registry, 4)));
        test_type j(tracked(thrower, utility::tracked<int>(registry, 9)));
        test_type k(tracked(thrower, utility::tracked<int>(registry, 24)));

        utility::assignable<test_type> a(i);
        BOOST_CHECK_EQUAL(a.content().i.content().content(), 4);

        /*
        value_construct, copy, move, copy_assign, move_assign, swap, destruct,
        destruct_moved.
        */
        auto before = registry.counts();

        // Copy-assignment: 1 destruction, 1 copy.
        try {
            a = j;
        } catch (...) {
            // a is now in an invalid state.
            // Reassignment should put this right.
            a = k;
            BOOST_CHECK_EQUAL(a.content().i.content().content(), 24);
            // Propagate the exception anyway.
            throw;
        }
        BOOST_CHECK_EQUAL(a.content().i.content().content(), 9);
        BOOST_CHECK_EQUAL(
            registry.since(before), copy_count(1) + destruct_count(1));

        before = registry.counts();
        // Copy-assignment: 1 destruction, 1 move.
        a = std::move(k);
        BOOST_CHECK_EQUAL(a.content().i.content().content(), 24);
        BOOST_CHECK_EQUAL(
            registry.since(before), move_count(1) + destruct_count(1));

        before = registry.counts();
        utility::assignable<test_type> b(
            tracked(thrower, utility::tracked<int>(registry, 79)));
        BOOST_CHECK_EQUAL(b.content().i.content().content(), 79);
        BOOST_CHECK_EQUAL(
            registry.since(before),
            value_construct_count(1) + move_count(3) + destruct_moved_count(3));

        before = registry.counts();
        utility::assignable<test_type> c(b);
        c.content().i.content().content() = 123;
        BOOST_CHECK_EQUAL(registry.since(before), copy_count(1));

        before = registry.counts();
        utility::assignable<test_type> d(std::move(c));
        BOOST_CHECK_EQUAL(d.content().i.content().content(), 123);
        BOOST_CHECK_EQUAL(registry.since(before), move_count(1));

        before = registry.counts();
        a = b;
        BOOST_CHECK_EQUAL(a.content().i.content().content(), 79);
        BOOST_CHECK_EQUAL(
            registry.since(before), copy_count(1) + destruct_count(1));

        before = registry.counts();
        a = std::move(d);
        BOOST_CHECK_EQUAL(a.content().i.content().content(), 123);
        BOOST_CHECK_EQUAL(
            registry.since(before), move_count(1) + destruct_count(1));

        before = registry.counts();
        utility::assignable<test_type> e(
            tracked(thrower, utility::tracked<int>(registry, 156)));
        BOOST_CHECK_EQUAL(e.content().i.content().content(), 156);
        BOOST_CHECK_EQUAL(
            registry.since(before),
            value_construct_count(1) + move_count(3) + destruct_moved_count(3));

        BOOST_MPL_ASSERT(
            (std::is_same<decltype(e.move_content()), test_type &&>) );

        before = registry.counts();
        test_type content = e.move_content();
        BOOST_CHECK_EQUAL(registry.since(before), move_count(1));

        // Print a list of on which occasions this may have thrown.
        std::cout << "Testing assignable: " << throw_toggles << std::endl;
    }
};

BOOST_AUTO_TEST_CASE(test_utility_assignable_exception_safety)
{
    utility::check_with_some_throw_toggles(test_assignable());
}

BOOST_AUTO_TEST_CASE(test_utility_assignable_exception_safety_many)
{
    utility::check_with_many_throw_toggles(test_assignable());
}

BOOST_AUTO_TEST_CASE(test_utility_assignable_exception_safety_all)
{
    utility::check_with_all_throw_toggles(test_assignable());
}

BOOST_AUTO_TEST_CASE(test_utility_assignable_types)
{
    {
        int i = 7;
        utility::assignable<int &> a(i);
        BOOST_CHECK_EQUAL(&a.content(), &i);

        a.content() = 8;
        BOOST_CHECK_EQUAL(i, 8);

        int j = 9;
        a = j;
        BOOST_CHECK_EQUAL(&a.content(), &j);
        BOOST_CHECK_EQUAL(i, 8);

        // Moving a reference merely returns the reference itself.
        BOOST_MPL_ASSERT((std::is_same<decltype(a.move_content()), int &>) );
    }
}

struct trivial
{};
struct pretty_trivial
{
    int i;
};
struct pretty_trivial_2
{
    int i;
    int j;
};
struct pretty_trivial_3
{
    int & i;
};
struct pretty_trivial_4
{
    int i;
    int & j;
};

struct pretty_trivial_5
{
    int i;
    int & j;

    explicit pretty_trivial_5(int i) : i(i), j(this->i) {}
};

struct nothrow_struct
{
    nothrow_struct(nothrow_struct const &) noexcept {}
    nothrow_struct(nothrow_struct &&) noexcept {}
};

struct throw_struct
{
    int i, j, k;
    throw_struct(throw_struct const &);
    throw_struct(throw_struct &&);
};

struct throw_struct_with_bool
{
    int i, j, k;
    bool b;
};

struct copyable
{
    int i;
    copyable(copyable const &) noexcept {}
};

struct moveable
{
    int i;
    moveable(moveable &&) noexcept {}
};

BOOST_AUTO_TEST_CASE(test_object_size)
{
    // BOOST_CHECK_EQUAL leaves a more informative trace than static_assert.
    // This is also not important enough to cause failure to compile.
    BOOST_CHECK_EQUAL(sizeof(assignable<int>), sizeof(int));

    BOOST_CHECK_EQUAL(sizeof(assignable<trivial>), sizeof(trivial));
    BOOST_CHECK_EQUAL(
        sizeof(assignable<pretty_trivial>), sizeof(pretty_trivial));
    BOOST_CHECK_EQUAL(
        sizeof(assignable<pretty_trivial_2>), sizeof(pretty_trivial_2));
    BOOST_CHECK_EQUAL(
        sizeof(assignable<pretty_trivial_3>), sizeof(pretty_trivial_3));
    BOOST_CHECK_EQUAL(
        sizeof(assignable<pretty_trivial_4>), sizeof(pretty_trivial_4));
    BOOST_CHECK_EQUAL(
        sizeof(assignable<pretty_trivial_5>), sizeof(pretty_trivial_5));

    BOOST_CHECK_EQUAL(sizeof(assignable<moveable>), sizeof(moveable));
    BOOST_CHECK_EQUAL(sizeof(assignable<copyable>), sizeof(copyable));

    BOOST_CHECK_EQUAL(
        sizeof(assignable<non_assignable<int>>), sizeof(non_assignable<int>));

    BOOST_CHECK_EQUAL(
        sizeof(assignable<nothrow_struct>), sizeof(nothrow_struct));
    BOOST_CHECK_EQUAL(
        sizeof(assignable<throw_struct>), sizeof(throw_struct_with_bool));
}

BOOST_AUTO_TEST_SUITE_END()
