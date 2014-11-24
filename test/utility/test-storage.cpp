/*
Copyright 2012 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_storage
#include "utility/test/boost_unit_test.hpp"

#include "utility/storage.hpp"

#include <type_traits>
#include <iostream>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_storage)

using utility::storage::store;
using utility::storage::get;
using utility::storage::get_pointer;
using utility::storage::pass;
using utility::storage::pass_rvalue;

template <class Target, class Source>
    Target implicit_cast (Source && object)
{ return std::forward <Source> (object); }

template <class Type> Type test_storage_of (Type i, Type j) {
    typename store <Type>::type storage = std::forward <Type> (i);
    storage = std::forward <Type> (j);
    // Make sure that this has a side-effect.
    std::cout << sizeof (storage) << alignof (storage) << std::endl;
    typename store <Type>::type * address = &storage;
    std::cout << address << std::endl;
    return implicit_cast <typename get <Type>::type> (storage);
}

typedef int array_type [3];
typedef int const const_array_type [3];

struct array_data {
    int a;
    int b;
    int c;
    array_data (int a, int b, int c) : a (a), b (b), c (c) {}
};

array_data test_storage_of_array (array_type & i, array_type & j) {
    typename store <array_type>::type storage = j;
    storage = j;
    std::cout << sizeof (storage) << alignof (storage) << std::endl;
    std::cout << &storage << std::endl;
    return array_data (storage [0], storage[1], storage [2]);
}

array_data test_storage_of_array (const_array_type & i, const_array_type & j) {
    typename store <const_array_type>::type storage = j;
    storage = j;
    std::cout << sizeof (storage) << alignof (storage) << std::endl;
    std::cout << &storage << std::endl;
    return array_data (storage [0], storage[1], storage [2]);
}

// Function types are strange: they can be passed in, but they can't be
// returned.
typedef int function_type (double);

function_type & test_storage_of_function (function_type i, function_type j) {
    typename store <function_type>::type storage = i;
    storage = j;
    std::cout << sizeof (storage) << std::endl;
    std::cout << &storage << std::endl;
    return storage;
}

int function_1 (double) { return 1; }
int function_2 (double) { return 2; }

struct structure {
    int member_function_1 (double) { return 1; }
    int member_function_2 (double) { return 2; }

    int c_member_function_1 (double) const { return 1; }
    int c_member_function_2 (double) const { return 2; }

    int v_member_function_1 (double) volatile { return 1; }
    int v_member_function_2 (double) volatile { return 2; }

    int cv_member_function_1 (double) const volatile { return 1; }
    int cv_member_function_2 (double) const volatile { return 2; }

    int member_1;
    int member_2;
};

typedef int (structure::* member_function_type) (double);
typedef int (structure::* c_member_function_type) (double) const;
typedef int (structure::* v_member_function_type) (double) volatile;
typedef int (structure::* cv_member_function_type) (double) const volatile;
typedef int (structure::* member_type);

BOOST_AUTO_TEST_CASE (test_utility_storage_store) {
    using std::is_same;

    {
        int value = test_storage_of <int> (5, 7);
        BOOST_CHECK_EQUAL (value, 7);
    }
    {
        int const value = test_storage_of <int const> (5, 7);
        BOOST_CHECK_EQUAL (value, 7);
    }
    // Lvalue reference.
    int i = 9;
    int j = 24;
    {
        int & value = test_storage_of <int &> (i, j);
        BOOST_CHECK_EQUAL (value, 24);
        BOOST_CHECK_EQUAL (&value, &j);
    }
    BOOST_CHECK_EQUAL (i, 9);
    BOOST_CHECK_EQUAL (j, 24);
    // Rvalue reference.
    {
        int && value = test_storage_of <int &&> (std::move (i), std::move (j));
        BOOST_CHECK_EQUAL (value, 24);
        BOOST_CHECK_EQUAL (&value, &j);
    }
    BOOST_CHECK_EQUAL (i, 9);
    BOOST_CHECK_EQUAL (j, 24);

    // Array.
    int a [3] = {3, 4, 5};
    int b [3] = {7, 8, 9};
    int const c_a [3] = {3, 4, 5};
    int const c_b [3] = {7, 8, 9};
    {
        array_data c = test_storage_of_array (a, b);
        BOOST_CHECK_EQUAL (c.a, 7);
        BOOST_CHECK_EQUAL (c.b, 8);
        BOOST_CHECK_EQUAL (c.c, 9);
    }
    {
        array_data c = test_storage_of_array (c_a, c_b);
        BOOST_CHECK_EQUAL (c.a, 7);
        BOOST_CHECK_EQUAL (c.b, 8);
        BOOST_CHECK_EQUAL (c.c, 9);
    }
    {
        array_type & c = test_storage_of <array_type &> (a, b);
        BOOST_CHECK_EQUAL (&c, &b);
        BOOST_CHECK_EQUAL (c [0], 7);
        BOOST_CHECK_EQUAL (c [1], 8);
        BOOST_CHECK_EQUAL (c [2], 9);
    }
    {
        const_array_type & c = test_storage_of <const_array_type &> (c_a, c_b);
        BOOST_CHECK_EQUAL (&c, &c_b);
        BOOST_CHECK_EQUAL (c [0], 7);
        BOOST_CHECK_EQUAL (c [1], 8);
        BOOST_CHECK_EQUAL (c [2], 9);
    }
    {
        array_type * c = test_storage_of <array_type *> (&a, &b);
        BOOST_CHECK_EQUAL (c, &b);
        BOOST_CHECK_EQUAL ((*c) [0], 7);
        BOOST_CHECK_EQUAL ((*c) [1], 8);
        BOOST_CHECK_EQUAL ((*c) [2], 9);
    }
    {
        const_array_type * c
            = test_storage_of <const_array_type *> (&c_a, &c_b);
        BOOST_CHECK_EQUAL (c, &c_b);
        BOOST_CHECK_EQUAL ((*c) [0], 7);
        BOOST_CHECK_EQUAL ((*c) [1], 8);
        BOOST_CHECK_EQUAL ((*c) [2], 9);
    }

    // Function.
    {
        int return_value
            = test_storage_of_function (function_1, function_2) (3.4);
        BOOST_CHECK_EQUAL (return_value, 2);
    }
    // Function reference.
    {
        int (& f) (double)
            = test_storage_of <int (&) (double)> (function_1, function_2);
        BOOST_CHECK_EQUAL (&f, &function_2);
    }
    // Function pointer.
    {
        int (* f) (double)
            = test_storage_of <int (*) (double)> (&function_1, &function_2);
        BOOST_CHECK_EQUAL (f, &function_2);
        f  = test_storage_of <int (*) (double)> (function_2, function_1);
        BOOST_CHECK_EQUAL (f, &function_1);
    }

    // Member pointer.
    {
        member_type m = test_storage_of <member_type> (
            &structure::member_1, &structure::member_2);
        BOOST_CHECK_EQUAL (m, &structure::member_2);
    }
    // Member function pointer.
    {
        member_function_type m = test_storage_of <member_function_type> (
            &structure::member_function_1, &structure::member_function_2);
        BOOST_CHECK_EQUAL (m, &structure::member_function_2);
    }
    {
        c_member_function_type m = test_storage_of <c_member_function_type> (
            &structure::c_member_function_1, &structure::c_member_function_2);
        BOOST_CHECK_EQUAL (m, &structure::c_member_function_2);
    }
    {
        v_member_function_type m = test_storage_of <v_member_function_type> (
            &structure::v_member_function_1, &structure::v_member_function_2);
        BOOST_CHECK_EQUAL (m, &structure::v_member_function_2);
    }
    {
        cv_member_function_type m = test_storage_of <cv_member_function_type> (
            &structure::cv_member_function_1, &structure::cv_member_function_2);
        BOOST_CHECK_EQUAL (m, &structure::cv_member_function_2);
    }

    // Error: void cannot be stored.
    // BOOST_MPL_ASSERT ((is_same <typename store <void>::type, void>));
}

template <class Type> struct test_container {
    typedef Type value_type;
    typedef typename pass <Type>::type argument_type;

    typename store <Type>::type value;

    test_container (argument_type value)
    : value (std::forward <argument_type> (value)) {}
};

template <class Container> typename get <
    typename std::decay <Container>::type::value_type, Container &&>::type
    inline extract (Container && container)
{
    get <typename std::decay <Container>::type::value_type, Container &&>
        extractor;
    return extractor (container.value);
}

template <class Container> typename get_pointer <
    typename std::decay <Container>::type::value_type, Container &&>::type
    inline extract_pointer (Container && container)
{
    get <typename std::decay <Container>::type::value_type, Container &>
        extractor;
    return &extractor (container.value);
}

// Trick to be able to take addresses of rvalues.
template <class Type> typename std::add_pointer <Type>::type
    address_of (Type && object)
{ return &object; }

/**
Check whether a type expression is get_detail::incomplete_type.
The exact type is an implementation detail, so maybe it ought to use a clever
way of checking that it is any incomplete type instead of one specific one.
*/
#define CHECK_INCOMPLETE_TYPE(...) \
    BOOST_MPL_ASSERT ((std::is_same <__VA_ARGS__, \
        utility::storage::detail::incomplete_type>))

