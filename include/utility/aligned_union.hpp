// SPDX-FileCopyrightText: Copyright 2011, 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/**
\file
Define an aligned_union type that takes a compile-time type list.
*/

#ifndef UTILITY_ALIGNED_UNION_HPP_INCLUDED
#define UTILITY_ALIGNED_UNION_HPP_INCLUDED

#include <type_traits>

#include "boost/mpl/placeholders.hpp"
#include "boost/mpl/sizeof.hpp"

#include "meta/max_element.hpp"
#include "meta/transform.hpp"

namespace utility {

/**
Compute a POD type that can be used as aligned storage for each of the
types in type container Types.
*/
template <typename Types> struct aligned_union
{
private:
    typedef boost::mpl::_ _;
    typedef meta::transform<std::alignment_of<_>, Types> alignments;
    static size_t const alignment = meta::max_element<alignments>::type::value;

    typedef meta::transform<boost::mpl::sizeof_<_>, Types> sizeofs;
    static size_t const storage_size = meta::max_element<sizeofs>::type::value;

public:
    typedef typename std::aligned_storage<storage_size, alignment>::type type;
};

}  // namespace utility

#endif  // UTILITY_ALIGNED_UNION_HPP_INCLUDED
