/*
Copyright 2015 Rogier van Dalen.

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

#ifndef UTILITY_ENABLE_IF_COMPILES_INCLUDED
#define UTILITY_ENABLE_IF_COMPILES_INCLUDED

namespace utility {

/** \void
Contain a type \a Type.

That sounds limited.
The use case for this is that the template arguments can be subject to SFINAE,
for example using \c decltype.

\tparam Condition
    Type that may well not compile.
\tparam Type
    (optional) The type of the \c type member typedef.
    If not given, \c type is \c void.
*/
template <class Condition, class Type = void> struct enable_if_compiles
{ typedef Type type; };

} // namespace utility

#endif // UTILITY_ENABLE_IF_COMPILES_INCLUDED