BOOST_AUTO_TEST_CASE (test_utility_storage_get) {
    // Normal type.
    {
        test_container <int> c (4);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int &>));
        BOOST_CHECK_EQUAL (extract (c), 4);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), int *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)), 4);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), int *>));
    }
    {
        test_container <int const> c (4);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 4);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), int const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)), 4);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), int const *>));
    }
    {
        test_container <int> const c (4);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 4);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), int const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)), 4);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), int const *>));
    }
    {
        test_container <int const> const c (4);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 4);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), int const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &&>));
        BOOST_CHECK_EQUAL (extract (c), 4);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), int const *>));
    }

    // Lvalue reference type.
    {
        int i = 7;
        test_container <int &> c (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &, decltype (std::move (c))>::type);
    }
    {
        int const i = 7;
        test_container <int const &> c (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int const &, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int const &, decltype (std::move (c))>::type);
    }
    {
        int i = 7;
        test_container <int &> const c (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &, decltype (std::move (c))>::type);
    }
    {
        int const i = 7;
        test_container <int const &> const c (i);
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int const &, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (&extract (c), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int const &, decltype (std::move (c))>::type);
    }

    // Rvalue reference type.
    {
        int i = 7;
        test_container <int &&> c (std::move (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &&, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &&, decltype (std::move (c))>::type);
    }
    {
        int const i = 7;
        test_container <int const &&> c (std::move (i));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), int const &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int const &&, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (typename
            get_pointer <int const &&, decltype (std::move (c))>::type);
    }
    {
        int i = 7;
        test_container <int &&> const c (std::move (i));
        BOOST_MPL_ASSERT ((std::is_same <decltype (extract (c)), int &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &&, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int &&, decltype (std::move (c))>::type);
    }
    {
        int const i = 7;
        test_container <int const &&> const c (std::move (i));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), int const &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <int const &&, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), int const &&>));
        BOOST_CHECK_EQUAL (extract (c), 7);
        BOOST_CHECK_EQUAL (address_of (extract (c)), &i);

        CHECK_INCOMPLETE_TYPE (typename
            get_pointer <int const &&, decltype (std::move (c))>::type);
    }

    // Array type.
    {
        array_type a = {12, 23, 34};
        test_container <array_type> c (a);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), array_type &>));
        BOOST_CHECK_EQUAL (extract (c) [0], 12);
        BOOST_CHECK_EQUAL (extract (c) [1], 23);
        BOOST_CHECK_EQUAL (extract (c) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), array_type *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), array_type &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)) [0], 12);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [1], 23);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), array_type *>));
    }
    {
        array_type const a = {12, 23, 34};
        test_container <array_type const> c (a);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), array_type const &>));
        BOOST_CHECK_EQUAL (extract (c) [0], 12);
        BOOST_CHECK_EQUAL (extract (c) [1], 23);
        BOOST_CHECK_EQUAL (extract (c) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), array_type const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), array_type const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)) [0], 12);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [1], 23);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), array_type const *>));
    }
    {
        array_type a = {12, 23, 34};
        test_container <array_type> const c (a);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), array_type const &>));
        BOOST_CHECK_EQUAL (extract (c) [0], 12);
        BOOST_CHECK_EQUAL (extract (c) [1], 23);
        BOOST_CHECK_EQUAL (extract (c) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), array_type const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), array_type const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)) [0], 12);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [1], 23);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), array_type const *>));
    }
    {
        array_type const a = {12, 23, 34};
        test_container <array_type const> const c (a);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), array_type const &>));
        BOOST_CHECK_EQUAL (extract (c) [0], 12);
        BOOST_CHECK_EQUAL (extract (c) [1], 23);
        BOOST_CHECK_EQUAL (extract (c) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), array_type const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), array_type const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)) [0], 12);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [1], 23);
        BOOST_CHECK_EQUAL (extract (std::move (c)) [2], 34);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), array_type const *>));
    }

    // Function type.
    {
        test_container <function_type> c (function_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), function_type &>));
        BOOST_CHECK_EQUAL (extract (c)(2.5), 1);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <function_type, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), function_type &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c))(2.5), 1);

        CHECK_INCOMPLETE_TYPE (typename
            get_pointer <function_type, decltype (std::move (c))>::type);
    }
    {
        // Functions do not come const-qualified.
        test_container <function_type> const c (function_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), function_type &>));
        BOOST_CHECK_EQUAL (extract (c)(2.5), 1);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <function_type, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), function_type &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c))(2.5), 1);

        CHECK_INCOMPLETE_TYPE (typename
            get_pointer <function_type, decltype (std::move (c))>::type);
    }

    // Function reference.
    {
        test_container <function_type &> c (function_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), function_type &>));
        BOOST_CHECK_EQUAL (extract (c)(2.5), 1);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <function_type &, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), function_type &>));
        BOOST_CHECK_EQUAL (extract (std::move (c))(2.5), 1);

        CHECK_INCOMPLETE_TYPE (typename
            get_pointer <function_type &, decltype (std::move (c))>::type);
    }
    {
        test_container <function_type &> const c (function_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), function_type &>));
        BOOST_CHECK_EQUAL (extract (c)(2.5), 1);

        CHECK_INCOMPLETE_TYPE (
            typename get_pointer <function_type &, decltype (c)>::type);

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), function_type &>));
        BOOST_CHECK_EQUAL (extract (std::move (c))(2.5), 1);

        CHECK_INCOMPLETE_TYPE (typename
            get_pointer <function_type &, decltype (std::move (c))>::type);
    }

    // Function pointer.
    {
        test_container <function_type *> c (function_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), function_type * &>));
        BOOST_CHECK_EQUAL (extract (c)(2.5), 1);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), function_type **>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), function_type * &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c))(2.5), 1);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), function_type **>));
    }
    {
        test_container <function_type *> const c (function_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), function_type * const &>));
        BOOST_CHECK_EQUAL (extract (c)(2.5), 1);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), function_type * const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), function_type * const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c))(2.5), 1);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))),
            function_type * const *>));
    }

    // Member pointer.
    {
        test_container <member_type> c (&structure::member_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), member_type &>));
        BOOST_CHECK_EQUAL (extract (c), &structure::member_1);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), member_type *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), member_type &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)), &structure::member_1);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), member_type *>));
    }
    {
        test_container <member_type const> c (&structure::member_1);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), member_type const &>));
        BOOST_CHECK_EQUAL (extract (c), &structure::member_1);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), member_type const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), member_type const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)), &structure::member_1);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))), member_type const *>));
    }

    // Member function pointer.
    {
        test_container <member_function_type> c (&structure::member_function_2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), member_function_type &>));
        BOOST_CHECK_EQUAL (extract (c), &structure::member_function_2);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), member_function_type *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), member_function_type &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)),
            &structure::member_function_2);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))),
            member_function_type *>));
    }
    {
        test_container <member_function_type const> c (
            &structure::member_function_2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), member_function_type const &>));
        BOOST_CHECK_EQUAL (extract (c), &structure::member_function_2);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), member_function_type const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))),
            member_function_type const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)),
            &structure::member_function_2);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))),
            member_function_type const *>));
    }

    // Member function pointer.
    {
        test_container <cv_member_function_type> c (
            &structure::cv_member_function_2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), cv_member_function_type &>));
        BOOST_CHECK_EQUAL (extract (c), &structure::cv_member_function_2);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), cv_member_function_type *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))), cv_member_function_type &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)),
            &structure::cv_member_function_2);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))),
            cv_member_function_type *>));
    }
    {
        test_container <cv_member_function_type const> c (
            &structure::cv_member_function_2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (c)), cv_member_function_type const &>));
        BOOST_CHECK_EQUAL (extract (c), &structure::cv_member_function_2);

        extract_pointer (c);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (c)), cv_member_function_type const *>));

        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract (std::move (c))),
            cv_member_function_type const &&>));
        BOOST_CHECK_EQUAL (extract (std::move (c)),
            &structure::cv_member_function_2);

        extract_pointer (std::move (c));
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (extract_pointer (std::move (c))),
            cv_member_function_type const *>));
    }
}

