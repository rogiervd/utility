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

#define BOOST_TEST_MODULE test_utility_pointer_policy
#include "utility/test/boost_unit_test.hpp"

#include <memory>

#include "utility/pointer_policy.hpp"

#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"
#include "utility/test/test_allocator.hpp"

BOOST_AUTO_TEST_SUITE (test_suite_utility_pointer_policy)

// Provide access to the protected methods of Storage.
template <class Storage> struct storage_tester
: Storage {
    storage_tester() : Storage() {}

    template <class ... Arguments>
    explicit storage_tester (Arguments && ... arguments)
    : Storage (std::forward <Arguments> (arguments)...) {}

    typename Storage::value_type * object() const noexcept
    { return Storage::object(); }

    void reset() noexcept { Storage::reset(); }
    void destruct() noexcept { Storage::destruct(); }
    void swap (storage_tester & that) noexcept { Storage::swap (that); }
};

struct base {
    virtual ~base() {}
};

struct derived : base {
    derived (utility::tracked_registry & registry, int i)
    : value (registry, i) {}

    utility::tracked <int> value;
};

BOOST_AUTO_TEST_CASE (test_utility_use_new_delete) {
    typedef utility::pointer_policy::use_new_delete <utility::tracked <int>>
        storage_type;

    utility::tracked_registry registry;

    storage_tester <storage_type> empty;
    BOOST_CHECK (empty.empty());

    storage_tester <storage_type> s1 (
        utility::pointer_policy::construct_as <utility::tracked <int>>(),
        registry, 7);

    BOOST_CHECK (!s1.empty());
    BOOST_CHECK_EQUAL (s1.object()->content(), 7);

    storage_tester <storage_type> s2 (
        utility::pointer_policy::construct_as <utility::tracked <int>>(),
        registry, 53);
    BOOST_CHECK_EQUAL (s2.object()->content(), 53);

    storage_tester <storage_type> s3 (s2);

    BOOST_CHECK_EQUAL (s2.object(), s3.object());

    s2.reset();
    BOOST_CHECK (s2.empty());

    s3.swap (s1);
    BOOST_CHECK_EQUAL (s1.object()->content(), 53);
    BOOST_CHECK_EQUAL (s3.object()->content(), 7);

    s1.destruct();
    s3.destruct();
}

BOOST_AUTO_TEST_CASE (test_utility_use_new_delete_derived) {
    typedef utility::pointer_policy::use_new_delete <base> storage_type;

    utility::tracked_registry registry;

    storage_tester <storage_type> s1 (
        utility::pointer_policy::construct_as <derived>(),
        registry, 7);

    BOOST_CHECK (!s1.empty());

    s1.destruct();
}

BOOST_AUTO_TEST_CASE (test_utility_use_allocator) {
    typedef utility::tracked <int> value_type;
    typedef utility::test_allocator <std::allocator <value_type>>
        allocator_type;
    utility::thrower thrower;
    std::allocator <value_type> std_allocator;
    allocator_type allocator (thrower, std_allocator);
    typedef utility::pointer_policy::use_allocator <value_type, allocator_type>
        storage_type;

    utility::tracked_registry registry;

    storage_tester <storage_type> empty (allocator);
    BOOST_CHECK (empty.empty());

    storage_tester <storage_type> s1 (
        utility::pointer_policy::construct_as <utility::tracked <int>>(),
        allocator, registry, 7);

    BOOST_CHECK (!s1.empty());
    BOOST_CHECK_EQUAL (s1.object()->content(), 7);

    storage_tester <storage_type> s2 (
        utility::pointer_policy::construct_as <utility::tracked <int>>(),
        allocator, registry, 53);
    BOOST_CHECK_EQUAL (s2.object()->content(), 53);

    storage_tester <storage_type> s3 (s2);

    BOOST_CHECK_EQUAL (s2.object(), s3.object());

    s2.reset();
    BOOST_CHECK (s2.empty());

    s3.swap (s1);
    BOOST_CHECK_EQUAL (s1.object()->content(), 53);
    BOOST_CHECK_EQUAL (s3.object()->content(), 7);

    s1.destruct();
    s3.destruct();
}

/* Check exception-safety of allocation. */

void test_use_new_delete_allocation (utility::thrower & thrower) {
    utility::tracked_registry registry;

    typedef utility::throwing <utility::tracked <int>,
        true, true, true, true, true, true> value_type;

    typedef utility::pointer_policy::use_new_delete <value_type>
        storage_type;

    // This may throw but should then deallocate immediately.
    storage_tester <storage_type> s1 (
        utility::pointer_policy::construct_as <value_type>(),
        thrower, registry, 7);

    // If it hasn't thrown, destruct manually.
    s1.destruct();
}

BOOST_AUTO_TEST_CASE (test_use_utility_new_delete_exception) {
    utility::check_all_throw_points (test_use_new_delete_allocation);
}

void test_use_allocator_allocation (utility::thrower & thrower) {
    utility::tracked_registry registry;

    typedef utility::throwing <utility::tracked <int>,
        true, true, true, true, true, true> value_type;

    typedef utility::test_allocator <std::allocator <value_type>>
        allocator_type;
    std::allocator <value_type> std_allocator;
    allocator_type allocator (thrower, std_allocator);

    typedef utility::pointer_policy::use_allocator <value_type, allocator_type>
        storage_type;

    // This may throw but should then deallocate immediately.
    storage_tester <storage_type> s1 (
        utility::pointer_policy::construct_as <value_type>(),
        allocator, thrower, registry, 7);

    // If it hasn't thrown, destruct manually.
    s1.destruct();
}

BOOST_AUTO_TEST_CASE (test_utility_use_allocator_exception) {
    utility::check_all_throw_points (test_use_new_delete_allocation);
}

BOOST_AUTO_TEST_SUITE_END()
