/*
Copyright 2014 Rogier van Dalen.

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

#ifndef UTILITY_DISABLE_IF_SAME_INCLUDED
#define UTILITY_DISABLE_IF_SAME_INCLUDED

#include <type_traits>

namespace utility {

/**
Contain a type only if Source is not a qualified version of Target, and Target
is not a base of Source.
The idea for this is from
http://ericniebler.com/2013/08/07/universal-references-and-the-copy-constructo/

This is useful if a class has a copy and/or a move constructor, and a templated
constructor that is supposed to be used for any other types, e.g. to be
forwarded to a contained or derived type.
This class can be used to disable the templated overload if the copy and/or
move constructor should be used.
*/
template <class Target, class Source, class Type = void>
    struct disable_if_same_or_derived
: std::enable_if <
    !std::is_same <Target, typename std::decay <Source>::type>::value &&
    !std::is_base_of <Target,
        typename std::remove_reference <Source>::type>::value> {};

/**
Contain a type (which is always \c void) only if \a Sources is not one type
which is a qualified version of Target, or a derived class.
If Sources is a type list of zero or more than 1 types, always contain \c void.

This differs from disable_if_same_or_derived in that this allows a variadic
Sources argument, but no explicit type to contain.
*/
template <class Target, class ... Sources>
    struct disable_if_variadic_same_or_derived
{ typedef void type; };

template <class Target, class Source>
    struct disable_if_variadic_same_or_derived <Target, Source>
: disable_if_same_or_derived <Target, Source> {};

} // namespace utility

#endif // UTILITY_DISABLE_IF_SAME_INCLUDED