template <class Type> int pass_to (typename pass <Type>::type)
{ return 0; }
template <class Type> int pass_to (typename pass_rvalue <Type>::type)
{ return 1; }

BOOST_AUTO_TEST_CASE (test_utility_storage_pass) {
    using utility::storage::detail::type_that_cannot_be_constructed;

    BOOST_MPL_ASSERT ((std::is_same <
        pass <int>::type, int const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <int>::type, int &&>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <int &>::type, int &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <int &>::type, type_that_cannot_be_constructed>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <int &&>::type, int &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <int &&>::type, type_that_cannot_be_constructed>));

    {
        double d = 4.5;
        BOOST_CHECK_EQUAL (pass_to <double> (d), 0);
        BOOST_CHECK_EQUAL (pass_to <double> (7.8), 1);

        BOOST_CHECK_EQUAL (pass_to <double &> (d), 0);
        BOOST_CHECK_EQUAL (pass_to <double &&> (7.8), 0);
    }

    // const.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <int const>::type, int const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <int const>::type, int &&>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <int const &>::type, int const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <int const &>::type, type_that_cannot_be_constructed>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <int const &&>::type, int const &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <int const &&>::type, type_that_cannot_be_constructed>));

    {
        double d = 4.5;
        BOOST_CHECK_EQUAL (pass_to <double const> (d), 0);
        BOOST_CHECK_EQUAL (pass_to <double const> (7.8), 1);

        BOOST_CHECK_EQUAL (pass_to <double const &> (d), 0);
        BOOST_CHECK_EQUAL (pass_to <double const &&> (7.8), 0);
    }

    // Array.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <array_type>::type, array_type const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <array_type>::type, array_type &&>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <array_type &>::type, array_type &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <array_type &>::type, type_that_cannot_be_constructed>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <array_type &&>::type, array_type &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <array_type &&>::type,
        type_that_cannot_be_constructed>));

    {
        array_type a = {5,6,7};
        BOOST_CHECK_EQUAL (pass_to <array_type> (a), 0);
        BOOST_CHECK_EQUAL (pass_to <array_type> (std::move (a)), 1);

        BOOST_CHECK_EQUAL (pass_to <array_type &> (a), 0);
        BOOST_CHECK_EQUAL (pass_to <array_type &&> (std::move (a)), 0);
    }

    // Const array.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <array_type const>::type, array_type const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <array_type const>::type, array_type &&>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <array_type const &>::type, array_type const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <array_type const &>::type,
        type_that_cannot_be_constructed>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <array_type const &&>::type, array_type const &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <array_type const &&>::type,
        type_that_cannot_be_constructed>));

    {
        array_type a = {5,6,7};
        BOOST_CHECK_EQUAL (pass_to <array_type const> (a), 0);
        BOOST_CHECK_EQUAL (pass_to <array_type const> (std::move (a)), 1);

        BOOST_CHECK_EQUAL (pass_to <array_type const &> (a), 0);
        BOOST_CHECK_EQUAL (pass_to <array_type const &&> (std::move (a)), 0);
    }

    // Function.
    // Function types don't come const-qualified.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <function_type>::type, function_type &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <function_type>::type, function_type &&>));

    {
        BOOST_CHECK_EQUAL (pass_to <function_type> (function_1), 0);
    }

    // Function reference.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <function_type &>::type, function_type &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <function_type &>::type,
        type_that_cannot_be_constructed>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <function_type &&>::type, function_type &&>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <function_type &&>::type,
        type_that_cannot_be_constructed>));

    {
        BOOST_CHECK_EQUAL (pass_to <function_type &> (function_1), 0);
    }

    // Function pointer.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <function_type *>::type, function_type * const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <function_type *>::type, function_type * &&>));

    {
        function_type * f = function_1;
        BOOST_CHECK_EQUAL (pass_to <function_type *> (f), 0);
        BOOST_CHECK_EQUAL (pass_to <function_type *> (std::move (f)), 1);

        BOOST_CHECK_EQUAL (pass_to <function_type * &> (f), 0);
        BOOST_CHECK_EQUAL (pass_to <function_type * &&> (std::move (f)), 0);
    }

    // Member.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <member_type>::type, member_type const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <member_type>::type, member_type &&>));

    {
        member_type m = &structure::member_1;
        BOOST_CHECK_EQUAL (pass_to <member_type> (m), 0);
        BOOST_CHECK_EQUAL (pass_to <member_type> (std::move (m)), 1);

        BOOST_CHECK_EQUAL (pass_to <member_type &> (m), 0);
        BOOST_CHECK_EQUAL (pass_to <member_type &&> (std::move (m)), 0);
    }

    // Member function.
    BOOST_MPL_ASSERT ((std::is_same <
        pass <member_function_type>::type, member_function_type const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <member_function_type>::type, member_function_type &&>));

    BOOST_MPL_ASSERT ((std::is_same <
        pass <v_member_function_type>::type,
        v_member_function_type const &>));
    BOOST_MPL_ASSERT ((std::is_same <
        pass_rvalue <v_member_function_type>::type,
        v_member_function_type &&>));

    {
        member_function_type m = &structure::member_function_1;
        BOOST_CHECK_EQUAL (pass_to <member_function_type> (m), 0);
        BOOST_CHECK_EQUAL (pass_to <member_function_type> (std::move (m)), 1);

        BOOST_CHECK_EQUAL (pass_to <member_function_type &> (m), 0);
        BOOST_CHECK_EQUAL (
            pass_to <member_function_type &&> (std::move (m)), 0);
    }
}

BOOST_AUTO_TEST_CASE (test_utility_storage_get_pointer) {
    using std::is_same;
    using utility::storage::get_pointer;

    struct container;
    typedef int type;

    BOOST_MPL_ASSERT ((is_same <get_pointer <
        type, container>::type, type *>));
    BOOST_MPL_ASSERT ((is_same <get_pointer <
        type const, container>::type, type const *>));
    BOOST_MPL_ASSERT ((is_same <get_pointer <
        type, container const>::type, type const *>));
    BOOST_MPL_ASSERT ((is_same <get_pointer <
        type const, container const>::type, type const *>));

    // Compile error
/*
    get_pointer <void, container>::type error;
    get_pointer <type &, container>::type error;
*/
}

BOOST_AUTO_TEST_SUITE_END()
