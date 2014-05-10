/*
Copyright 2014 Rogier van Dalen.

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

} // namespace utility

#endif // UTILITY_DISABLE_IF_SAME_INCLUDED
