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

struct delegate_constructors {
    int x;

    delegate_constructors() : x (4) {}

    delegate_constructors (int y) : delegate_constructors() {}
};

BOOST_AUTO_TEST_CASE (pass_config_no_cxx11_delegate_constructors) {
    delegate_constructors d (7);
    BOOST_CHECK_EQUAL (d.x, 4);
}
