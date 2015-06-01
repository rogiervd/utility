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

#define BOOST_TEST_MODULE pass_config_no_cxx11_delegate_constructors
#include "utility/test/boost_unit_test.hpp"

#include "utility/config.hpp"

BOOST_AUTO_TEST_SUITE (pass_config_no_cxx11_delegate_constructors)

#ifdef UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
// Pass.
#else
#include "trigger-config_no_cxx11_delegate_constructors.ipp"
#endif

BOOST_AUTO_TEST_SUITE_END()
