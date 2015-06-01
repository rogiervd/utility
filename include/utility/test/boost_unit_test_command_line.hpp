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
