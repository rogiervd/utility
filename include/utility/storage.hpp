/*
Copyright 2011, 2012 Rogier van Dalen.

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

/**
\file
Define which types container classes use to store types specified by the user.
For example, reference types are stored as std::reference_wrapper's.
*/

#ifndef UTILITY_STORAGE_HPP_INCLUDED
#define UTILITY_STORAGE_HPP_INCLUDED

#include <utility>

namespace utility { namespace storage {

    /**
    Returns a type that can be used to store a user-specified type.

    If a Container type is passed in, the type stored will be returned in the
    context of the const-qualification of the container.
    Container is allowed to be a reference type; this is ignored.
    */
    template <typename Type, typename Container = void> struct store_as;

    template <typename Type> struct store_as_impl
    { typedef Type type; };
    template <typename Type> struct store_as_impl <Type &>
    { typedef std::reference_wrapper <Type> type; };
    template <typename Type> struct store_as_impl <Type &&>
    { typedef std::reference_wrapper <Type> type; };

    template <typename Type, typename Container> struct store_as
    : store_as_impl <Type> {};
    template <typename Type, typename Container>
        struct store_as <Type, Container const>
    { typedef typename store_as_impl <Type>::type const type; };
    template <typename Type, typename Container>
        struct store_as <Type, Container &>
    : store_as <Type, Container> {};
    // Don't try to store a void.
    template <typename Container> struct store_as <void, Container &> {};

    /**
    Return a type that can be used to return a contained type.

    The type of the container is considered.
    A non-reference container type is taken to be a rvalue reference (temporary)
    that can be emptied, so the resulting type is Type &&.
    Constness and referenceness of the container also propagate.
    If type is void, however, it will always be returned unchanged.
    */
    template <typename Type, typename Container> struct get_as;

    template <typename Type, typename Container> struct get_as_impl
    { typedef Type && type; };

    template <typename Type, typename Container>
        struct get_as_impl <Type, Container const>
    { typedef Type const && type; };

    template <typename Type, typename Container>
        struct get_as_impl <Type, Container &>
    { typedef Type & type; };

    template <typename Type, typename Container>
        struct get_as_impl <Type, Container const &>
    { typedef Type const & type; };

    template <typename Type, typename Container> struct get_as
    : get_as_impl <Type, Container> {};
    template <typename Container> struct get_as <void, Container>
    { typedef void type; };

    /**
    Return a type that can serve as a pointer to a contained type.
    */
    template <typename Type, typename Container> struct get_pointer
    { typedef Type * type; };
    template <typename Type, typename Container>
        struct get_pointer <Type, Container const>
    { typedef Type const * type; };

    // Getting a pointer to a reference type does not make sense so disable it.
    template <typename Type, typename Container>
        struct get_pointer <Type &, Container> {};
    template <typename Type, typename Container>
        struct get_pointer <Type &, Container const> {};

}} // namespace utility::storage

#endif // UTILITY_STORAGE_HPP_INCLUDED
