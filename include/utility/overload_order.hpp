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

#ifndef UTILITY_ORDER_OVERLOAD_HPP_INCLUDED
#define UTILITY_ORDER_OVERLOAD_HPP_INCLUDED

namespace utility {

    /**
    Helper class to disambiguate function overloads.
    Use a pointer to this class as an argument type, with different values for
    "priority" for each overloaded function.
    Then call the class with "pick_overload()" for that argument.
    */
    template <int priority> struct overload_order
    : overload_order <priority + 1> {};

    template <> struct overload_order <32> {};

    inline overload_order <0> * pick_overload() { return nullptr; }

} // namespace utility

#endif // UTILITY_ORDER_OVERLOAD_HPP_INCLUDED
