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

#define BOOST_TEST_MODULE test_utility_shared
#include <boost/test/unit_test.hpp>

#include <memory>

#include "utility/test/test_allocator.hpp"
#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

#include "utility/shared.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_shared)

struct shared : utility::shared {};

BOOST_AUTO_TEST_CASE (test_utility_shared_basic) {
    shared s;
    BOOST_CHECK_EQUAL (shared::get_count (&s), 0);
    shared::acquire (&s);
    BOOST_CHECK_EQUAL (shared::get_count (&s), 1);
    shared::acquire (&s);
    BOOST_CHECK_EQUAL (shared::get_count (&s), 2);

    BOOST_CHECK (!shared::release_count (&s));
    BOOST_CHECK_EQUAL (shared::get_count (&s), 1);
    BOOST_CHECK (shared::release_count (&s));
    BOOST_CHECK_EQUAL (shared::get_count (&s), 0);
}

struct test_exceptions {
    template <class ThrowToggles> struct shared_throwing : utility::shared {
        utility::throwing <utility::tracked <int>, ThrowToggles>
            content_;

        shared_throwing (utility::thrower & thrower,
            utility::tracked_registry & registry, int value)
        : content_ (thrower, utility::tracked <int> (registry, value)) {}

        int  & content()  { return content_.content().content(); }
        int const & content() const { return content_.content().content(); }
    };

    template <class ThrowToggles>
        void operator() (utility::thrower & thrower, ThrowToggles) const
    {
        typedef shared_throwing <ThrowToggles> throwing;
        utility::tracked_registry registry;
        {
            // This must be scoped or the registry could go out of scope before
            // this object.
            throwing on_stack (thrower, registry, 7);
        }

        utility::test_allocator <std::allocator <throwing>> allocator (thrower);

        // Allocate and deallocate manually.
        throwing * on_heap = shared::construct <throwing> (
            allocator, thrower, registry, 234);
        BOOST_CHECK_EQUAL (shared::get_count (on_heap), 0);
        BOOST_CHECK_EQUAL (on_heap->content(), 234);
        on_heap->~throwing();
        allocator.deallocate (on_heap, 1);

        // Allocate manually, but then handle with acquire() and release().
        on_heap = shared::construct <throwing> (
            allocator, thrower, registry, 789);
        BOOST_CHECK_EQUAL (shared::get_count (on_heap), 0);
        BOOST_CHECK_EQUAL (on_heap->content(), 789);
        shared::acquire (on_heap);
        BOOST_CHECK_EQUAL (shared::get_count (on_heap), 1);
        shared::acquire (on_heap);
        BOOST_CHECK_EQUAL (shared::get_count (on_heap), 2);
        shared::release (allocator, on_heap);
        BOOST_CHECK_EQUAL (shared::get_count (on_heap), 1);
        shared::release (allocator, on_heap);
        // The object should be destructed now.
    }
};

BOOST_AUTO_TEST_CASE (test_utility_shared_exceptions) {
    // Check only a throwing allocator.
    utility::check_with_some_throw_toggles (test_exceptions());
    // Check all configurations.
    utility::check_with_all_throw_toggles (test_exceptions());
}

BOOST_AUTO_TEST_SUITE_END()
