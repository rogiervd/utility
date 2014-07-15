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

/** \file
Define macro to make it easier to declare the return type of a simple function.
*/

#ifndef UTILITY_RETURNS_HPP_INCLUDED
#define UTILITY_RETURNS_HPP_INCLUDED

#include <boost/preprocessor/cat.hpp>

/**
In C++11, an "auto" return type needs an explicit type specification after the
traditional function signature.
This looks like
    auto f (...) -> decltype (x) { return x; }
This macro simplifies those expressions into
    auto f (...) RETURNS (x);
This requires a semicolon after the macro invocation, so that simple parsers
(for example, for inline documentation) do not get confused.
*/
#define RETURNS(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; } \
    struct BOOST_PP_CAT (utility_returns_should_be_followed_by_a_semicolon, \
        __LINE__)

#endif // UTILITY_RETURNS_HPP_INCLUDED
