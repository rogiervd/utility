// SPDX-FileCopyrightText: Copyright 2015 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

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
{
    typedef Type type;
};

}  // namespace utility

#endif  // UTILITY_ENABLE_IF_COMPILES_INCLUDED
