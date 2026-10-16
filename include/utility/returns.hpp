// SPDX-FileCopyrightText: Copyright 2011, 2012, 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define macro to make it easier to declare the return type of a simple function.
*/

#ifndef UTILITY_RETURNS_HPP_INCLUDED
#define UTILITY_RETURNS_HPP_INCLUDED

#include <type_traits>

#include <boost/preprocessor/cat.hpp>

/** \brief
Make a type declaration and implementation for a function at once.

For example, to create a function that returns \c x:
\code
auto f (...) RETURNS (x);
\endcode

The type that is produced is the type of the expression given (as opposed to the
type of the variable, for exampe).
In the example, if \c x has type \c int, the return type may be <c>int &</c>.
If the decayed type is required, \ref RETURNS_DECAYED can be used.

In C++11, an "auto" return type needs an explicit type specification after the
traditional function signature.
This looks like
\code
auto f (...) -> decltype ((x)) { return x; }
\endcode
This macro simplifies those expressions to the above
This requires a semicolon after the macro invocation, so that simple parsers
(for example, for inline documentation) do not get confused.
*/
#define RETURNS(...)                                  \
    ->decltype((__VA_ARGS__)) { return __VA_ARGS__; } \
    struct BOOST_PP_CAT(                              \
        utility_returns_should_be_followed_by_a_semicolon, __LINE__)

/** \brief
Make a type declaration (returning a decayed type) and an implementation for a
function at once.

For example, to create a function that returns \c x:
\code
auto f (...) RETURNS (x);
\endcode

This is like \ref RETURNS but returns the decayed type.
*/
#define RETURNS_DECAYED(...)                             \
    ->typename std::decay<decltype((__VA_ARGS__))>::type \
    {                                                    \
        return __VA_ARGS__;                              \
    }                                                    \
    struct BOOST_PP_CAT(                                 \
        utility_returns_should_be_followed_by_a_semicolon, __LINE__)

#endif  // UTILITY_RETURNS_HPP_INCLUDED
