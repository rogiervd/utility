// SPDX-FileCopyrightText: Copyright 2009, 2014, 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UTILITY_TEST_TEST_ALLOCATOR_HPP_INCLUDED
#define UTILITY_TEST_TEST_ALLOCATOR_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <memory>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>

#include "thrower.hpp"

namespace utility {

namespace detail {

    class test_allocator_registry
    {
        struct allocation
        {
            unsigned index;
            std::reference_wrapper<std::type_info const> type;
            size_t n;

            allocation(unsigned index, std::type_info const & type, size_t n)
            : index(index), type(type), n(n)
            {}
        };

        unsigned current_index;
        std::map<void *, allocation> allocations;

        static void error_leak(std::pair<void *, allocation> const & a)
        {
            if (a.second.n == 1) {
                BOOST_ERROR(
                    "Memory leak: " << a.second.type.get().name()
                                    << " object at " << a.first
                                    << " was not deallocated (allocation no. "
                                    << a.second.index << ").");
            } else {
                BOOST_ERROR(
                    "Memory leak: " << a.second.n << " "
                                    << a.second.type.get().name()
                                    << " objects at " << a.first
                                    << " were not deallocated (allocation no. "
                                    << a.second.index << ").");
            }
        }

    public:
        test_allocator_registry() : current_index(0) {}

        ~test_allocator_registry() { check_done(); }

        void insert(void * p, std::type_info const & type, size_t n)
        {
            if (n) {
                allocations.insert(
                    std::make_pair(p, allocation(current_index, type, n)));
                ++current_index;
            } else {
                BOOST_TEST_MESSAGE(
                    "Allocating zero objects of type " << type.name() << ".");
            }
        }

        void erase(void * p, std::type_info const & type, size_t n)
        {
            std::map<void *, allocation>::iterator i;
            i = allocations.find(p);
            if (i == allocations.end()) {
                BOOST_ERROR(
                    "Trying to deallocate "
                    << n << " " << type.name() << " objects at " << p
                    << " where there was none allocated.");
            } else {
                if (type != i->second.type || n != i->second.n) {
                    BOOST_ERROR(
                        "Trying to deallocate "
                        << n << " " << type.name() << " objects at " << p
                        << " where there were " << i->second.n << " "
                        << i->second.type.get().name()
                        << " objects allocated (allocation no. "
                        << i->second.index << ").");
                }
                allocations.erase(i);
            }
        }

        void check_done() const
        {
            std::for_each(allocations.begin(), allocations.end(), &error_leak);
        }

        unsigned allocation_count() const { return current_index; }
    };

    class test_allocator_checker
    {
        std::shared_ptr<test_allocator_registry> registry;

    public:
        test_allocator_checker()
        {
            registry = std::shared_ptr<test_allocator_registry>(
                new test_allocator_registry);
        }

        ~test_allocator_checker() {}

        void check_allocation(void * p, std::type_info const & type, size_t n)
        {
            registry->insert(p, type, n);
        }

        void check_deallocation(void * p, std::type_info const & type, size_t n)
        {
            registry->erase(p, type, n);
        }

        void check_done() const { registry->check_done(); }
        void throw_point() const {}

        unsigned allocation_count() const
        {
            return registry->allocation_count();
        }

        bool operator==(test_allocator_checker const & other) const
        {
            return registry == other.registry;
        }

        bool operator!=(test_allocator_checker const & other) const
        {
            return registry != other.registry;
        }
    };

    class test_allocator_checker_thrower : public test_allocator_checker
    {
        std::reference_wrapper<utility::thrower> t;

    public:
        explicit test_allocator_checker_thrower(thrower & t) : t(t) {}

        void throw_point() const { t.get().throw_point(); }
    };

}  // namespace detail

/**
Allocator that wraps around a base allocator and checks whether all
allocated objects are properly deallocated.
It also allows for throwing errors.
*/
template <typename BaseAllocator, bool ThrowOnAllocation = true>
class test_allocator
{
    BaseAllocator base_allocator;
    typename boost::mpl::if_c<
        ThrowOnAllocation, detail::test_allocator_checker_thrower,
        detail::test_allocator_checker>::type checker;

    template <class BaseAllocator2, bool ThrowOnAllocation2>
    friend class test_allocator;

    typedef std::allocator_traits<BaseAllocator> base_traits;

public:
    typedef typename base_traits::pointer pointer;
    typedef typename base_traits::const_pointer const_pointer;
    typedef typename base_traits::void_pointer void_pointer;
    typedef typename base_traits::const_void_pointer const_void_pointer;

    typedef typename base_traits::size_type size_type;
    typedef typename base_traits::difference_type difference_type;

    typedef typename base_traits::value_type value_type;
    typedef value_type & reference;
    typedef value_type const & const_reference;

    /**
    rebind just forwards to the base allocator's rebind functionality.
    */
    template <typename Type> struct rebind
    {
        typedef test_allocator<
            typename base_traits::template rebind_alloc<Type>,
            ThrowOnAllocation>
            other;
    };

    explicit test_allocator(thrower & t) noexcept : base_allocator(), checker(t)
    {}

    explicit test_allocator(
        thrower & t, BaseAllocator const & base_allocator) noexcept
    : base_allocator(base_allocator), checker(t)
    {}

    test_allocator(test_allocator const & other) noexcept
    : base_allocator(other.base_allocator), checker(other.checker)
    {}

    test_allocator(test_allocator && other) noexcept
    : base_allocator(std::move(other.base_allocator)), checker(other.checker)
    {}

    template <typename BaseAlloc2> test_allocator(
        test_allocator<BaseAlloc2, ThrowOnAllocation> const & other) noexcept
    : base_allocator(other.base_allocator), checker(other.checker)
    {}

    ~test_allocator() noexcept {}

    test_allocator & operator=(test_allocator const & other)
    {
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
    pointer allocate(size_type n, const_void_pointer hint = nullptr)
    {
        checker.throw_point();
        pointer p = base_traits::allocate(base_allocator, n, hint);
        checker.check_allocation(p, typeid(value_type), n);
        return p;
    }

    // p may not be 0.
    void deallocate(pointer p, size_type n) noexcept
    {
        checker.check_deallocation(p, typeid(value_type), n);

        // Fill the deallocated memory with an unlikely value.
        {
            size_t total_size = n * sizeof(value_type);
            std::uninitialized_fill_n(
                reinterpret_cast<char *>(p), total_size,
                // An unlikely value
                0xed);
        }

        base_traits::deallocate(base_allocator, p, n);
    }

    size_type max_size() const noexcept
    {
        return base_traits::max_size(base_allocator);
    }

    void construct(pointer p, value_type const & t)
    {
        base_traits::construct(base_allocator, p, t);
    }

    void destroy(pointer p) { base_traits::destroy(base_allocator, p); }

    bool operator==(test_allocator const & other) const
    {
        if (checker == other.checker) {
            assert(base_allocator == other.base_allocator);
            return true;
        } else {
            return false;
        }
    }

    bool operator!=(test_allocator const & other) const
    {
        return !(*this == other);
    }

    void check_done() const { checker.check_done(); }

    unsigned allocation_count() { return checker.allocation_count(); }

    friend void swap(test_allocator & left, test_allocator & right) noexcept
    {
        using std::swap;
        swap(left.base_allocator, right.base_allocator);
        swap(left.checker, right.checker);
    }
};

}  // namespace utility

#endif  // UTILITY_TEST_TEST_ALLOCATOR_HPP_INCLUDED
