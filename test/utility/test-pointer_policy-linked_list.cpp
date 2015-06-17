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

/** \file
A use case that has particular requirements is linked lists.
Destruction would normally work quite happily, but recursively.
However, in the case of linked lists, recursion might cause stack overflows.
Therefore, pointer_policy has facilities to destruct nodes that have a pointer
to the node type in a loop instead of recursively.

This file tries to cause a stack overflow.
*/

#define BOOST_TEST_MODULE test_utility_pointer_policy_linked_list
#include "utility/test/boost_unit_test.hpp"

#include "utility/small_ptr.hpp"

#include <memory>
#include <iostream>

static constexpr std::size_t blow_up_stack_number = 500000;

template <class Type, class Allocator = std::allocator <Type>>
    class recursive_ptr;

/**
Simple example node for a linked list.
With \a DestructWithLoop set to \c true, destruction should happen iteratively.
With it set to \c false, destruction happens recursively, and a stack overflow
might occur.
*/
template <bool DestructWithLoop> struct node;

template <bool DestructWithLoop> struct node : utility::shared {
    int value_;
    utility::small_ptr <node <DestructWithLoop>> next_;

    node (int value)
    : value_ (value), next_ (std::allocator <node>()) {}
};

// Supply access to the pointer to the next element.
namespace utility { namespace pointer_policy {

    template <> struct move_recursive_next <node <true>> {
        utility::small_ptr <node <true>> &&
            operator() (node <true> * object) const
        { return std::move (object->next_); }
    };

}} // namespace utility::pointer_policy

BOOST_AUTO_TEST_SUITE (test_utility_pointer_policy_linked_list)

template <bool DestructWithLoop> utility::small_ptr <node <DestructWithLoop>>
    make_list (std::size_t number)
{
    typedef node <DestructWithLoop> node_type;
    typedef utility::small_ptr <node <DestructWithLoop>> pointer_type;
    std::allocator <node_type> allocator;
    pointer_type first (allocator);
    pointer_type * current = &first;
    for (std::size_t i = 0; i != number; ++ i) {
        *current = pointer_type::construct (allocator, i);
        current = &(*current)->next_;
    }
    return first;
}

template <bool DestructWithLoop> void test_big_list() {
    std::cout << "Making list." << std::endl;
    auto l = make_list <DestructWithLoop> (blow_up_stack_number);

    std::cout << "Checking list." << std::endl;
    {
        int index = 45;
        auto n = l;
        for (int i = 0; i != index; ++ i)
            n = n->next_;
        BOOST_CHECK_EQUAL (n->value_, 45);
    }

    BOOST_CHECK (l.unique());
    std::cout << "Destructing list." << std::endl;
    l = make_list <DestructWithLoop> (0);
    std::cout << "Done." << std::endl;
}

BOOST_AUTO_TEST_CASE (test_linked_list) {
    // This should not crash.
    test_big_list <true>();

    // This does cause a stack overflow.
    // test_big_list <false>();
}

BOOST_AUTO_TEST_SUITE_END()
