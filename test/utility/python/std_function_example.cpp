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

/** \file
Example of some functions that take std::function and can be called from Python.
*/

#include "utility/python/std_function.hpp"

#include <boost/python.hpp>

void call_function_with (std::function <void (int)> const & f, int i)
{ f (i); }

boost::python::object call_python_function (
    std::function <boost::python::object (boost::python::object)> f,
    boost::python::object const & object)
{ return f (object); }

// Function that doubles either the value, or the result from a nullary
// function.
// This is to test whether overloads are disambiguated properly.
double times_two (std::function <double()> const & get_value)
{ return 2 * get_value(); }

int times_two (std::function <int (int)> const & get_value)
{ return 2 * get_value (1); }

double times_two (double value) { return 2 * value; }

BOOST_PYTHON_MODULE (std_function_example) {
    using boost::python::def;

    utility::python::register_std_function <std::function <void (int)>>();
    utility::python::register_std_function <std::function <double()>>();
    utility::python::register_std_function <
        std::function <boost::python::object (boost::python::object)>>();

    def ("call_function_with", call_function_with);

    def ("call_python_function", call_python_function);

    // Register the function first; it should fail to match if the argument
    // is not a function.
    def <double (std::function <double()> const &)> ("times_two", times_two);
    // This will never be called, because it is impossible te distinguish
    // between Python function signatures.
    def <int (std::function <int (int)> const &)> ("times_two", times_two);
    // This will be called.
    def <double (double)> ("times_two", times_two);
}
