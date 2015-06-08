/*
Copyright 2012, 2013, 2015 Rogier van Dalen.

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

/**
\file
Simple utilities for std::unique_ptr.
*/

#ifndef UTILITY_UNIQUE_PTR_HPP_INCLUDED
#define UTILITY_UNIQUE_PTR_HPP_INCLUDED

#include <type_traits>
#include <memory>

namespace utility {

    namespace detail {

        template <class Type> struct is_unique_ptr : std::false_type {};

        template <class Type, class Deleter>
            struct is_unique_ptr <std::unique_ptr <Type, Deleter>>
        : std::true_type {};

    } // namespace detail

    /** \brief
    Metafunction that detects whether a type is a unique_ptr.

    The unique_ptr can be qualified.
    */
    template <class Type> struct is_unique_ptr
    : detail::is_unique_ptr <typename std::decay <Type>::type> {};

    /** \brief
    Produce a new object of type Type with "new" and return a std::unique_ptr
    to it.

    \note This does not work for array types, unlike the C++14 version.
    */
    template <typename Type, typename ... Args>
        std::unique_ptr <Type> make_unique (Args && ... args)
    {
        return std::unique_ptr <Type> (
            new Type (std::forward <Args> (args) ...));
    }

    /** \brief
    Turn a std::unique_ptr into a std::shared_ptr.

    This is particularly useful to wrap functions that return a std::unique_ptr.
    This is a conservative and explicit approach to handling management of
    resources.
    However, the type in the unique_ptr may be long to type.
    This function therefore can make life easier.

    The unique_ptr is unshared, so it is safe to construct a new shared_ptr.
    */
    template <class Type, class Deleter>
        inline std::shared_ptr <Type> shared_from_unique (
            std::unique_ptr <Type, Deleter> && unique)
    { return std::shared_ptr <Type> (std::move (unique)); }

} // namespace utility

#endif // UTILITY_UNIQUE_PTR_HPP_INCLUDED
