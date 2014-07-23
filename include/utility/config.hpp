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

#ifndef UTILITY_CONFIG_HPP_INCLUDED
#define UTILITY_CONFIG_HPP_INCLUDED

#if defined __clang__

#   if !__has_feature (cxx_delegating_constructors)
#       define UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
#   endif

#elif defined __GNUC__
#   if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7) \
            || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#       define UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
#   endif
#endif

#endif // UTILITY_CONFIG_HPP_INCLUDED
