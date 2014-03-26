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
