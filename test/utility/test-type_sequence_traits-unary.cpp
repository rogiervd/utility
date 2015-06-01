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

#define BOOST_TEST_MODULE test_utility_type_sequence_traits_unary
#include "utility/test/boost_unit_test.hpp"

#include "utility/type_sequence_traits.hpp"

BOOST_AUTO_TEST_SUITE(test_utility_type_sequence_traits_unary)

/**
Check that a unary predicate on a sequence corresponds to one on single types.
*/
template <template <class> class SinglePredicate,
    template <class> class AllPredicate,
    class Type1, class Type2, class Type3>
void check()
{
    static_assert (AllPredicate <meta::vector<>>::value, "");
    static_assert (AllPredicate <meta::vector <Type1>>::value ==
        SinglePredicate <Type1>::value, "");
    static_assert (AllPredicate <meta::vector <Type1, Type2>>::value ==
        (SinglePredicate <Type1>::value && SinglePredicate <Type2>::value), "");
    static_assert (AllPredicate <meta::vector <Type1, Type2, Type3>>::value ==
        (SinglePredicate <Type1>::value && SinglePredicate <Type2>::value
            && SinglePredicate <Type3>::value), "");
}

struct default_constructible {
    std::shared_ptr <int> a;

    default_constructible() : a (std::make_shared <int> (6)) {}
};

struct nothrow_default_constructible {
    int a;

    nothrow_default_constructible() noexcept : a (4) {}
};

struct not_default_constructible {
    std::shared_ptr <int> a;

    not_default_constructible (int i) : a (std::make_shared <int> (i)) {}
};

BOOST_AUTO_TEST_CASE (test_utility_are_default_constructible) {
    // Check that the example types are interesting enough.
    static_assert (utility::is_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (utility::is_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_default_constructible <
        not_default_constructible>::value, "");

    check <utility::is_default_constructible,
        utility::are_default_constructible,
        int, int &, void>();
    check <utility::is_default_constructible,
        utility::are_default_constructible,
        int, default_constructible, not_default_constructible>();
    check <utility::is_default_constructible,
        utility::are_default_constructible,
        default_constructible, not_default_constructible,
        nothrow_default_constructible>();
}

BOOST_AUTO_TEST_CASE (test_utility_are_nothrow_default_constructible) {
    // Check that the example types are interesting enough.
    static_assert (utility::is_nothrow_default_constructible <
        nothrow_default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        default_constructible>::value, "");
    static_assert (!utility::is_nothrow_default_constructible <
        not_default_constructible>::value, "");

    check <utility::is_nothrow_default_constructible,
        utility::are_nothrow_default_constructible,
        int, int &, void>();
    check <utility::is_nothrow_default_constructible,
        utility::are_nothrow_default_constructible,
        int, default_constructible, not_default_constructible>();
    check <utility::is_nothrow_default_constructible,
        utility::are_nothrow_default_constructible,
        default_constructible, not_default_constructible,
        nothrow_default_constructible>();
}

BOOST_AUTO_TEST_SUITE_END()
