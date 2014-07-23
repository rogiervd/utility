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

#define BOOST_TEST_MODULE fail_config_no_cxx11_delegate_constructors
#include "utility/test/boost_unit_test.hpp"

#include "utility/config.hpp"

BOOST_AUTO_TEST_SUITE (fail_config_no_cxx11_delegate_constructors)

#ifndef UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
#error "Test disabled for this compiler - fail to compile on purpose."
#else
#include "trigger-config_no_cxx11_delegate_constructors.ipp"
#endif

BOOST_AUTO_TEST_SUITE_END()
