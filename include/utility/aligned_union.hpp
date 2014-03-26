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
Define an aligned_union type that takes a compile-time type list.
*/

#ifndef UTILITY_ALIGNED_UNION_HPP_INCLUDED
#define UTILITY_ALIGNED_UNION_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/placeholders.hpp>

#include "meta/transform.hpp"
#include "meta/max_element.hpp"

namespace utility {

    /**
    Compute a POD type that can be used as aligned storage for each of the
    types in type container Types.
    */
    template <typename Types> struct aligned_union {
    private:
        typedef boost::mpl::_ _;
        typedef meta::transform <std::alignment_of <_>, Types> alignments;
        static const size_t alignment
            = meta::max_element <alignments>::type::value;

        typedef meta::transform <boost::mpl::sizeof_ <_>, Types> sizeofs;
        static const size_t storage_size
            = meta::max_element <sizeofs>::type::value;

    public:
        typedef typename std::aligned_storage <storage_size, alignment>::type
            type;
    };

} // namespace utility

#endif // UTILITY_ALIGNED_UNION_HPP_INCLUDED
