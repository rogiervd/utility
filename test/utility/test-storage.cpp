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

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_storage)

template <class Container, class ConstContainer>
    void test_utility_storage_store_as_qualified()
{
    using std::is_same;
    using utility::storage::store_as;

    typedef int type;

    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type, Container>::type, type>));
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type const, Container>::type, type const>));
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type &, Container>::type, std::reference_wrapper <type>>));
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type const &, Container>::type, std::reference_wrapper <type const>>));

    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type, ConstContainer>::type, type const>));
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type const, ConstContainer>::type, type const>));
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type &, ConstContainer>::type, std::reference_wrapper <type> const>));
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        type const &, ConstContainer>::type,
        std::reference_wrapper <type const> const>));

    // Error: void cannot be stored.
/*
    BOOST_MPL_ASSERT ((is_same <typename store_as <
        void, Container>::type, void>));
*/
}

BOOST_AUTO_TEST_CASE (test_utility_storage_store_as) {
    struct container;
    test_utility_storage_store_as_qualified <container, container const>();
    test_utility_storage_store_as_qualified <container &, container const &>();
}

BOOST_AUTO_TEST_CASE (test_utility_storage_get_as_moving) {
    using std::is_same;
    using utility::storage::get_as;

    struct container;
    typedef int type;

    // rvalue container
    BOOST_MPL_ASSERT ((is_same <get_as <
        type, container>::type, type &&>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const, container>::type, type const &&>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type &, container>::type, type &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const &, container>::type, type const &>));

    // const rvalue container
    BOOST_MPL_ASSERT ((is_same <get_as <
        type, container const>::type, type const &&>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const, container const>::type, type const &&>));
    // A reference is already const.
    BOOST_MPL_ASSERT ((is_same <get_as <
        type &, container const>::type, type &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const &, container const>::type, type const &>));

    // lvalue reference container
    BOOST_MPL_ASSERT ((is_same <get_as <
        type, container &>::type, type &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const, container &>::type, type const &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type &, container &>::type, type &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const &, container &>::type, type const &>));

    // rvalue reference container
    BOOST_MPL_ASSERT ((is_same <get_as <
        type, container const &>::type, type const &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const, container const &>::type, type const &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type &, container const &>::type, type &>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        type const &, container const &>::type, type const &>));

    // void type
    BOOST_MPL_ASSERT ((is_same <get_as <
        void, container>::type, void>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        void, container const>::type, void>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        void, container &>::type, void>));
    BOOST_MPL_ASSERT ((is_same <get_as <
        void, container const &>::type, void>));
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

    // void also works.
    BOOST_MPL_ASSERT ((is_same <get_pointer <
        void, container>::type, void *>));
    BOOST_MPL_ASSERT ((is_same <get_pointer <
        void, container const>::type, void const *>));

    // Compile error
/*
    get_pointer <type &, container>::type error;
*/
}

BOOST_AUTO_TEST_SUITE_END()
