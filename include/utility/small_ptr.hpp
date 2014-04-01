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

#ifndef UTILITY_SMALL_PTR_HPP_INCLUDED
#define UTILITY_SMALL_PTR_HPP_INCLUDED

#include <algorithm> // For std::swap
#include <memory>
#include <type_traits>

#include <boost/mpl/if.hpp>

#include "with_allocator.hpp"
#include "shared.hpp"

namespace utility {

    template <class Type, class Allocator = std::allocator <Type>>
        class small_ptr;

    template <class Type>
        void swap_around (small_ptr <Type> & p1,
            small_ptr <Type> & p2, small_ptr <Type> & p3);

    /**
    Smart pointer to an object of known type.
    Since it uses an allocator, it has a fixed type.
    It is meant as an efficient type to be used inside containers.
    */
    template <class Type, class Allocator> class small_ptr
    : private with_allocator <Allocator>
    {
    public:
        typedef Type value_type;
        typedef Allocator allocator_type;

    private:
        // Only check this when the class is fully instantiated.
        static void static_checks() {
            static_assert (std::is_base_of <shared, Type>::value,
                "Type must be derived from 'shared'.");
            static_assert (std::is_same <typename Allocator::value_type,
                typename std::decay <value_type>::type>::value,
                "The allocator value_type must match small_ptr's Type.");
        }

        typedef with_allocator <Allocator> with_allocator_type;

        Type * object;

    public:
        /**
        Construct empty.
        */
        explicit small_ptr (allocator_type const & allocator)
        noexcept (noexcept (with_allocator_type (allocator)))
            : with_allocator_type (allocator), object (nullptr) {}

        /**
        Construct with an object, which must have been allocated with
        \c allocator.
        The object's owner count is increased.
        It is possible to call this on a pointer that another small_ptr already
        owns; sharing will work.
        Of course, the allocator must be the same.
        However, this would be a great opportunity for bugs.
        Instead, it is usually best to use \c construct.
        */
        explicit small_ptr (Type * object, allocator_type const & allocator)
        noexcept (noexcept (with_allocator_type (allocator)))
            : with_allocator_type (allocator), object (object)
        {
            static_checks();
            shared::acquire (object);
        }

        /**
        The preferred way of constructing an object of type Type, returning an
        owner.
        This takes the allocator by value because it will be copied to the
        small_ptr anyway.
        */
        template <class ... Arguments>
            static small_ptr
            construct (Allocator allocator, Arguments && ... arguments)
        {
            Type * object = shared::construct <Type> (
                allocator, std::forward <Arguments> (arguments) ...);
            return small_ptr (object, std::move (allocator));
        }

        /**
        Copy-construct from another small_ptr.
        This increased the use count on the underlying object.
        */
        small_ptr (small_ptr const & p)
            : with_allocator_type (p.allocator()), object (p.object)
        {
            if (object)
                shared::acquire (object);
        }

        /**
        Move-construct from another small_ptr.
        The original pointer will be empty.
        No atomic operations are used.
        */
        small_ptr (small_ptr && p)
            : with_allocator_type (std::move (p.allocator())),
            object (p.object)
        { p.object = nullptr; }

        ~small_ptr() {
            if (object)
                shared::release (with_allocator_type::allocator(), object);
        }

        /**
        Copy assignment.
        This also copies the allocator.
        */
        small_ptr & operator = (const small_ptr & that) {
            // Save the new pointer and allocator temparily because they may be
            // destructed when the old object is destructed (e.g. in a linked
            // list).
            Type * new_object = that.object;
            allocator_type new_allocator (that.allocator());
            if (new_object)
                shared::acquire (new_object);
            if (object)
                shared::release (with_allocator_type::allocator(), object);
            with_allocator_type::allocator() = new_allocator;
            object = new_object;

            return *this;
        }

        /**
        Move assignment.
        This also copies the allocator.
        */
        small_ptr & operator = (small_ptr && that) {
            // Save the new pointer and allocator temparily because they may be
            // destructed when the old object is destructed (e.g. in a linked
            // list).
            Type * new_object = that.object;
            allocator_type new_allocator (that.allocator());
            that.object = nullptr;
            if (object)
                shared::release (with_allocator_type::allocator(), object);
            with_allocator_type::allocator() = new_allocator;
            object = new_object;

            return *this;
        }

        Type & operator * () const { return *object; }

        Type * operator -> () const { return object; }

        /**
        \return A normal pointer to the owned object, or nullptr if this is
        empty.
        */
        Type * get() const { return object; }

        /**
        \return true iff this owns an object.
        */
        bool empty() const { return object == nullptr; }

        explicit operator bool() const { return !empty(); }

        /**
        \return The number of small_ptr's owning the object this owns.
        0 if this is empty.
        */
        long use_count() const {
            if (object)
                return shared::get_count (object);
            else
                return 0;
        }

        /**
        \return true iff this is the only owner of the object.
        If this is empty, return false.
        */
        bool unique() const { return use_count() == 1; }

        /**
        \return The allocator in current use.
        */
        allocator_type const & allocator() const
        { return with_allocator_type::allocator(); }

        /**
        Swap the object and allocator with \a that.
        */
        void swap (small_ptr & that) {
            using std::swap;
            swap (with_allocator_type::allocator(),
                that.with_allocator_type::allocator());
            swap (this->object, that.object);
        }
    };

    template <class Type, class Allocator>
        inline void swap (small_ptr <Type, Allocator> & p1,
            small_ptr <Type, Allocator> & p2)
    { p1.swap (p2); }

    /**
    Owner-based equality.
    If two small_ptr's own the same object in memory, they compare equal.
    If they are both empty, they compare equal.
    Otherwise, they compare unequal.
    */
    template <class Type, class Allocator>
        inline bool operator == (small_ptr <Type, Allocator> const & p1,
            small_ptr <Type, Allocator> const & p2)
    { return p1.get() == p2.get(); }

    /**
    Owner-based inequality.
    */
    template <class Type, class Allocator>
        inline bool operator != (small_ptr <Type, Allocator> const & p1,
            small_ptr <Type, Allocator> const & p2)
    { return p1.get() != p2.get(); }

    /**
    Owner-based ordering.
    */
    template <class Type, class Allocator>
        inline bool operator < (small_ptr <Type, Allocator> const & p1,
            small_ptr <Type, Allocator> const & p2)
    { return p1.get() < p2.get(); }

    template <class Type, class Allocator>
        inline bool operator > (small_ptr <Type, Allocator> const & p1,
            small_ptr <Type, Allocator> const & p2)
    { return p1.get() > p2.get(); }

    template <class Type, class Allocator>
        inline bool operator <= (small_ptr <Type, Allocator> const & p1,
            small_ptr <Type, Allocator> const & p2)
    { return p1.get() <= p2.get(); }

    template <class Type, class Allocator>
        inline bool operator >= (small_ptr <Type, Allocator> const & p1,
            small_ptr <Type, Allocator> const & p2)
    { return p1.get() >= p2.get(); }

}   // namespace utility

#endif // UTILITY_SMALL_PTR_HPP_INCLUDED
