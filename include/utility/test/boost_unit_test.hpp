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

#ifndef UTILITY_TEST_BOOST_UNIT_TEST_HPP_INCLUDED
#define UTILITY_TEST_BOOST_UNIT_TEST_HPP_INCLUDED

/**
There are some problems compiling the unit tests on various Boost versions.
This file hopefully abstracts away these problems.
Before including this, define BOOST_TEST_MODULE to be the name of the test.
*/

#ifndef BOOST_TEST_DYN_LINK
// Automatically defined in Boost 1.51.
#   define BOOST_TEST_DYN_LINK
#endif

/*
Clang gives a warning that Boost.Test should probably fix:
.../boost/test/floating_point_comparison.hpp:251:25: error: unused variable
    'check_is_close' [-Werror,-Wunused-variable]
*/
#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-variable"
#   include <boost/test/auto_unit_test.hpp>
#   pragma clang diagnostic pop
#else
#   include <boost/test/auto_unit_test.hpp>
#endif

#endif // UTILITY_TEST_BOOST_UNIT_TEST_HPP_INCLUDED
