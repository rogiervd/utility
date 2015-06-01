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

#ifndef UTILITY_SHARED_HPP_INCLUDED
#define UTILITY_SHARED_HPP_INCLUDED

#include <new>
#include <type_traits>

#include <boost/detail/atomic_count.hpp>
#include <boost/utility/enable_if.hpp>

#include "with_allocator.hpp"

namespace utility {

    class shared;

    /**
    Base class for reference-counted objects.
    \sa small_ptr
    */
    class shared {
    private:
        boost::detail::atomic_count count;

    protected:
        shared() noexcept : count (0l) {}

        // The copy and move constructors mustn't copy the count.
        shared (shared const &) noexcept : count (0l) {}
        shared (shared &&) noexcept : count (0l) {}

        // Assignment mustn't copy the count.
        shared & operator = (shared const &) noexcept { return *this; }
        shared & operator = (shared &&) noexcept { return *this; }

    public:
        /**
        Register as an owner of the object.
        Call release_count later to der
        */
        static void acquire (shared * s) noexcept { ++ s->count; }

        /**
        Release a reference to a shared object.
        \return true iff this was the last reference, i.e. iff the object must
        be deleted.
        */
        static bool release_count (shared * s) noexcept
        { return ! -- s->count; }

        /**
        \return The current number of owners of this object.
        */
        static long get_count (shared const * s) noexcept { return s->count; }

        /**
        Use an allocator to construct an object of type "Type" and return a
        pointer.
        This provides strong exception-safety.
        If construction fails with an exception, the memory will be deallocated.
        */
        template <class Type, class Allocator, class ... Arguments>
            static Type *
            construct (Allocator & allocator, Arguments && ... arguments)
        {
            static_assert (std::is_base_of <shared, Type>::value,
                "Type must be derived from 'shared'.");
            static_assert (std::is_same <typename Allocator::value_type,
                typename std::decay <Type>::type>::value,
                "The allocator value_type must match small_ptr's Type.");

            // This works in two steps, allocating memory and constructing the
            // object.
            // That gives a risk of memory leaks.

            // Step 1: allocate memory.
            // This might fail with, say, std::bad_alloc, but then no memory
            // is leaked.
            Type * object = allocator.allocate (1);

            // Step 2: construct the object.
            // But first tell our guard to deallocate the object if something
            // goes wrong.
            static bool const is_safe = noexcept (
                Type (std::forward <Arguments> (arguments) ...));
            guard <Type, Allocator, is_safe> g (object, allocator);
            new (object) Type (std::forward <Arguments> (arguments) ...);
            // If we end up here, construction has succeeded, and the memory
            // does not need to be deallocated.
            g.dismiss();

            return object;
        }

        /**
        Release ownership of the object.
        If the ownership count drops to zero, destruct the object and deallocate
        the memory from the allocator.
        Does not set \a object to zero.
        */
        template <class Type, class Allocator>
        static void release (Allocator & allocator, Type * object) noexcept {
            // This seems to always on GCC 4.6.
            // static_assert (noexcept (object->~Type()),
            //     "Type must have a no-throw destructor.");
            if (release_count (object)) {
                // object must be deleted
                object->~Type();
                allocator.deallocate (object, 1);
            }
        }

    private:
        /**
        Guard that deallocates memory if an exception is thrown.
        */
        template <class Type, class Allocator, bool safe> class guard {
            Type * object;
            Allocator & allocator;
            bool set;
        public:
            guard (Type * object, Allocator & allocator) noexcept
            : object (object), allocator (allocator), set (true) {}

            ~guard() noexcept {
                if (set)
                    allocator.deallocate (object, 1);
            }

            void dismiss() noexcept { set = false; }
        };

        // Version for non-throwing constructors.
        template <class Type, class Allocator>
            class guard <Type, Allocator, true>
        {
        public:
            guard (Type *, Allocator &) noexcept {}
            ~guard() noexcept {}
            void dismiss() noexcept {}
        };

    };

} // namespace utility

#endif // UTILITY_SHARED_HPP_INCLUDED
