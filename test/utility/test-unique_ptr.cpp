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

#define BOOST_TEST_MODULE test_utility_unique_ptr
#include "utility/test/boost_unit_test.hpp"

#include "utility/unique_ptr.hpp"

#include <type_traits>

#include "utility/test/tracked.hpp"

using utility::tracked;
using utility::tracked_registry;

using utility::shared_from_unique;
using utility::is_unique_ptr;

std::unique_ptr <tracked <int>> get_5 (tracked_registry & r) {
    return std::unique_ptr <tracked <int>> (new tracked <int> (r, 5));
}

/**
List that does not delete its successors when it is destructed.
*/
struct manual_list {
    tracked <int> value_;
    manual_list * next_;

    manual_list (tracked_registry & r, int value, manual_list * next = nullptr)
    : value_ (r, value), next_ (next) {}

    /// Does not delete next.
    ~manual_list() {}
};

struct manual_list_deleter {
    void operator() (manual_list * l) const {
        while (l) {
            manual_list * next = l->next_;
            delete l;
            l = next;
        }
    }
};

BOOST_AUTO_TEST_SUITE(test_utility_unique_ptr)

/* Test make_unique. */

bool exists = false;

struct type {
    type() { exists = true; }
    ~type() { exists = false; }
};

BOOST_AUTO_TEST_CASE (test_utility_make_unique) {
    auto p = utility::make_unique <type>();
    static_assert (std::is_same <decltype (p), std::unique_ptr <type>>::value,
        "");
    BOOST_CHECK (exists);
    p.reset();
    BOOST_CHECK (!exists);
}

/* Test shared_from_unique. */

BOOST_AUTO_TEST_CASE(test_shared_from_unique) {
    tracked_registry r;
    {
        auto five = shared_from_unique (get_5 (r));
        BOOST_CHECK_EQUAL (five->content(), 5);

        std::shared_ptr <tracked <int>> eight = five;
        eight->content() = 8;

        BOOST_CHECK_EQUAL (five->content(), 8);
    }
}

BOOST_AUTO_TEST_CASE(test_shared_from_unique_deleter) {
    tracked_registry r;
    {
        manual_list * last = new manual_list (r, 11);
        // This requires a deleter, otherwise "last" will remain in memory.
        std::unique_ptr <manual_list, manual_list_deleter> l (
            new manual_list (r, 9, last), manual_list_deleter());

        BOOST_CHECK_EQUAL (l->value_.content(), 9);
        BOOST_CHECK_EQUAL (l->next_->value_.content(), 11);

        auto l2 = shared_from_unique (std::move (l));
        BOOST_CHECK (!l);

        BOOST_CHECK_EQUAL (l2->value_.content(), 9);
        BOOST_CHECK_EQUAL (l2->next_->value_.content(), 11);
    }
}

/* Test is_unique_ptr. */

BOOST_AUTO_TEST_CASE(test_is_unique_ptr) {
    // Values.
    static_assert (!is_unique_ptr <void>::value, "");
    static_assert (!is_unique_ptr <int>::value, "");
    static_assert (!is_unique_ptr <manual_list>::value, "");

    // Normal pointers
    static_assert (!is_unique_ptr <void *>::value, "");
    static_assert (!is_unique_ptr <int * const>::value, "");
    static_assert (!is_unique_ptr <manual_list * &>::value, "");

    static_assert (!is_unique_ptr <std::unique_ptr <int> *>::value, "");
    static_assert (!is_unique_ptr <std::unique_ptr <manual_list> *>::value, "");

    // std::shared_ptr.
    static_assert (!is_unique_ptr <std::shared_ptr <void *>>::value, "");
    static_assert (!is_unique_ptr <std::shared_ptr <int * const>>::value, "");
    static_assert (!is_unique_ptr <
        std::shared_ptr <manual_list * &>>::value, "");

    // std::unique_ptr.
    static_assert (is_unique_ptr <std::unique_ptr <void>>::value, "");
    static_assert (is_unique_ptr <std::unique_ptr <int *>>::value, "");
    static_assert (is_unique_ptr <
        std::unique_ptr <manual_list, manual_list_deleter>>::value, "");

    static_assert (is_unique_ptr <std::unique_ptr <void *> const &>::value, "");
    static_assert (is_unique_ptr <
        std::unique_ptr <int, manual_list_deleter> &>::value, "");
    static_assert (is_unique_ptr <
        std::unique_ptr <manual_list> const &>::value, "");

    static_assert (is_unique_ptr <std::unique_ptr <void>>::value, "");
    static_assert (is_unique_ptr <std::unique_ptr <int> const>::value, "");
    static_assert (is_unique_ptr <std::unique_ptr <manual_list>>::value, "");

    // std::unique_ptr with possibly confusing pointee types.
    static_assert (is_unique_ptr <
        std::unique_ptr <std::unique_ptr <int>>>::value, "");
    static_assert (is_unique_ptr <
        std::unique_ptr <std::shared_ptr <int>>>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
