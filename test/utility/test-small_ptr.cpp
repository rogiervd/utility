/*
Copyright 2006, 2007, 2009, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_utility_small_ptr
#include "utility/test/boost_unit_test.hpp"

#include <memory>

#include "utility/test/test_allocator.hpp"
#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

#include "utility/small_ptr.hpp"

BOOST_AUTO_TEST_SUITE(test_suite_utility_small_ptr)

template <bool t1, bool t2, bool t3, bool t4, bool t5, bool t6>
    class shared_object;

template <bool t1, bool t2, bool t3, bool t4, bool t5, bool t6>
    class shared_object : public utility::shared
{
    typedef utility::test_allocator <std::allocator <shared_object>, true>
        test_allocator;
public:
    /**
    Initialise with a value but not a
    */
    shared_object (utility::thrower & thrower,
        utility::tracked_registry & registry, int value)
        : value_ (thrower, utility::tracked <int> (registry, value)),
        next_ (test_allocator (thrower)) {}

    shared_object (utility::thrower & thrower,
        utility::tracked_registry & registry, int value,
        utility::small_ptr <shared_object, test_allocator> const & next)
        : value_ (thrower, utility::tracked <int> (registry, value)),
        next_ (next) {}

    ~shared_object() noexcept {}

    int value() { return value_.content().content(); }

    void set_next (utility::small_ptr <shared_object> const & n)
    { next_ = n; }

    utility::small_ptr <shared_object, test_allocator> const & next() const
    { return next_; }

    utility::small_ptr <shared_object, test_allocator> & next()
    { return next_; }

private:
    utility::throwing <utility::tracked <int>, t1, t2, t3, t4, t5, t6> value_;
    utility::small_ptr <shared_object, test_allocator> next_;
};

/**
Assuming operator == works correctly and operator < does something, test whether
the other comparison operators are consistent with them.
*/
template <class Type> void check_comparison (Type const & o1, Type const o2) {
    if (o1 == o2) {
        BOOST_CHECK (! (o1 != o2));
        BOOST_CHECK (! (o1 < o2));
        BOOST_CHECK (! (o1 > o2));
        BOOST_CHECK (o1 <= o2);
        BOOST_CHECK (o1 >= o2);
    } else {
        if (o1 < o2) {
            BOOST_CHECK (o1 != o2);
            BOOST_CHECK (!(o1 > o2));
            BOOST_CHECK (o1 < o2);
            BOOST_CHECK (!(o1 >= o2));
        } else {
            BOOST_CHECK (o1 != o2);
            BOOST_CHECK (o1 > o2);
            BOOST_CHECK (! (o1 < o2));
            BOOST_CHECK (o1 >= o2);
        }
    }
}

