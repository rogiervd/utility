#!/usr/bin/python

# Copyright 2014 Rogier van Dalen.

# This file is part of Rogier van Dalen's Utility library for C++.

# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from std_function_example import *

value = [0]

def increase_value_by (i):
    value[0] += i

call_function_with (increase_value_by, 7)
assert (value[0] == 7)
call_function_with (increase_value_by, 4)
assert (value[0] == 11)

def three():
    return 3.5

# Automatically select the right overload.
v = times_two (12.5)
assert (v == 25)
v = times_two (three)
assert (v == 7)

# Returning a Python object should work.
def add_67_and_return (d):
    d [6] = 7
    return d

d = {4:5}
d2 = call_python_function (add_67_and_return, d)
assert (d [6] == 7)
assert (d is d2)

# Cause an error.
try:
    call_function_with (5, 6)
    # An exception should have been raised..
    assert (False)
# This does not work:
# except Boost.Python.ArgumentException:
# See https://mail.python.org/pipermail/cplusplus-sig/2010-April/015470.html
except Exception, e:
    if (not str(e).startswith("Python argument types in")):
        raise
