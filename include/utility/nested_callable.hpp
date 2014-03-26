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
Find properties of nested function calls expressed as types.
*/

#ifndef UTILITY_NESTED_CALLABLE_HPP_INCLUDED
#define UTILITY_NESTED_CALLABLE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/apply.hpp>

#include "meta/vector.hpp"
#include "meta/all.hpp"
#include "meta/any.hpp"

namespace nested_callable {

    /**
    Find the result type of the nested function call.
    For example, if "plus" and "times" are callable types,
    result_of <plus (double, times (int, float))>::type
    may evaluate to "double".
    If the type is not a function expression, return the type itself.
    This uses std::result_of.
    */
    template <class Type> struct result_of { typedef Type type; };

    template <class Function, class ... Arguments>
        struct result_of <Function (Arguments ...)>
    : std::result_of <
        typename result_of <Function>::type (
            typename result_of <Arguments>::type ...)
    > {};

    template <class Function, class ... Arguments>
        struct result_of <Function (*) (Arguments ...)>
    : result_of <Function (Arguments ...)> {};

    namespace detail {

        /**
        Apply a predicate to a function expression after substituting any
        nested function expressions with their result types.
        */
        template <class Predicate, class Expression> struct shallow;

        template <class Predicate, class Function,  class ... Arguments>
            struct shallow <Predicate, Function (Arguments ...)>
        : boost::mpl::apply <Predicate,
            typename result_of <Function>::type (
                typename result_of <Arguments>::type ...)>::type {};

    } // namespace detail

    /**
    Compute whether a predicate is true for all function calls within a nested
    function call.
    For the higher levels, the function expressions are replaced with their
    result types.
    For example,
    all <plus (double, times (int, float))>
    will evaluate to mpl::true_ iff the predicate evaluates to true on
    times (int, float) and on plus (double, float).
    The computation is short-circuited; if the internal function expressions
    evaluate to false, false is returned immediately.
    In the example, if the predicate returned false on times (int, float), then
    it is not evaluated on plus (double, ...).
    */
    template <class Predicate, class Expression> struct all
    : boost::mpl::true_ {};

    template <class Predicate, class Function, class ... Arguments>
        struct all <Predicate, Function (Arguments ...)>
    : boost::mpl::eval_if <
        meta::all <meta::vector <
            all <Predicate, Function>,
            all <Predicate, Arguments> ...>>,
        detail::shallow <Predicate, Function (Arguments ...)>,
        boost::mpl::false_
    >::type {};

    template <class Predicate, class Function, class ... Arguments>
        struct all <Predicate, Function (*) (Arguments ...)>
    : all <Predicate, Function (Arguments ...)> {};

    /**
    Compute whether a predicate is false for any function call within a nested
    function call.
    This is the opposite of \c all.
    */
    template <class Predicate, class Expression> struct any
    : boost::mpl::false_ {};

    template <class Predicate, class Function, class ... Arguments>
        struct any <Predicate, Function (Arguments ...)>
    : boost::mpl::eval_if <
        meta::any <meta::vector <
            any <Predicate, Function>,
            any <Predicate, Arguments> ...>>,
        boost::mpl::true_,
        detail::shallow <Predicate, Function (Arguments ...)>
    >::type {};

    template <class Predicate, class Function, class ... Arguments>
        struct any <Predicate, Function (*) (Arguments ...)>
    : any <Predicate, Function (Arguments ...)> {};

} // namespace nested_callable

#endif // UTILITY_NESTED_CALLABLE_HPP_INCLUDED