template <bool t1, bool t2, bool t3, bool t4, bool t5, bool t6>
void test_small_ptr (utility::thrower & thrower)
{
    typedef shared_object <t1, t2, t3, t4, t5, t6> shared_object;
    typedef utility::test_allocator <std::allocator <shared_object>, true>
        test_allocator;
    utility::tracked_registry registry;
    test_allocator allocator (thrower);
    typedef utility::small_ptr <shared_object, test_allocator> small_ptr;
    {
        test_allocator allocator1 (thrower);
        auto empty1 = small_ptr (allocator1);
        BOOST_CHECK (empty1.empty());
        BOOST_CHECK (empty1 == empty1);
        BOOST_CHECK (!empty1);
        BOOST_CHECK (!empty1.unique());
        BOOST_CHECK_EQUAL (empty1.use_count(), 0);
        BOOST_CHECK (empty1.allocator() == allocator1);
        BOOST_CHECK (empty1.get() == nullptr);

        test_allocator allocator2 (thrower);
        auto empty2 = small_ptr (allocator2);
        BOOST_CHECK (empty2.empty());
        BOOST_CHECK (!empty2);
        BOOST_CHECK (empty2 == empty2);
        BOOST_CHECK (empty1 == empty2);
        BOOST_CHECK (!empty2.unique());
        BOOST_CHECK (empty2.allocator() == allocator2);

        // Copy empty to empty.
        {
            auto empty3 = small_ptr (allocator2);
            BOOST_CHECK (empty3.allocator() == empty2.allocator());

            empty3 = empty1;
            BOOST_CHECK (empty3.empty());
            BOOST_CHECK (empty3.allocator() == allocator1);
        }
        // Move empty to empty
        {
            auto empty3 = small_ptr (allocator2);
            BOOST_CHECK (empty3.allocator() == empty2.allocator());

            empty3 = std::move (empty1);
            BOOST_CHECK (empty3.empty());
            BOOST_CHECK (empty3.allocator() == allocator1);
        }
    }
    {
        test_allocator allocator1 (thrower);
        auto empty1 = small_ptr (allocator1);

        test_allocator allocator2 (thrower);
        auto empty2 = small_ptr (allocator2);

        BOOST_CHECK (empty1.allocator() != empty2.allocator());

        // Constructing from a pointer is possible, but not recommended.
        shared_object * object_pointer_1;
        object_pointer_1 = allocator1.allocate (1);
        try {
            new (object_pointer_1) shared_object (thrower, registry, 26);
        } catch (...) {
            allocator1.deallocate (object_pointer_1, 1);
            throw;
        }
        small_ptr object1 = small_ptr (object_pointer_1, allocator1);
        BOOST_CHECK (!object1.empty());
        BOOST_CHECK_EQUAL (object1.get(), object_pointer_1);
        BOOST_CHECK_EQUAL (object1.use_count(), 1);

        BOOST_CHECK_EQUAL (object1->value(), 26);

        shared_object * object_pointer_2;
        object_pointer_2 = allocator1.allocate (1);
        try {
            new (object_pointer_2) shared_object (thrower, registry, 126);
        } catch (...) {
            allocator1.deallocate (object_pointer_2, 1);
            throw;
        }
        small_ptr object2 = small_ptr (object_pointer_2, allocator1);
        BOOST_CHECK (!object2.empty());
        BOOST_CHECK_EQUAL (object2->value(), 126);
        BOOST_CHECK (object2.unique());
        BOOST_CHECK_EQUAL (object2.use_count(), 1);

        small_ptr object2_again (object_pointer_2, allocator1);
        BOOST_CHECK_EQUAL (object2_again->value(), 126);
        BOOST_CHECK (!object2.unique());
        BOOST_CHECK (!object2_again.unique());
        BOOST_CHECK_EQUAL (object2.use_count(), 2);
    }
    {
        test_allocator allocator1 (thrower);
        test_allocator allocator2 (thrower);
        small_ptr empty1 (allocator1);
        small_ptr p1 = small_ptr::construct (allocator1, thrower, registry, 75);
        small_ptr p2 = small_ptr::construct (allocator2, thrower, registry, 93);

        BOOST_CHECK (!p1.empty());
        BOOST_CHECK (!!p1);
        BOOST_CHECK (!p2.empty());
        BOOST_CHECK (!!p2);
        BOOST_CHECK (p1.unique());
        BOOST_CHECK (p2.unique());
        BOOST_CHECK (p1 != empty1);
        BOOST_CHECK (p1 != p2);
        BOOST_CHECK_EQUAL (p1.use_count(), 1);
        BOOST_CHECK_EQUAL (p2.use_count(), 1);

        BOOST_CHECK_EQUAL (p1->value(), 75);
        BOOST_CHECK_EQUAL (p2->value(), 93);
        BOOST_CHECK_EQUAL ((*p1).value(), 75);
        BOOST_CHECK_EQUAL ((*p2).value(), 93);

        BOOST_CHECK (p1.allocator() == empty1.allocator());
        BOOST_CHECK (p1.allocator() != p2.allocator());
        BOOST_CHECK (p2.allocator() != empty1.allocator());

        // swap member.
        p1.swap (p2);
        BOOST_CHECK_EQUAL (p1->value(), 93);
        BOOST_CHECK_EQUAL (p2->value(), 75);

        BOOST_CHECK (p1 != p2);

        BOOST_CHECK (p2.allocator() == empty1.allocator());
        BOOST_CHECK (p1.allocator() != p2.allocator());
        BOOST_CHECK (p1.allocator() != empty1.allocator());

        // Copy construction: shared object.
        small_ptr p3 (p1);
        BOOST_CHECK (p1 == p3);
        BOOST_CHECK (!p3.empty());
        BOOST_CHECK (!p1.unique());
        BOOST_CHECK (!p3.unique());
        BOOST_CHECK_EQUAL (p1.use_count(), 2);
        BOOST_CHECK_EQUAL (p3.use_count(), 2);
        BOOST_CHECK_EQUAL (p3->value(), 93);
        BOOST_CHECK (p3.allocator() == p1.allocator());

        check_comparison (p1, p3);
        check_comparison (p3, p1);
        check_comparison (p2, p3);
        check_comparison (p1, p2);

        // Move construction.
        small_ptr p4 (std::move (p2));
        BOOST_CHECK (p2.empty());
        BOOST_CHECK (!p2);
        BOOST_CHECK (p4 != p1);
        BOOST_CHECK (p4 != p2);
        BOOST_CHECK (p4.unique());
        BOOST_CHECK_EQUAL (p4->value(), 75);
        // The allocator remains, though.
        BOOST_CHECK (p4.allocator() == p2.allocator());
        BOOST_CHECK (p4.allocator() != p1.allocator());

        // Copy assignment if the assignee is empty.
        {
            small_ptr p5 (allocator2);
            BOOST_CHECK (p5.empty());
            BOOST_CHECK (!p5);
            BOOST_CHECK (p5.allocator() != p4.allocator());
            p5 = p4;
            BOOST_CHECK (p5 == p4);
            BOOST_CHECK (!p4.unique());
            BOOST_CHECK (!p5.unique());
            BOOST_CHECK_EQUAL (p5->value(), 75);
            BOOST_CHECK (p5.allocator() == p4.allocator());
        }
        // Copy assignment if the assignee is not empty.
        {
            small_ptr p5 = small_ptr::construct (allocator2,
                thrower, registry, -2);
            BOOST_CHECK (!p5.empty());
            BOOST_CHECK (p5.unique());
            BOOST_CHECK (p5.allocator() != p4.allocator());
            p5 = p4;
            BOOST_CHECK (p5 == p4);
            BOOST_CHECK (!p4.unique());
            BOOST_CHECK (!p5.unique());
            BOOST_CHECK_EQUAL (p5->value(), 75);
            BOOST_CHECK (p5.allocator() == p4.allocator());
        }

        // Move assignment if the assignee is empty.
        small_ptr p5 (allocator2);
        BOOST_CHECK (p5.allocator() != p4.allocator());
        p5 = std::move (p4);
        BOOST_CHECK (p4.empty());
        BOOST_CHECK (!p5.empty());
        BOOST_CHECK (p5.unique());
        BOOST_CHECK (p5 != p4);
        BOOST_CHECK_EQUAL (p5->value(), 75);
        BOOST_CHECK (p5.allocator() == p4.allocator());

        // swap free function.
        BOOST_CHECK (p1 == p3);
        using std::swap; // Should not be picked up.
        swap (p1, p2);
        BOOST_CHECK (p2 == p3);
        BOOST_CHECK (p1 != p3);
    }

    // Check self-assignment.
    {
        test_allocator allocator1 (thrower);

        small_ptr p = small_ptr::construct (allocator1, thrower, registry, -23);
        BOOST_CHECK (!p.empty());
        BOOST_CHECK_EQUAL (p.use_count(), 1);
        BOOST_CHECK_EQUAL (p->value(), -23);

        // Self copy assignment.
        p = p;
        BOOST_CHECK (!p.empty());
        BOOST_CHECK_EQUAL (p.use_count(), 1);
        BOOST_CHECK_EQUAL (p->value(), -23);

        // Self move assignment.
        p = std::move (p);
        BOOST_CHECK (!p.empty());
        BOOST_CHECK_EQUAL (p.use_count(), 1);
        BOOST_CHECK_EQUAL (p->value(), -23);

        // Self swap: member function.
        p.swap (p);
        BOOST_CHECK (!p.empty());
        BOOST_CHECK_EQUAL (p.use_count(), 1);
        BOOST_CHECK_EQUAL (p->value(), -23);

        // Self swap: member function.
        swap (p, p);
        BOOST_CHECK (!p.empty());
        BOOST_CHECK_EQUAL (p.use_count(), 1);
        BOOST_CHECK_EQUAL (p->value(), -23);
    }

    // Check singly-linked lists
    {
        small_ptr list = small_ptr::construct (allocator,
            thrower, registry, 10);
        BOOST_CHECK_EQUAL (list->value(), 10);
        // Prepend 11.
        list = small_ptr::construct (allocator, thrower, registry, 11, list);
        BOOST_CHECK_EQUAL (list->value(), 11);
        BOOST_CHECK_EQUAL (list->next()->value(), 10);
        // Prepend 12, moving.
        list = small_ptr::construct (allocator,
            thrower, registry, 12, std::move (list));
        BOOST_CHECK_EQUAL (list->value(), 12);
        BOOST_CHECK_EQUAL (list->next()->value(), 11);
        // Prepend 12, moving.
        list = small_ptr::construct (allocator,
            thrower, registry, 13, std::move (list));
        BOOST_CHECK_EQUAL (list->value(), 13);
        BOOST_CHECK_EQUAL (list->next()->value(), 12);
        BOOST_CHECK_EQUAL (list->next()->next()->value(), 11);
        BOOST_CHECK_EQUAL (list->next()->next()->next()->value(), 10);
        BOOST_CHECK (!list->next()->next()->next()->next());

        // Pop the front.
        list = list->next();
        BOOST_CHECK_EQUAL (list->value(), 12);
        BOOST_CHECK_EQUAL (list->next()->value(), 11);

        list = std::move (list->next());
        BOOST_CHECK_EQUAL (list->value(), 11);
        BOOST_CHECK_EQUAL (list->next()->value(), 10);

        // This is not proper use of the shared_object class, to move one of
        // its members, but it does have defined behaviour in this case.
        small_ptr new_list = std::move (list->next());
        BOOST_CHECK_EQUAL (list->value(), 11);
        BOOST_CHECK (!list->next());

        BOOST_CHECK_EQUAL (new_list->value(), 10);
        BOOST_CHECK (!new_list->next());
    }
}

