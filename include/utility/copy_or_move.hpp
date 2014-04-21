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

/** \file
Define simple utilities dealing with arguments that are either const-references
or rvalue references of a specific type.
This can be for forwarding a specific type (e.g. in a container) or for copy
and move constructors.
*/

#ifndef UTILITY_COPY_OR_MOVE_HPP_INCLUDED
#define UTILITY_COPY_OR_MOVE_HPP_INCLUDED

#include <type_traits>

namespace utility {

/**
Take an argument either as a Target const & or as a Target &&, and return it
exactly.
This is useful to overload a function with Type && that is really supposed to
receive only a const-reference or an unqualifed rvalue reference to a specific
type.
*/
template <class Target> inline
    typename std::decay <Target>::type const &
    copy_or_move (Target const & argument)
{ return argument; }

template <class Target> inline
    typename std::decay <Target>::type &&
    copy_or_move (Target && argument)
{ return static_cast <Target &&> (argument); }

/**
Forward an argument of type Source (qualified, like one would for std::forward)
as either a const-reference or an unqualified rvalue reference.
*/
template <class Target, class Source> inline
    auto forward_copy_or_move (
        typename std::remove_reference <Source>::type & argument)
-> decltype (copy_or_move <Target> (std::declval <Source &&>()))
{ return copy_or_move <Target> (static_cast <Source &&> (argument)); }

namespace detail {
    template <class Container, class Member> struct forward_member_result
    { typedef typename std::remove_reference <Member>::type && type; };

    template <class Container, class Member>
        struct forward_member_result <Container &, Member>
    { typedef typename std::remove_reference <Member>::type & type; };

} // namespace detail

/**
Forward a member with the reference qualification of its container.
That is, if the container is not an lvalue reference, return an rvalue reference
to the member.
This can be used to forward a member of a struct or an element of a container.
This is not as safe to use as std::forward.
*/
template <class Container, class Member>
    typename detail::forward_member_result <Container, Member>::type
    forward_member (Member & member)
{
    return static_cast <typename
        detail::forward_member_result <Container, Member>::type> (member);
}

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

#endif // UTILITY_COPY_OR_MOVE_HPP_INCLUDED
