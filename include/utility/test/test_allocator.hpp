/*
Copyright 2009, 2014 Rogier van Dalen.

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

#ifndef UTILITY_TEST_TEST_ALLOCATOR_HPP_INCLUDED
#define UTILITY_TEST_TEST_ALLOCATOR_HPP_INCLUDED

#include <map>
#include <memory>
#include <algorithm>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>

#include "thrower.hpp"

namespace utility {

    namespace detail {

        class test_allocator_registry {
            struct allocation {
                unsigned index;
                std::reference_wrapper <const std::type_info> type;
                size_t n;

                allocation (unsigned index, const std::type_info & type,
                    size_t n)
                : index (index), type (type), n (n) {}
            };

            unsigned current_index;
            std::map <void *, allocation> allocations;

            static void error_leak (const std::pair <void *, allocation> & a) {
                if (a.second.n == 1) {
                    BOOST_ERROR ("Memory leak: " << a.second.type.get().name()
                        << " object at " << a.first
                        << " was not deallocated (allocation no. "
                        << a.second.index << ").");
                } else {
                    BOOST_ERROR ("Memory leak: " << a.second.n << " "
                        << a.second.type.get().name() << " objects at "
                        << a.first << " were not deallocated (allocation no. "
                        << a.second.index << ").");
                }
            }

        public:
            test_allocator_registry() : current_index (0) {}

            ~test_allocator_registry() { check_done(); }

            void insert (void * p, const std::type_info & type, size_t n) {
                if (n) {
                    allocations.insert (std::make_pair
                        (p, allocation (current_index, type, n)));
                    ++ current_index;
                } else {
                    BOOST_MESSAGE ("Allocating zero objects of type "
                        << type.name() << ".");
                }
            }

            void erase (void * p, const std::type_info & type, size_t n) {
                std::map <void *, allocation>::iterator i;
                i = allocations.find (p);
                if (i == allocations.end()) {
                    BOOST_ERROR ("Trying to deallocate " << n << " "
                        << type.name() << " objects at " << p
                        << "where there was none allocated.");
                } else {
                    if (type != i->second.type || n != i->second.n) {
                        BOOST_ERROR ("Trying to deallocate " << n << " "
                            << type.name() << " objects at " << p
                            << " where there were " << i->second.n << " "
                            << i->second.type.get().name()
                            << " objects allocated (allocation no. "
                            << i->second.index << ").");
                    }
                    allocations.erase (i);
                }
            }

            void check_done() const {
                std::for_each (allocations.begin(), allocations.end(),
                    &error_leak);
            }

            unsigned allocation_count() const { return current_index; }
        };

        class test_allocator_checker {
            std::shared_ptr <test_allocator_registry> registry;
        public:
            test_allocator_checker() {
                registry = std::shared_ptr <test_allocator_registry> (
                    new test_allocator_registry);
            }

            test_allocator_checker (const test_allocator_checker & other)
            : registry (other.registry) {}

            ~test_allocator_checker() {}

            void check_allocation (
                void * p, const std::type_info & type, size_t n)
            { registry->insert (p, type, n); }

            void check_deallocation (
                void * p, const std::type_info & type, size_t n)
            { registry->erase (p, type, n); }

            void check_done() const { registry->check_done(); }
            void throw_point() const {}

            unsigned allocation_count() const
            { return registry->allocation_count(); }

            bool operator == (test_allocator_checker const & other) const
            { return registry == other.registry; }

            bool operator != (test_allocator_checker const & other) const
            { return registry != other.registry; }
        };

        class test_allocator_checker_thrower : public test_allocator_checker {
            std::reference_wrapper <utility::thrower> t;
        public:
            explicit test_allocator_checker_thrower (thrower & t)
            : t (t) {}

            test_allocator_checker_thrower
                (const test_allocator_checker_thrower & other)
            : test_allocator_checker (other), t (other.t) {}

            void throw_point() const { t.get().throw_point(); }
        };

    } // namespace detail

    /**
    Allocator that wraps around a base allocator and checks whether all
    allocated objects are properly deallocated.
    It also allows for throwing errors.

    \todo Once the Department's system has compilers that come with it, start
    using std::allocator_traits.
    */
    template <typename BaseAllocator, bool ThrowOnAllocation = true>
        class test_allocator
    {
        BaseAllocator base_allocator;
        typename boost::mpl::if_c <ThrowOnAllocation,
                detail::test_allocator_checker_thrower,
                detail::test_allocator_checker
            >::type
            checker;

        template <class BaseAllocator2, bool ThrowOnAllocation2>
            friend class test_allocator;
    public:
        // Not all compilers have std::allocator_traits.

        /*
        typedef typename boost::allocator_traits <BaseAllocator>::pointer
            pointer;
        typedef typename boost::allocator_traits <BaseAllocator>::const_pointer
            const_pointer;
        typedef typename boost::allocator_traits <BaseAllocator>::void_pointer
            void_pointer;
        typedef typename boost::allocator_traits <BaseAllocator>
            ::const_void_pointer const_void_pointer;

        typedef typename boost::allocator_traits <BaseAllocator>::size_type
            size_type;
        typedef typename boost::allocator_traits <BaseAllocator>
            ::difference_type difference_type
        */

        typedef typename BaseAllocator::pointer pointer;
        typedef typename BaseAllocator::const_pointer const_pointer;

        typedef typename BaseAllocator::size_type size_type;
        typedef typename BaseAllocator::difference_type difference_type;

        typedef typename BaseAllocator::reference reference;
        typedef typename BaseAllocator::const_reference const_reference;
        typedef typename BaseAllocator::value_type value_type;

        /**
        rebind just forwards to the base allocator's rebind functionality.
        */
        template <typename Type> struct rebind {
            typedef test_allocator
                <typename BaseAllocator::template rebind <Type>::other,
                    ThrowOnAllocation>
                other;
        };

        test_allocator (thrower & t) noexcept
        : base_allocator(), checker (t) {}

        explicit test_allocator (thrower & t,
            BaseAllocator const & base_allocator) noexcept
        : base_allocator (base_allocator), checker (t) {}

        test_allocator (test_allocator const & other) noexcept
        : base_allocator (other.base_allocator), checker (other.checker) {}

        test_allocator (test_allocator && other) noexcept
        : base_allocator (std::move (other.base_allocator)),
            checker (other.checker) {}

        template <typename BaseAlloc2>
        test_allocator (const test_allocator <BaseAlloc2, ThrowOnAllocation> &
            other) noexcept
        : base_allocator (other.base_allocator), checker (other.checker)
        {}

        ~test_allocator() noexcept {}

        test_allocator & operator = (test_allocator const & other) {
            checker = other.checker;
            return *this;
        }

        // pointer address (reference r) const {
        //     return base_allocator.address (r);
        // }

        // const_pointer address (const_reference r) const {
        //     return base_allocator.address (r);
        // }

        /**
        n may be 0, but the result is undefined.
        */
        pointer allocate (size_type n, const void * hint = 0) {
            checker.throw_point();
            // allocate with hint is optional though!
            pointer p = base_allocator.allocate (n, hint);
            checker.check_allocation (p, typeid (value_type), n);
            return p;
        }

        // p may not be 0.
        void deallocate (pointer p, size_type n) noexcept {
            checker.check_deallocation (p, typeid (value_type), n);

            // Fill the deallocated memory with an unlikely value.
            {
                size_t total_size = n * sizeof (value_type) ;
                std::uninitialized_fill_n (
                    reinterpret_cast <char *> (p),
                    total_size,
                    // An unlikely value
                    0xed);
            }

            base_allocator.deallocate (p, n);
        }

        size_type max_size() const noexcept
        { return base_allocator.max_size(); }

        void construct (pointer p, const value_type & t)
        { base_allocator.construct (p, t); }

        void destroy (pointer p)
        { base_allocator.destroy (p); }

        bool operator == (const test_allocator & other) const {
            if (checker == other.checker) {
                assert (base_allocator == other.base_allocator);
                return true;
            } else
                return false;
        }

        bool operator != (const test_allocator & other) const
        { return ! (*this == other); }

        void check_done() const { checker.check_done(); }

        unsigned allocation_count() { return checker.allocation_count(); }
    };

} // namespace utility

#endif  // UTILITY_TEST_TEST_ALLOCATOR_HPP_INCLUDED