BOOST_AUTO_TEST_CASE (test_utility_small_ptr) {
    // This is, assuming that std::allocator is empty.
    BOOST_CHECK_EQUAL (
        sizeof (utility::small_ptr <std::allocator <int> >),
        sizeof (void *));

    utility::check_all_throw_points (
        test_small_ptr <false, false, false, false, false, false>);
}

/** Check all configurations for throwing. */
// (Not actually used at the moment.)
template <int throw_bits> void check_small_ptr_all_throw_points() {
    utility::check_all_throw_points (test_small_ptr <
        !(throw_bits & 0x01), !(throw_bits & 0x02), !(throw_bits & 0x04),
        !(throw_bits & 0x08), !(throw_bits & 0x10), !(throw_bits & 0x20)>);
    check_small_ptr_all_throw_points <throw_bits - 1>();
}

template <> void check_small_ptr_all_throw_points <-1>() {}

BOOST_AUTO_TEST_CASE (test_utility_small_ptr_exception_safety) {
//    check_small_ptr_all_throw_points <64>();
    utility::check_all_throw_points (
        test_small_ptr <true, false, false, false, false, false>);
    utility::check_all_throw_points (
        test_small_ptr <false, true, false, false, false, false>);
    utility::check_all_throw_points (
        test_small_ptr <false, false, true, false, false, false>);
    utility::check_all_throw_points (
        test_small_ptr <false, false, false, true, false, false>);
    utility::check_all_throw_points (
        test_small_ptr <false, false, false, false, true, false>);
    utility::check_all_throw_points (
        test_small_ptr <false, false, false, false, false, true>);

    utility::check_all_throw_points (
        test_small_ptr <true, true, true, true, true, true>);
}

BOOST_AUTO_TEST_SUITE_END()
