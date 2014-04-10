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

#include "pointer_policy.hpp"

namespace utility {

    template <class Type, class Allocator = std::allocator <Type>>
        class small_ptr;

    namespace detail {

        template <class Type, class Allocator> struct small_ptr_policies {
            typedef pointer_policy::strict_weak_ordered <
                pointer_policy::pointer_access <
                pointer_policy::reference_count_shared <
                pointer_policy::use_allocator <
                    Type, Allocator>>>> type;
        };

    } // namespace detail

    /**
    Smart pointer to an object of known type.
    Since it uses an allocator, it has a fixed type.
    It is meant as an efficient type to be used inside containers.
    */
    template <class Type, class Allocator> class small_ptr
    : public pointer_policy::pointer <
        typename detail::small_ptr_policies <Type, Allocator>::type,
        small_ptr <Type, Allocator>>
    {
        typedef typename detail::small_ptr_policies <Type, Allocator>::type
            policies_type;
        typedef pointer_policy::pointer <policies_type, small_ptr> base_type;
    public:
        template <class ... Arguments>
            explicit small_ptr (Arguments && ... arguments)
        : base_type (std::forward <Arguments> (arguments) ...) {}

        small_ptr (small_ptr const &) = default;
        small_ptr (small_ptr &&) = default;

        small_ptr & operator = (small_ptr const &) = default;
        small_ptr & operator = (small_ptr &&) = default;
    };

}   // namespace utility

#endif // UTILITY_SMALL_PTR_HPP_INCLUDED
