// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_utility_is_default_constructible
#include <boost/test/unit_test.hpp>

#include "utility/is_default_constructible.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_is_default_constructible)

struct default_constructible
{
    std::shared_ptr<int> a;

    default_constructible() : a(std::make_shared<int>(6)) {}
};

struct nothrow_default_constructible
{
    nothrow_default_constructible() noexcept {}
};

struct not_default_constructible
{
    not_default_constructible() = delete;
};

struct abstract
{
    virtual void method() = 0;
};

template <template <class> class Predicate> void check_with()
{
    static_assert(!Predicate<void>::value, "");
    static_assert(Predicate<int>::value, "");
    static_assert(Predicate<int const>::value, "");
    static_assert(Predicate<void *>::value, "");
    static_assert(!Predicate<int &>::value, "");
    static_assert(!Predicate<int const &>::value, "");
    static_assert(!Predicate<int &&>::value, "");
    static_assert(!Predicate<int const &&>::value, "");

    static_assert(Predicate<int[5]>::value, "");

    typedef int function_type(float);
    function_type f;

    static_assert(Predicate<int(float)>::value, "");
    static_assert(!Predicate<int (&)(float)>::value, "");
    static_assert(Predicate<int (*)(float)>::value, "");

    static_assert(!Predicate<abstract>::value, "");
}

BOOST_AUTO_TEST_CASE(test_utility_is_default_constructible)
{
    static_assert(
        utility::is_default_constructible<nothrow_default_constructible>::value,
        "");
    static_assert(
        utility::is_default_constructible<default_constructible>::value, "");
    static_assert(
        !utility::is_default_constructible<not_default_constructible>::value,
        "");

    static_assert(
        utility::is_default_constructible<
            nothrow_default_constructible[3]>::value,
        "");
    static_assert(
        utility::is_default_constructible<default_constructible[5]>::value, "");
    static_assert(
        !utility::is_default_constructible<not_default_constructible[8]>::value,
        "");

    check_with<utility::is_default_constructible>();
}
/*
BOOST_AUTO_TEST_CASE (test_utility_is_nothrow_default_constructible) {
    static_assert (utility::is_nothrow_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        not_default_constructible>::value, "");

    static_assert (utility::is_nothrow_default_constructible <
        nothrow_default_constructible [12]>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        default_constructible [12]>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        not_default_constructible [12]>::value, "");

    check_with <utility::is_nothrow_default_constructible> ();
}*/

BOOST_AUTO_TEST_SUITE_END()
