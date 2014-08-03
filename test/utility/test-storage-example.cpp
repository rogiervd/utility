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

/** \file
Give an example, and test, of how to use the classes defined in
\c utility::storage.
*/

#define BOOST_TEST_MODULE test_utility_storage
#include "utility/test/boost_unit_test.hpp"

#include "utility/storage.hpp"

#include <type_traits>
#include <string>

#include <boost/mpl/assert.hpp>

#include "utility/test/tracked.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_storage_example)

namespace storage = utility::storage;

template <class Type> class simple_container {
public:
    simple_container (typename storage::pass <Type>::type content)
    : content_ (content) {}

    simple_container (typename storage::pass_rvalue <Type>::type content)
    : content_ (std::move (content)) {}

    typename storage::get <Type, simple_container &>::type
        content() { return content_; }
    typename storage::get <Type, simple_container const &>::type
        content() const { return content_; }

    typename storage::get_pointer <Type, simple_container &>::type
        pointer() { return & content(); }
    typename storage::get_pointer <Type, simple_container const &>::type
        pointer() const { return & content(); }

    void replace_with (typename storage::pass <Type>::type new_content)
    { content_ = new_content; }
    void replace_with (typename storage::pass_rvalue <Type>::type new_content)
    { content_ = std::move (new_content); }

private:
    typename storage::store <Type>::type content_;
};

BOOST_AUTO_TEST_CASE (test_utility_storage_example_simple) {
    {
        simple_container <int> c (7);
        BOOST_CHECK_EQUAL (c.content(), 7);
        c.content() = 8;
        BOOST_CHECK_EQUAL (c.content(), 8);
    }
    // const container.
    {
        simple_container <int> const c (-7);
        BOOST_CHECK_EQUAL (c.content(), -7);
        BOOST_MPL_ASSERT ((
            std::is_same <decltype (c.content()), int const &>));
    }
    // Contain a reference.
    {
        int i;
        simple_container <int &> c (i);
        i = 5;
        BOOST_CHECK_EQUAL (c.content(), 5);
        c.content() = 15;
        BOOST_CHECK_EQUAL (i, 15);

        // Replace the original object.
        int i2 = 4;
        c.replace_with (i2);
        BOOST_CHECK_EQUAL (c.content(), 4);
        BOOST_CHECK_EQUAL (i, 15);

        c.content() = 13;
        BOOST_CHECK_EQUAL (i2, 13);
        BOOST_CHECK_EQUAL (i, 15);
    }
    // const container to a reference.
    {
        int i;
        simple_container <int &> const c (i);
        i = 5;
        BOOST_CHECK_EQUAL (c.content(), 5);
        c.content() = 15;
        BOOST_CHECK_EQUAL (i, 15);
    }

#ifndef UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
    // Arrays
    {
        int is [5] {5, 3, 5, 7};
        simple_container <int [5]> c (is);
        BOOST_CHECK_EQUAL (c.content() [0], 5);
        BOOST_CHECK_EQUAL (c.content() [1], 3);
        BOOST_CHECK_EQUAL (c.content() [2], 5);
        BOOST_CHECK_EQUAL (c.content() [3], 7);
        BOOST_CHECK_EQUAL (c.content() [4], 0);

        BOOST_CHECK_EQUAL ((*c.pointer())[0], 5);
        BOOST_CHECK_EQUAL ((*c.pointer())[3], 7);
    }
    {
        int const is [2] {27};
        simple_container <int [2]> c (is);
        BOOST_CHECK_EQUAL (c.content() [0], 27);
        BOOST_CHECK_EQUAL (c.content() [1], 0);
    }
    {
        int is [2] {27};
        simple_container <int [2]> const c (is);
        BOOST_CHECK_EQUAL (c.content() [0], 27);
        BOOST_CHECK_EQUAL (c.content() [1], 0);
    }
    {
        int const is [2] {27};
        simple_container <int [2]> const c (is);
        BOOST_CHECK_EQUAL (c.content() [0], 27);
        BOOST_CHECK_EQUAL (c.content() [1], 0);
    }
    {
        int is [2] {27};
        simple_container <int const [2]> const c (is);
        BOOST_CHECK_EQUAL (c.content() [0], 27);
        BOOST_CHECK_EQUAL (c.content() [1], 0);
    }
#endif
}

