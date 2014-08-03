/*
Copyright 2011, 2012, 2014 Rogier van Dalen.

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
Define types to store, pass, and return types in container classes.
For example, references are stored as std::reference_wrapper's, and passed as
themselves.

store, get, get_pointer, pass, and pass_rvalue can be used together.
*/

#ifndef UTILITY_STORAGE_HPP_INCLUDED
#define UTILITY_STORAGE_HPP_INCLUDED

#include <utility>

#include <meta/count.hpp>

#include "config.hpp"

namespace utility { namespace storage {

    namespace detail {

        /**
        Wrap an rvalue reference.
        This works similarly to \c std::reference_wrapper.
        This is assignable, but convertible to an rvalue reference.
        */
        template <class Type> class rvalue_reference_wrapper {
            Type * location;
        public:
            rvalue_reference_wrapper (Type && object) : location (&object) {}

            operator Type && () const
            { return static_cast <Type &&> (*location); }
        };

        /**
        Wrap a function (not a function reference or a function pointer).
        Functions are not really values, so they are saved as a pointer type.
        */
        template <class FunctionType> class function_wrapper {
        public:
            typedef FunctionType * pointer_type;
            typedef FunctionType & reference_type;
            typedef FunctionType function_type;

            function_wrapper (pointer_type pointer)
            : pointer_ (pointer) {}

            operator pointer_type() const { return pointer_; }
            operator reference_type() const { return *pointer_; }

        private:
            pointer_type pointer_;
        };

        /**
        Wrap an array.
        This class can be constructed from an array reference, and it is
        convertible to an array reference.

        The implementation of this class requires delegate constructors, so it
        does not work for compilers that do not support those.
        */
        template <class Type, std::size_t N> class array_wrapper {
#ifdef UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
            template <class Type2> struct always_false
            { static bool const value = false; };

            static_assert (always_false <Type>::value,
                "utility::storage::store is not implemented for arrays on this "
                "compiler. "
                "The implementation requires delegate constructors. Sorry.");
#else
        public:
            typedef Type value_type [N];

            typedef Type (& reference_type) [N];
            typedef Type const (& const_reference_type) [N];

            array_wrapper (Type const (& data) [N])
            : array_wrapper (data, typename meta::count <N>::type()) {}

            operator reference_type() { return data_; }
            operator const_reference_type() const { return data_; }

        private:
            Type data_ [N];

            // This is the only way to initialise a std::initialiser_list.
            template <class ... Indices>
            array_wrapper (
                Type const (& data) [N], meta::vector <Indices ...> const &)
            : data_ {data [Indices::value] ...} {}
#endif
        };

        struct incomplete_type;

        struct type_that_cannot_be_constructed {
            type_that_cannot_be_constructed() = delete;
            type_that_cannot_be_constructed (
                type_that_cannot_be_constructed const &) = delete;
        };

    } // namespace detail

    /**
    Returns a type that can be used to store a user-specified type.
    This type is assignable from Type, for common types, like references,
    functions, and arrays.
    (However, classes that are not assignable will remain not assignable.
    But that might be expected by the user anyway.)
    \c sizeof and \c alignof are available.
    The type is implicitly convertible to <c>get \<Type>::type</c>.
    A pointer can be taken: store<>::type * is valid.
    */
    template <class Type> struct store;

    template <class Type> struct store
    { typedef Type type; };

    // References: store as reference_wrapper's.
    template <class Type> struct store <Type &>
    { typedef std::reference_wrapper <Type> type; };

    template <class Type> struct store <Type &&>
    { typedef detail::rvalue_reference_wrapper <Type> type; };

    // Const: store non-const.
    template <class Type> struct store <Type const>
    { typedef Type type; };

    // Function: store as a function pointer.
    // (Function pointers and function reference need no special treatment.)
    template <class Result, typename ... Arguments>
        struct store <Result (Arguments ...)>
    { typedef detail::function_wrapper <Result (Arguments ...)> type; };

    // Arrays.
    template <class Type, std::size_t N>
        struct store <Type [N]>
    { typedef detail::array_wrapper <Type, N> type; };
    // Disambiguate const arrays.
    template <class Type, std::size_t N>
        struct store <Type const [N]>
    { typedef detail::array_wrapper <Type, N> type; };

    // Don't try to store a void.
    template <> struct store <void> {};


    /**
    Suitably qualified version of Type that is contained in (qualified)
    Container.

    Also, this has an operator() which takes a value of
    <c>get \<Type, Container &>::type</c> (which a value of
    <c>store \<Type>::type</c> is implicitly convertible to) and returns a value
    of type type.
    (The only difference between these two types is potentially their
    rvalue-ness.)

    \tparam Type
        The type to be qualified appropriately.
        If \a Type is a reference (lvalue or rvalue), then it will be returned
        unchanged.
        If \a Type is void, it will also be returned unchanged.
    \tparam Container
        (optional)
        The object that is taken to contain the value.
        This must be a reference, either an lvalue reference (&) or an rvalue
        reference (&&).
        If is not given, it is assumed to be an lvalue reference type.
        For an lvalue reference Container, an lvalue reference is normally
        returned.
        An rvalue reference Container is taken to be a temporary that can be
        emptied, so the resulting type will be Type &&, an rvalue reference.
        Constness and referenceness of the container also propagate.
    */
    template <class Type, class Container = int &> struct get;

