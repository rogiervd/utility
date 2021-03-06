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
Register std::function with Boost.Python.
*/

#ifndef UTILITY_PYTHON_STD_FUNCTION_HPP_INCLUDED
#define UTILITY_PYTHON_STD_FUNCTION_HPP_INCLUDED

// Include the Python C API.
// This must be included before any standard headers, the documentation says.
#include <Python.h>

#include <functional>

#include <boost/python/object.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/converter/registry.hpp>

namespace utility { namespace python {

/**
Construct an object of this class to register std::function for a specific
function type with Boost.Python.
This should be done in the BOOST_PYTHON_MODULE function.

This makes it possible, for example, to call from Python functions that take
std::function as an argument.

The conversion to std::function is blocked if the Python object is not callable.
That is a low bar, since the number of arguments or their types cannot be
checked.
But it is possible to distinguish between overloads taking functions and those
taking other types of values.

\tparam StdFunction
    The specialisation of std::function to register.
*/
template <class StdFunction> class register_std_function;

template <class FunctionType>
    class register_std_function <std::function <FunctionType>>
{
public:
    register_std_function() {
        boost::python::converter::registry::push_back (
            &convertible, &construct,
            boost::python::type_id <std::function <FunctionType>>());
    }

private:
    // Always convertible if the object is a Python callable.
    static void * convertible (PyObject * pointer)
    { return PyCallable_Check (pointer) ? pointer : nullptr; }

    /// Call a Python object and extract the desired result type.
    template <class FunctionType2> class extract_result;

    // Result is not void.
    template <class Result, class ... Arguments>
        class extract_result <Result (Arguments ...)>
    {
        boost::python::object function_;
    public:
        extract_result (boost::python::object && function)
        : function_ (function) {}

        template <class ... ActualArguments>
            Result operator() (ActualArguments && ... arguments) const
        {
            return boost::python::extract <Result> (
                function_ (std::forward <ActualArguments> (arguments) ...));
        }
    };

    // Result is void.
    template <class ... Arguments> class extract_result <void (Arguments ...)> {
        boost::python::object function_;
    public:
        extract_result (boost::python::object && function)
        : function_ (function) {}

        template <class ... ActualArguments>
            void operator() (ActualArguments && ... arguments) const
        {
            function_ (std::forward <ActualArguments> (arguments) ...);
        }
    };

    static void construct (PyObject* python_object,
        boost::python::converter::rvalue_from_python_stage1_data * data)
    {
        typedef std::function <FunctionType> function_type;
        boost::python::object object (boost::python::handle<> (
            boost::python::borrowed (python_object)));
        typedef boost::python::converter::rvalue_from_python_storage
            <function_type> storage_type;
        void * storage = reinterpret_cast <storage_type*> (data)->storage.bytes;

        // Objects of type boost::python::function can be called, so it can be
        // passed straight to std::function.
        new (storage) function_type (
            extract_result <FunctionType> (std::move (object)));

        data->convertible = storage;
    }
};

}} // namespace utility::python

#endif // UTILITY_PYTHON_STD_FUNCTION_HPP_INCLUDED
