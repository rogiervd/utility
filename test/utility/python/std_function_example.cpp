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
