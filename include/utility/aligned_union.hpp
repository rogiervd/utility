/*
Copyright 2011, 2012 Rogier van Dalen.

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
