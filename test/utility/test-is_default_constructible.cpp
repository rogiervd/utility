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

#define BOOST_TEST_MODULE test_utility_is_default_constructible
#include "utility/test/boost_unit_test.hpp"

#include "utility/is_default_constructible.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_is_default_constructible)

struct default_constructible {
    std::shared_ptr <int> a;

    default_constructible() : a (std::make_shared <int> (6)) {}
};

struct nothrow_default_constructible {
    nothrow_default_constructible() noexcept {}
};

struct not_default_constructible {
    not_default_constructible() = delete;
};

struct abstract {
    virtual void method() = 0;
};

template <template <class> class Predicate> void check_with() {
    static_assert (!Predicate <void>::value, "");
    static_assert (Predicate <int>::value, "");
    static_assert (Predicate <int const>::value, "");
    static_assert (Predicate <void *>::value, "");
    static_assert (!Predicate <int &>::value, "");
    static_assert (!Predicate <int const &>::value, "");
    static_assert (!Predicate <int &&>::value, "");
    static_assert (!Predicate <int const &&>::value, "");

    static_assert (Predicate <int [5]>::value, "");

    typedef int function_type (float);
    function_type f;

    static_assert (Predicate <int (float)>::value, "");
    static_assert (!Predicate <int (&) (float)>::value, "");
    static_assert (Predicate <int (*) (float)>::value, "");

    static_assert (!Predicate <abstract>::value, "");
}

BOOST_AUTO_TEST_CASE (test_utility_is_default_constructible) {
    static_assert (utility::is_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (utility::is_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_default_constructible <
        not_default_constructible>::value, "");

    static_assert (utility::is_default_constructible <
        nothrow_default_constructible [3]>::value, "");
    static_assert (utility::is_default_constructible <
        default_constructible[5]>::value, "");
    static_assert (!utility::is_default_constructible <
        not_default_constructible[8]>::value, "");

    check_with <utility::is_default_constructible> ();
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
