/*
Copyright 2009, 2014 Rogier van Dalen.

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

#ifndef UTILITY_TEST_THROWING_HPP_INCLUDED
#define UTILITY_TEST_THROWING_HPP_INCLUDED

#include <type_traits>
#include <memory>
#include <utility>

#include <type_traits>

#include <boost/test/test_tools.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>

#include "thrower.hpp"

#include "utility/is_assignable.hpp"

namespace utility {

    /**
    Wrapper around object that makes it throw in various circumstances.
    This is useful to check exception-safety.
    */
    template <class Content,
        bool ThrowOnConstruction = false,
        bool ThrowOnCopy = false,
        bool ThrowOnMove = false,
        bool ThrowOnCopyAssign = false,
        bool ThrowOnMoveAssign = false,
        bool ThrowOnConversion = false
        >
    class throwing
    {
        thrower & thrower_;
        Content content_;

    public:
        template <class ... Arguments>
        throwing (thrower & t, Arguments && ... arguments)
        noexcept (std::is_nothrow_constructible <Content, Arguments ...>::value
            && !ThrowOnConstruction)
        : thrower_ (t), content_ (std::forward <Arguments> (arguments) ...)
        { thrower_.template throw_point_if <ThrowOnConstruction>(); }

        throwing (throwing const & other)
        noexcept (std::is_nothrow_constructible <Content, Content const &
            >::value && !ThrowOnCopy)
        : thrower_ (other.thrower_), content_ (other.content_)
        { thrower_.template throw_point_if <ThrowOnCopy>(); }

        throwing (throwing && other)
        noexcept (std::is_nothrow_constructible <Content, Content &&
            >::value && !ThrowOnMove)
        : thrower_ (other.thrower_), content_ (std::move (other.content_))
        { thrower_.template throw_point_if <ThrowOnMove>(); }

        ~throwing() {}

        throwing & operator = (throwing const & other)
        noexcept (utility::is_nothrow_assignable <Content &, Content const &
            >::value && !ThrowOnCopyAssign)
        {
            content_ = other.content_;
            thrower_.template throw_point_if <ThrowOnCopyAssign>();
            return *this;
        }

        throwing & operator = (throwing && other)
        noexcept (utility::is_nothrow_assignable <Content &, Content &&>::value
            && !ThrowOnMoveAssign)
        {
            content_ = std::move (other.content_);
            thrower_.template throw_point_if <ThrowOnMoveAssign>();
            return *this;
        }

        const Content & content() const { return content_; }
        Content & content() { return content_; }

        operator Content() const {
            thrower_.template throw_point_if <ThrowOnConversion>();
            return content();
        }

        bool operator == (const throwing & o) const
        { return content_ == o.content_; }
    };

    template <class T> struct is_throwing
    : public boost::mpl::bool_ <false> {};

    template <class Content, bool B1, bool B2, bool B3, bool B4>
        struct is_throwing <throwing <Content, B1, B2, B3, B4> >
    : public boost::mpl::bool_ <true> {};

} // namespace utility

#endif // UTILITY_TEST_THROWING_HPP_INCLUDED