BOOST_AUTO_TEST_CASE (test_utility_storage_example_extensive) {
    {
        utility::tracked_registry registry;
        simple_container <utility::tracked <int>> c (
            utility::tracked <int> (registry, 9));
        BOOST_CHECK_EQUAL (c.content().content(), 9);
        // value_construct, copy, move, copy_assign, move_assign, swap,
        // destruct, destruct_moved)
        registry.check_counts (1, 0, 1, 0, 0, 0, 0, 1);

        c.content() = utility::tracked <int> (registry, 91);
        BOOST_CHECK_EQUAL (c.content().content(), 91);
        registry.check_counts (2, 0, 1, 0, 1, 0, 0, 2);

        utility::tracked <int> t (registry, 89);
        c.replace_with (t);
        BOOST_CHECK_EQUAL (c.content().content(), 89);
        registry.check_counts (3, 0, 1, 1, 1, 0, 0, 2);
    }
    // Initialise with lvalue reference.
    {
        utility::tracked_registry registry;
        utility::tracked <int> t (registry, 78);
        simple_container <utility::tracked <int>> c (t);
        BOOST_CHECK_EQUAL (c.content().content(), 78);
        // value_construct, copy, move, copy_assign, move_assign, swap,
        // destruct, destruct_moved)
        registry.check_counts (1, 1, 0, 0, 0, 0, 0, 0);
    }
    // Const container.
    {
        utility::tracked_registry registry;
        simple_container <utility::tracked <int>> const c (
            utility::tracked <int> (registry, 9));
        BOOST_CHECK_EQUAL (c.content().content(), 9);
        BOOST_MPL_ASSERT ((std::is_same <decltype (c.content()),
            utility::tracked <int> const &>));
    }
    // Const contents.
    {
        utility::tracked_registry registry;
        simple_container <utility::tracked <int> const> c (
            utility::tracked <int> (registry, 9));
        BOOST_CHECK_EQUAL (c.content().content(), 9);
        // value_construct, copy, move, copy_assign, move_assign, swap,
        // destruct, destruct_moved)
        registry.check_counts (1, 0, 1, 0, 0, 0, 0, 1);

        c.replace_with (utility::tracked <int> (registry, 91));
        BOOST_CHECK_EQUAL (c.content().content(), 91);
        registry.check_counts (2, 0, 1, 0, 1, 0, 0, 2);
    }
}

int example_function_1 (double, std::string) { return 0; }
int example_function_2 (double, std::string) { return 1; }

struct example_struct {
    int example_function_1 (std::string, double) { return 0; }
    int example_function_2 (std::string, double) { return 1; }

    int example_function_3 (std::string, double) const { return 0; }
    int example_function_4 (std::string, double) const { return 1; }

    int example_function_5 (std::string, double) volatile { return 0; }
    int example_function_6 (std::string, double) volatile { return 1; }

    int example_function_7 (std::string, double) const volatile { return 0; }
    int example_function_8 (std::string, double) const volatile { return 1; }

    std::string example_member_1;
    std::string example_member_2;
};

BOOST_AUTO_TEST_CASE (test_utility_storage_example_functions) {
    // Function.
    {
        simple_container <int (double, std::string)> c (example_function_1);
        BOOST_CHECK_EQUAL (&c.content(), &example_function_1);
        c.replace_with (example_function_2);
        BOOST_CHECK_EQUAL (&c.content(), &example_function_2);
    }
    // Function reference.
    {
        simple_container <int (&) (double, std::string)> c (example_function_1);
        BOOST_CHECK_EQUAL (&c.content(), &example_function_1);
        c.replace_with (example_function_2);
        BOOST_CHECK_EQUAL (&c.content(), &example_function_2);
    }
    // Function pointer.
    {
        simple_container <int (*) (double, std::string)> c (example_function_1);
        BOOST_CHECK_EQUAL (c.content(), &example_function_1);
        c.replace_with (example_function_2);
        BOOST_CHECK_EQUAL (c.content(), &example_function_2);
    }
    // Member functions.
    {
        simple_container <int (example_struct::*) (std::string, double)> c (
            &example_struct::example_function_1);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_1);
        c.replace_with (&example_struct::example_function_2);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_2);
    }
    {
        simple_container <
            int (example_struct::*) (std::string, double) const> c (
            &example_struct::example_function_3);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_3);
        c.replace_with (&example_struct::example_function_4);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_4);
    }
    {
        simple_container <
            int (example_struct::*) (std::string, double) volatile> c (
            &example_struct::example_function_5);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_5);
        c.replace_with (&example_struct::example_function_6);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_6);
    }
    {
        simple_container <
            int (example_struct::*) (std::string, double) const volatile> c (
            &example_struct::example_function_7);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_7);
        c.replace_with (&example_struct::example_function_8);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_function_8);
    }
    {
        simple_container <std::string (example_struct::*)> c (
            &example_struct::example_member_1);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_member_1);
        c.replace_with (&example_struct::example_member_2);
        BOOST_CHECK_EQUAL (c.content(), &example_struct::example_member_2);
    }
}

BOOST_AUTO_TEST_SUITE_END()
