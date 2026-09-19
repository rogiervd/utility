/*
Copyright 2009, 2014, 2015 Rogier van Dalen.

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

#ifndef UTILITY_TEST_THROWING_HPP_INCLUDED
#define UTILITY_TEST_THROWING_HPP_INCLUDED

#include <memory>
#include <type_traits>
#include <utility>

#include <type_traits>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>
#include <boost/test/test_tools.hpp>

#include "thrower.hpp"

#include "utility/is_assignable.hpp"

namespace utility {

static constexpr int toggle_throw_on_construction = 1;
static constexpr int toggle_throw_on_copy = 2;
static constexpr int toggle_throw_on_move = 4;
static constexpr int toggle_throw_on_copy_assign = 8;
static constexpr int toggle_throw_on_move_assign = 16;
static constexpr int toggle_throw_on_conversion = 32;

/**
Set of toggles for when a \c throwing object should throw.
These toggles work at compile time, so this class has a functional feel to
it.
*/
template <int Bits> struct throw_toggles
{
    static constexpr int bits = Bits;

    static constexpr bool throw_on_construction =
        bool(Bits & toggle_throw_on_construction);
    static constexpr bool throw_on_copy = bool(Bits & toggle_throw_on_copy);
    static constexpr bool throw_on_move = bool(Bits & toggle_throw_on_move);
    static constexpr bool throw_on_copy_assign =
        bool(Bits & toggle_throw_on_copy_assign);
    static constexpr bool throw_on_move_assign =
        bool(Bits & toggle_throw_on_move_assign);
    static constexpr bool throw_on_conversion =
        bool(Bits & toggle_throw_on_conversion);

    friend std::ostream & operator<<(std::ostream & os, throw_toggles t)
    {
        os << "throw on (";
        if (throw_on_construction)
            os << "construction, ";
        if (throw_on_copy)
            os << "copy, ";
        if (throw_on_move)
            os << "move, ";
        if (throw_on_copy_assign)
            os << "copy assignment, ";
        if (throw_on_move_assign)
            os << "move assignment, ";
        if (throw_on_conversion)
            os << "conversion";
        return os << ')';
    }

    throw_toggles<(Bits | 1)> set_throw_on_construction() const
    {
        return throw_toggles<(Bits | 1)>();
    }
    throw_toggles<(Bits | 2)> set_throw_on_copy() const
    {
        return throw_toggles<(Bits | 2)>();
    }
    throw_toggles<(Bits | 4)> set_throw_on_move() const
    {
        return throw_toggles<(Bits | 4)>();
    }
    throw_toggles<(Bits | 8)> set_throw_on_copy_assign() const
    {
        return throw_toggles<(Bits | 8)>();
    }
    throw_toggles<(Bits | 16)> set_throw_on_move_assign() const
    {
        return throw_toggles<(Bits | 16)>();
    }
    throw_toggles<(Bits | 32)> set_throw_on_conversion() const
    {
        return throw_toggles<(Bits | 32)>();
    }
};

typedef throw_toggles<0> do_not_throw;
typedef throw_toggles<63> always_throw;

/**
Wrapper around object that makes it throw in various circumstances.
This is useful to check exception-safety.
*/
template <class Content, class Toggles> class throwing
{
    thrower & thrower_;
    Content content_;

public:
    template <class... Arguments>
    throwing(thrower & t, Arguments &&... arguments) noexcept(
        !Toggles::throw_on_construction)
    : thrower_(t), content_(std::forward<Arguments>(arguments)...)
    {
        thrower_.template throw_point_if<Toggles::throw_on_construction>();
    }

    throwing(throwing const & other) noexcept(!Toggles::throw_on_copy)
    : thrower_(other.thrower_), content_(other.content_)
    {
        thrower_.template throw_point_if<Toggles::throw_on_copy>();
    }

    throwing(throwing && other) noexcept(!Toggles::throw_on_move)
    : thrower_(other.thrower_), content_(std::move(other.content_))
    {
        thrower_.template throw_point_if<Toggles::throw_on_move>();
    }

    ~throwing() {}

    throwing & operator=(throwing const & other) noexcept(
        !Toggles::throw_on_copy_assign)
    {
        content_ = other.content_;
        thrower_.template throw_point_if<Toggles::throw_on_copy_assign>();
        return *this;
    }

    throwing & operator=(throwing && other) noexcept(
        !Toggles::throw_on_move_assign)
    {
        content_ = std::move(other.content_);
        thrower_.template throw_point_if<Toggles::throw_on_move_assign>();
        return *this;
    }

    const Content & content() const { return content_; }
    Content & content() { return content_; }

    operator Content() const noexcept(!Toggles::throw_on_conversion)
    {
        thrower_.template throw_point_if<Toggles::throw_on_conversion>();
        return content();
    }

    bool operator==(const throwing & o) const { return content_ == o.content_; }
};

template <class T> struct is_throwing : public boost::mpl::bool_<false>
{};

template <class Content, class Toggles>
struct is_throwing<throwing<Content, Toggles>> : public boost::mpl::bool_<true>
{};

/**
Call a function that takes a thrower and a ThrowToggles object a few times
with different ThrowToggles types.
*/
template <class TestFunction>
inline void check_with_some_throw_toggles(TestFunction && function)
{
    {
        thrower t;
        function(t, do_not_throw());
    }

    // Common combination.
    check_all_throw_points([&function](thrower & t) {
        function(
            t,
            do_not_throw()
                .set_throw_on_construction()
                .set_throw_on_copy()
                .set_throw_on_copy_assign());
    });

    // Uncommon combination.
    check_all_throw_points([&function](thrower & t) {
        function(
            t,
            do_not_throw()
                .set_throw_on_move()
                .set_throw_on_move_assign()
                .set_throw_on_conversion());
    });

    check_all_throw_points(
        [&function](thrower & t) { function(t, always_throw()); });
}

/**
Call a function that takes a thrower and a ThrowToggles object many times
with different ThrowToggles types.
*/
template <class TestFunction>
inline void check_with_many_throw_toggles(TestFunction && function)
{
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<0>()); });

    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<49>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<16>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<5>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<32>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<8>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<13>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<55>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<17>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<24>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<61>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<60>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<46>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<15>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<23>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<26>()); });
    check_all_throw_points(
        [&function](thrower & t) { function(t, throw_toggles<63>()); });
}

namespace detail {

    template <int ToggleBits> struct check_with_all_throw_toggles
    {
        template <class TestFunction>
        void operator()(TestFunction && function) const
        {
            check_all_throw_points([&function](thrower & t) {
                function(t, throw_toggles<ToggleBits>());
            });
            check_with_all_throw_toggles<ToggleBits - 1>()(function);
        }
    };

    template <> struct check_with_all_throw_toggles<-1>
    {
        template <class TestFunction> void operator()(TestFunction &&) const {}
    };

}  // namespace detail

/**
Call a function that takes a thrower and a ThrowToggles object with all
different ThrowToggles types.
*/
template <class TestFunction>
inline void check_with_all_throw_toggles(TestFunction && function)
{
    detail::check_with_all_throw_toggles<always_throw::bits>()(function);
}

}  // namespace utility

#endif  // UTILITY_TEST_THROWING_HPP_INCLUDED
