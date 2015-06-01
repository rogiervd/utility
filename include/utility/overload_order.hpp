/*
Copyright 2014 Rogier van Dalen.

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
