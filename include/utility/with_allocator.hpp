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

#ifndef UTILITY_WITH_ALLOCATOR_HPP_INCLUDED
#define UTILITY_WITH_ALLOCATOR_HPP_INCLUDED

namespace utility {

    /**
    Hold an allocator, but take up no space if it is empty.
    \todo swap could be optimised using allocator_traits.
    */
    template <class Allocator> class with_allocator : private Allocator {
    protected:
        typedef Allocator allocator_type;

        template <class Allocator2> friend class with_allocator;

    public:
        with_allocator (with_allocator const & other)
        noexcept (noexcept (
            allocator_type (std::declval <allocator_type const &>())))
        : allocator_type (other) {}

        with_allocator (with_allocator && other)
        noexcept (noexcept (
            allocator_type (std::declval <allocator_type &&>())))
        : allocator_type (std::move (other)) {}

        with_allocator (allocator_type const & allocator)
        noexcept (noexcept (
            allocator_type (std::declval <allocator_type const &>())))
        : allocator_type (allocator) {}

        with_allocator (allocator_type && allocator)
        noexcept (noexcept (
            allocator_type (std::declval <allocator_type &&>())))
        : allocator_type (std::move (allocator)) {}

    protected:
        allocator_type & allocator()
        { return *static_cast <allocator_type *> (this); }

        allocator_type const & allocator() const
        { return *static_cast <allocator_type const *> (this); }

        void swap (with_allocator & other) {
            using std::swap;
            swap (allocator(), other.allocator());
        }
    };

} // namespace utility

#endif // UTILITY_WITH_ALLOCATOR_HPP_INCLUDED
