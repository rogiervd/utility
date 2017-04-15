/*
Copyright 2011, 2012, 2014, 2015, 2017 Rogier van Dalen.

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
Define types to store, pass, and return types in container classes.
For example, references are stored as std::reference_wrapper's, and passed as
themselves.

store, get, get_pointer, pass, and pass_rvalue can be used together.
*/

#ifndef UTILITY_STORAGE_HPP_INCLUDED
#define UTILITY_STORAGE_HPP_INCLUDED

#include <utility>
#include <memory>

#include <meta/count_c.hpp>

#include "config.hpp"

namespace utility { namespace storage {

    namespace detail {

        /**
        A type that can be used to represent void where a proper object is
        required.
        */
        class void_ {};

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
        template <class Type, std::size_t N> class array_wrapper;

        // Implementation type that is constructed with a list of indices.
        template <class Type, std::size_t N> struct array_wrapper_implementation
        {
            Type data_ [N];

            // This is the only way to initialise a std::initialiser_list.
            template <std::size_t ... Indices>
                array_wrapper_implementation (Type const (& data) [N],
                    meta::size_t_vector <Indices ...>)
            : data_ {data [Indices] ...} {}

            void assign (Type const (& data) [N], meta::size_t_vector <>) {}

            template <std::size_t FirstIndex, std::size_t ... Indices>
                void assign (Type const (& data) [N],
                    meta::size_t_vector <FirstIndex, Indices ...>)
            {
                data_ [FirstIndex] = data [FirstIndex];
                assign (data, meta::size_t_vector <Indices ...>());
            }
        };

        template <class Type, std::size_t N> class array_wrapper {
            typedef typename meta::count_c <N>::type indices_type;
        public:
            typedef Type value_type [N];

            typedef Type (& reference_type) [N];
            typedef Type const (& const_reference_type) [N];

            array_wrapper (Type const (& data) [N])
            : implementation (data, indices_type()) {}

            operator reference_type()
            { return implementation.data_; }

            operator const_reference_type() const
            { return implementation.data_; }

            array_wrapper & operator= (Type const (& data) [N]) {
                implementation.assign (data, indices_type());
                return *this;
            }

        private:
            array_wrapper_implementation <Type, N> implementation;
        };

        struct type_that_cannot_be_constructed_1 {
            type_that_cannot_be_constructed_1() = delete;
            type_that_cannot_be_constructed_1 (
                type_that_cannot_be_constructed_1 const &) = delete;
        };

        struct type_that_cannot_be_constructed_2 {
            type_that_cannot_be_constructed_2() = delete;
            type_that_cannot_be_constructed_2 (
                type_that_cannot_be_constructed_2 const &) = delete;
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

    // void.
    template <> struct store <void> {
        typedef detail::void_ type;
    };


    /**
    Suitably qualified version of Type that is contained in (qualified)
    Container.

    Also, this has an operator() which can take a lvalue reference to
    <c>store \<Type, Container &>::type</c> returns a value of type \c type.
    (The only difference between these two types is potentially their
    rvalue-ness, unless \a Type is void.)

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

        /// Return \c Type, unless it is a function type, in which case it is
        /// converted into a function reference.
        template <class Type> struct value { typedef Type type; };

        template <class Result, class ... Arguments>
            struct value <Result (Arguments ...)>
        { typedef Result (& type) (Arguments ...); };

    } // namespace get_detail

    template <class Type, class Container> struct get
    {
        typedef typename get_detail::deal_with_reference <Type, Container>::type
            type;
        type operator() (typename get <Type, Container &>::type object) const
        { return static_cast <type> (object); }
    };

    template <class Container> struct get <void, Container> {
        typedef void type;
        type operator() (detail::void_) {}
    };

    /** \brief
    Contain a suitably qualified version of Type that can be returned as a
    value.

    The value will remain valid after the container is destructed (unlike, in
    general, <c>get \<...>::type</c>).

    Normally, \a Type itself is returned.
    However, if \a Type is a function type, a function reference is returned.

    If Type is an array type, which cannot be returned, this does not contain
    \c type at all.
    To allow a member function to disappear in that case, the following idiom
    can be used:
    \code
        template <class Result = typename storage::get_value <Type>>
            typename Result::type value() const
    \endcode

    Unlike \c get, this class does not provide any runtime behaviour.
    This is intentional.
    A function or member function returning <c>get_value \<...>::type</c> can
    use <c>get \<Type, Container></c> internally.
    Here, \c Container must be qualified, and how Container is qualified makes a
    difference.
    This means that a move or a copy may end up being used, which should be the
    correct behaviour.

    \tparam Type
        The type to be qualified appropriately.
    */
    template <class Type> struct get_value
    : get_detail::value <Type> {};

    // Specialisation for arrays: do not contain "type".
    template <class Type, std::size_t Number> struct get_value <Type[Number]>
    {};

    /**
    Return a type that can serve as a pointer to a contained type.
    This gives the address of what get() returns with an lvalue Container type.

    Also, this has an operator() which can take a lvalue reference to
    <c>store \<Type, Container &>::type</c> returns a pointer to it of type
    \c type.

    If \a Type is \c void, the pointer that is returned is to a const-qualified
    void and points to the actual object saved.

    If \a Type is a reference type, a pointer is returned to the referenced
    object.
    This means that it works well as a return type for \c operator->.
    */
    template <class Type, class Container> class get_pointer {
        struct unusable { typedef unusable type; };
    public:
        typedef typename std::add_pointer <typename
            get_detail::deal_with_const <Type, typename
                std::remove_reference <Container>::type>::type>::type type;

        // This overload is used iff Type is not void.
        inline type operator() (typename std::conditional <
            !std::is_same <Type, void>::value,
            get <Type, Container &>, unusable>::type::type object)
            const
        { return std::addressof (object); }

        // These overloads will only be chosen if Type is void.
        inline type operator() (detail::void_ & object) const
        { return &object; }

        inline type operator() (detail::void_ const & object) const
        { return &object; }
    };

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

    If \a Type is void, then a type is returned that can never be constructed
    and therefore never passed in.
    <c>pass_rvalue \<Type>::type</c> will behave the same, but return a
    different type, so that two overloads can be declared and neither of them
    be instantiated.

    \sa pass_rvalue
    */
    template <class Type> struct pass
    { typedef Type const & type; };

    template <class Type> struct pass <Type &&>
    { typedef Type && type; };

    template<> struct pass <void>
    { typedef detail::type_that_cannot_be_constructed_1 type; };

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
    { typedef detail::type_that_cannot_be_constructed_1 type; };

    template <class Type> struct pass_rvalue <Type &&>
    { typedef detail::type_that_cannot_be_constructed_1 type; };

    template<> struct pass_rvalue <void>
    { typedef detail::type_that_cannot_be_constructed_2 type; };

}} // namespace utility::storage

#endif // UTILITY_STORAGE_HPP_INCLUDED
