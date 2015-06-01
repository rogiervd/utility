/*
Copyright 2009, 2014 Rogier van Dalen.

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
