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

#define BOOST_TEST_MODULE test_utility_order_overload
#include "utility/test/boost_unit_test.hpp"

#include "utility/overload_order.hpp"

#include <limits>
#include <string>

#include <boost/utility/enable_if.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_utility_order_overload)

using utility::pick_overload;

template <class Type>
    typename boost::enable_if_c <
    std::numeric_limits <Type>::is_signed
    && std::numeric_limits <Type>::is_integer,
    std::string>::type get_type (Type, utility::overload_order <1> *)
{ return "signed integer"; }

template <class Type>
    typename boost::enable_if_c <std::numeric_limits <Type>::is_integer,
    std::string>::type get_type (Type, utility::overload_order <2> *)
{ return "integer"; }

template <class Type>
    typename boost::enable_if_c <std::numeric_limits <Type>::is_specialized,
    std::string>::type get_type (Type, utility::overload_order <3> *)
{ return "number type"; }

template <class Type> std::string get_type (Type, utility::overload_order <4> *)
{ return "no number type"; }

BOOST_AUTO_TEST_CASE (test_utility_order_overload) {
    BOOST_CHECK_EQUAL (get_type (4, pick_overload()), "signed integer");
    BOOST_CHECK_EQUAL (get_type (4u, pick_overload()), "integer");
    BOOST_CHECK_EQUAL (get_type (5., pick_overload()), "number type");
    BOOST_CHECK_EQUAL (get_type ("4", pick_overload()), "no number type");
}

BOOST_AUTO_TEST_SUITE_END()
