/*
Copyright 2006, 2007, 2009, 2014 Rogier van Dalen.

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
Test that the reference count actually works atomically, by accessing an object
from two different threads.
To test the test, temporarily turn the atomic counter in "shared.hpp" into
"long count" and watch the spectacular crashes.
*/

#define BOOST_TEST_MODULE test_utility_shared_atomicity
#include "utility/test/boost_unit_test.hpp"

#include <memory>

#include "utility/small_ptr.hpp"

#include <boost/thread/thread.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_utility_shared)

class test_object : public utility::shared {
    int i;
public:
    test_object (int i) : i (i) {}

    void increment()  { ++ i; }
    void decrement()  { -- i; }
};

class hammer {
    typedef utility::small_ptr <test_object> small_ptr;
    small_ptr object1;
    small_ptr object2;
public:
    hammer()
    : object1 (small_ptr::construct (std::allocator <test_object>(), 1)),
        object2 (small_ptr::construct (std::allocator <test_object>(), 1))
    {}

    void operator() () const {
        for (int i = 0; i != 10000; ++ i) {
            small_ptr extra_object1 = object1;
            extra_object1->increment();
            small_ptr extra_object2 = object2;
            extra_object2->increment();
        }
        for (int i = 0; i != 10000; ++ i) {
            small_ptr extra_object2 = object2;
            extra_object2->decrement();
            small_ptr extra_object1 = object1;
            extra_object1->decrement();
        }
    }
};

BOOST_AUTO_TEST_CASE (test_utility_shared_atomicity) {
    hammer action;

    // This causes random crashes if the use counter is not atomic.
    boost::thread t1 (std::ref (action));
    boost::thread t2 (std::ref (action));

    t2.join();
    t1.join();
}

BOOST_AUTO_TEST_SUITE_END()
