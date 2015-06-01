#!/usr/bin/python

# Copyright 2014 Rogier van Dalen.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
   
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
