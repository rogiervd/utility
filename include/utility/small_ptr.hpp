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
