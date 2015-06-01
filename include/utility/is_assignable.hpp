/*
Copyright 2013 Rogier van Dalen.

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
std::is_assignable is not available on GCC 4.6.
This provides an equivalent class.
*/

#ifndef UTILITY_IS_ASSIGNABLE_HPP_INCLUDED
#define UTILITY_IS_ASSIGNABLE_HPP_INCLUDED

#include <type_traits>
#include <utility>

namespace utility {

    namespace detail {

        /**
        is_assignable, but does not derive from std::integral_constant.
        */
        template <class Destination, class Source> struct is_assignable {
        private:
            struct no {};
            struct yes {};
            template <class Type> struct make_yes { typedef yes type; };

            // Use SFINAE: if the argument to make_yes can be instantiated,
            // assignment is possible.
            template <class Destination2, class Source2>
                static typename make_yes <decltype (
                    std::declval <Destination2>() = std::declval <Source2>())
                >::type
            test (int);

            template <class, class> static no test (...);
        public:
            static const bool value = std::is_same <
                decltype (test <Destination, Source> (1)), yes>::value;
        };

        /**
        Assuming is_assignable is true, is this noexcept?
        */
        template <class Destination, class Source> struct is_nothrow_assignable
        : std::integral_constant <bool,
            noexcept (std::declval <Destination>() = std::declval <Source>())>
        {};

    } // namespace detail

    /**
    Implementation of std::is_assignable in case the standard library does not
    provide it.
    */
    template <class Destination, class Source> struct is_assignable
    : std::integral_constant <bool,
        detail::is_assignable <Destination, Source>::value> {};

    /**
    Implementation of std::is_nothrow_assignable in case the standard library
    does not provide it.
    */
    template <class Destination, class Source> struct is_nothrow_assignable
    : std::conditional <is_assignable <Destination, Source>::value,
        detail::is_nothrow_assignable <Destination, Source>,
        std::false_type>::type {};

} // namespace utility

#endif // UTILITY_IS_ASSIGNABLE_HPP_INCLUDED
