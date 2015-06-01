/*
Copyright 2012, 2013 Rogier van Dalen.

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
