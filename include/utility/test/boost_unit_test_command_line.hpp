// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UTILITY_TEST_BOOST_UNIT_TEST_COMMAND_LINE_HPP_INCLUDED
#define UTILITY_TEST_BOOST_UNIT_TEST_COMMAND_LINE_HPP_INCLUDED

#include <string>

#include "boost_unit_test.hpp"

bool command_line_contains(std::string const & argument)
{
    int argc = boost::unit_test::framework::master_test_suite().argc;
    char ** argv = boost::unit_test::framework::master_test_suite().argv;
    for (int argument_index = 1; argument_index < argc; ++argument_index) {
        if (argv[argument_index] == argument) {
            return true;
        }
    }
    return false;
}

#endif  // UTILITY_TEST_BOOST_UNIT_TEST_COMMAND_LINE_HPP_INCLUDED
