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

#define BOOST_TEST_MODULE test_utility_make_unique
#include "utility/test/boost_unit_test.hpp"

#include "utility/make_unique.hpp"

#include <type_traits>

BOOST_AUTO_TEST_SUITE(test_utility_make_unique)

bool exists = false;

struct type {
    type() { exists = true; }
    ~type() { exists = false; }
};

BOOST_AUTO_TEST_CASE (test_utility_make_unique) {
    auto p = utility::make_unique <type>();
    static_assert (std::is_same <decltype (p), std::unique_ptr <type>>::value,
        "");
    BOOST_CHECK (exists);
    p.reset();
    BOOST_CHECK (!exists);
}

BOOST_AUTO_TEST_SUITE_END()