    namespace get_detail {

        template <class Type, class Container> struct deal_with_reference;
        template <class Type, class Container> struct deal_with_const;

        template <class Type, class Container> struct deal_with_const
        { typedef Type type; };
        template <class Type, class Container>
            struct deal_with_const <Type, Container const>
        { typedef Type const type; };

        // Rvalue container -> rvalue type.
        template <class Type, class Container>
            struct deal_with_reference <Type, Container &&>
        { typedef typename deal_with_const <Type, Container>::type && type; };
        template <class Type, class Container>
            struct deal_with_reference <Type &, Container &&>
        { typedef typename deal_with_const <Type &, Container>::type type; };
        template <class Type, class Container>
            struct deal_with_reference <Type &&, Container &&>
        { typedef typename deal_with_const <Type &&, Container>::type type; };

        // Lvalue container -> lvalue type.
        template <class Type, class Container>
            struct deal_with_reference <Type, Container &>
        { typedef typename deal_with_const <Type, Container>::type & type; };
        // Except if the type is an rvalue.
        template <class Type, class Container>
            struct deal_with_reference <Type &, Container &>
        { typedef typename deal_with_const <Type &, Container>::type type; };
        template <class Type, class Container>
            struct deal_with_reference <Type &&, Container &>
        { typedef typename deal_with_const <Type &&, Container>::type type; };

    } // namespace get_detail

    template <class Type, class Container> struct get
    : get_detail::deal_with_reference <Type, Container>
    {
        typedef typename get_detail::deal_with_reference <Type, Container>::type
            type;
        type inline operator() (typename get <Type, Container &>::type object)
            const
        { return static_cast <type> (object); }
    };

    template <class Container> struct get <void, Container>
    { typedef void type; };

    /**
    Return a type that can serve as a pointer to a contained type.
    For address of whatever's been returned by get() with an lvalue Container
    type.

    There are a number of types that it is not possible to meaningfully get a
    pointer type to.
    These are void, lvalue and rvalue references, and function types and
    function reference.

    For these types, the returned type is an incomplete type.
    This way, a method that returns this type can be declared, but will trigger
    a compile error if it gets instantiated.
    */
    template <class Type, class Container> struct get_pointer {
        typedef typename get_detail::deal_with_const <Type,
                typename std::remove_reference <Container>::type>::type * type;
    };

    template <class Container>
        struct get_pointer <void, Container>
    { typedef detail::incomplete_type type; };

    template <class Type, class Container>
        struct get_pointer <Type &, Container>
    { typedef detail::incomplete_type type; };
    template <class Type, class Container>
        struct get_pointer <Type &&, Container>
    { typedef detail::incomplete_type type; };

    template <class ReturnType, class ... Arguments, class Container>
        struct get_pointer <ReturnType (Arguments ...), Container>
    { typedef detail::incomplete_type type; };

    /**
    Return a type that is an argument type for copy-constructing an object of
    type \a Type (or <c>store \<Type>::type</c>).
    This is normally <c>Type const &</c>.
    The exception is if \a Type is an rvalue reference, in which case \a Type is
    returned.

    It is possible to provide an additional overload for a
    function/method/constructor that takes a \a Type by rvalue reference where
    this is possible and different from <c>pass \<Type>::type</c>.
    This type can be computed with <c>pass_rvalue \<Type>::type</c>.

    \sa pass_rvalue
    */
    template <class Type> struct pass
    { typedef Type const & type; };
    template <class Type> struct pass <Type &&>
    { typedef Type && type; };

    /**
    Return a type that is an argument type for move-constructing an object
    of type Type (or store <Type>::type).
    This only returns a type if it is possible to produce a different type from
    <c>pass \<Type>::type</c>.
    If there is no such other type, then return a type that can never be
    constructed, nor, therefore, passed in.

    This way, a function/method/constructor can be overloaded with
    <c>pass \<Type>::type</c> and <c>pass_rvalue \<Type>::type</c>, and both
    declarations will compile; the former may never be instantiated, depending
    on \a Type.

    The type is always an rvalue reference, so it should usually be passed on
    with \c std::move.
    */
    template <class Type, class Dummy = void> struct pass_rvalue
    { typedef typename std::remove_const <Type>::type && type; };

    template <class Type> struct pass_rvalue <Type &>
    { typedef detail::type_that_cannot_be_constructed type; };

    template <class Type> struct pass_rvalue <Type &&>
    { typedef detail::type_that_cannot_be_constructed type; };

}} // namespace utility::storage

#endif // UTILITY_STORAGE_HPP_INCLUDED
