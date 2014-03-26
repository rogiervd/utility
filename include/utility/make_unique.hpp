/*
Copyright 2012, 2013 Rogier van Dalen.

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
Define make_unique, which is not in the C++11 standard.
The C++14 standard adds it.
*/

#ifndef UTILITY_MAKE_UNIQUE_HPP_INCLUDED
#define UTILITY_MAKE_UNIQUE_HPP_INCLUDED

#include <memory>

namespace utility {

    /**
    Produce a new object of type Type with "new" and return a std::unique_ptr
    to it.
    \note This does not work for array types, unlike the C++14 version.
    */
    template <typename Type, typename ... Args>
        std::unique_ptr <Type> make_unique (Args && ... args)
    {
        return std::unique_ptr <Type> (
            new Type (std::forward <Args> (args) ...));
    }

} // namespace utility

#endif // UTILITY_MAKE_UNIQUE_HPP_INCLUDED
