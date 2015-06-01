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

struct delegate_constructors {
    int x;

    delegate_constructors() : x (4) {}

    delegate_constructors (int y) : delegate_constructors() {}
};

BOOST_AUTO_TEST_CASE (pass_config_no_cxx11_delegate_constructors) {
    delegate_constructors d (7);
    BOOST_CHECK_EQUAL (d.x, 4);
}
