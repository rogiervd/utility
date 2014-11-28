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

#ifndef UTILITY_TEST_BOOST_UNIT_TEST_COMMAND_LINE_HPP_INCLUDED
#define UTILITY_TEST_BOOST_UNIT_TEST_COMMAND_LINE_HPP_INCLUDED

#include <string>

#include "boost_unit_test.hpp"

bool command_line_contains (std::string const & argument) {
    int argc = boost::unit_test::framework::master_test_suite().argc;
    char ** argv = boost::unit_test::framework::master_test_suite().argv;
    for (int argument_index = 1; argument_index < argc; ++ argument_index) {
        if (argv [argument_index] == argument)
            return true;
    }
    return false;
}

#endif // UTILITY_TEST_BOOST_UNIT_TEST_COMMAND_LINE_HPP_INCLUDED
