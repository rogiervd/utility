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

#ifndef UTILITY_CONFIG_HPP_INCLUDED
#define UTILITY_CONFIG_HPP_INCLUDED

#if defined __clang__

#   if !__has_feature (cxx_delegating_constructors)
#       define UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
#   endif

// Random crashes occur on CLang 3.0 when many templates are instantiated.
// This makes unit tests fail, so it may be worth switching them off.
#   if (__clang_major__ == 3 && __clang_minor__ == 0)
#       define UTILITY_CONFIG_CLANG_RANDOM_TEMPLATE_CRASHES
#   endif

#elif defined __GNUC__

#   if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7) \
            || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#       define UTILITY_CONFIG_NO_CXX11_DELEGATE_CONSTRUCTORS
#   endif

#endif

#endif // UTILITY_CONFIG_HPP_INCLUDED
