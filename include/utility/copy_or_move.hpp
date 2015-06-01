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

} // namespace utility

#endif // UTILITY_COPY_OR_MOVE_HPP_INCLUDED
