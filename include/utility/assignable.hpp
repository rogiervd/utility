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

#ifndef UTILITY_ASSIGNABLE_HPP_INCLUDED
#define UTILITY_ASSIGNABLE_HPP_INCLUDED

#include <cassert>
#include <new>
#include <type_traits>

#include "storage.hpp"
#include "disable_if_same.hpp"

namespace utility {

    namespace assignable_detail {

        /**
        Keep memory to keep an object, that allows sizeof, but do not initialise
        the memory.
        */
        template <class Content> class memory_for {
        public:
            typedef typename std::aligned_storage <
                sizeof (Content), alignof (Content)>::type memory_type;

        private:
            memory_type memory_;

        public:
            void * memory()
            { return reinterpret_cast <void *> (&this->memory_); }

            void const * memory() const
            { return reinterpret_cast <void const *> (&this->memory_); }

            Content * object()
            { return reinterpret_cast <Content *> (memory()); }

            Content const * object() const
            { return reinterpret_cast <Content const *> (memory()); }
        };

        /**
        Keep memory for an object and a flag to say whether it is constructed.
        The memory must be initialised explicitly, and the flag must be set
        explicitly.
        \tparam AlwaysConstructed
            Set this to true if querying the flag is allowed to always return
            true.
        */
        template <class Content, bool AlwaysConstructed> class manual_optional
        : public memory_for <Content>
        {
            bool constructed_;
        public:
            manual_optional (bool constructed) : constructed_(constructed) {}

            bool constructed() const { return constructed_; }

            void set_constructed (bool constructed)
            { constructed_ = constructed; }
        };

        template <class Type> class manual_optional <Type, true>
        : public memory_for <Type>
        {
        public:
            manual_optional (bool) {}
            bool constructed() const { return true; }
            void set_constructed (bool) {}
        };

        /**
        Return whether for both copy and move construction, either it is
        impossible, or it does not throw.
        */
        template <class Type> struct construction_is_nothrow
        : std::integral_constant <bool,
            (!std::is_constructible <Type, Type const &>::value
                || std::is_nothrow_constructible <Type, Type const &>::value)
            &&
            (!std::is_constructible <Type, Type &&>::value
                || std::is_nothrow_constructible <Type, Type &&>::value)
        > {};

    } // namespace assignable_detail

    /**
    Make a contained object assignable, by destructing and constructing it.
    If the contained object's constructor fails with an exception, try again
    to assign something to get this back in a valid state.
    If copying and moving the contained object is no-throw, then the size of
    this is equal to the size of the object; otherwise a bool is added.
    */
    template <class Content> class assignable
    : assignable_detail::manual_optional <
        typename storage::store <Content>::type,
        assignable_detail::construction_is_nothrow <
            typename storage::store <Content>::type>::value>
    {
    private:
        typedef typename storage::store <Content>::type store_type;
        static bool constexpr always_constructed
            = assignable_detail::construction_is_nothrow <store_type>::value;

        typedef assignable_detail::manual_optional <
            store_type, always_constructed> base_type;

        template <class ... Arguments>
            void construct (Arguments && ... arguments)
        {
            new (this->object()) store_type (
                std::forward <Arguments> (arguments)...);
        }

        void destruct() {
            assert (this->constructed());
            this->object()->~store_type();
        }

        template <class Argument> void assign (Argument && argument) {
            if (this->constructed()) {
                destruct();
                this->set_constructed (false);
            }
            construct (std::forward <Argument> (argument));
            this->set_constructed (true);
        }

    public:
        assignable (assignable const & that) : base_type (true)
        { construct (that.content()); }

        assignable (assignable && that) : base_type (true)
        { construct (std::move (that.content())); }

        assignable (typename utility::storage::pass <Content>::type content)
        : base_type (true)
        { construct (content); }

        assignable (typename
            utility::storage::pass_rvalue <Content>::type content)
        : base_type (true)
        { construct (std::move (content)); }

        ~assignable() {
            if (this->constructed())
                destruct();
        }

        assignable & operator = (assignable const & that) {
            this->assign (*that.object());
            return *this;
        }

        assignable & operator = (assignable && that) {
            this->assign (std::move (*that.object()));
            return *this;
        }

        assignable & operator = (typename
            utility::storage::pass <Content>::type content)
        {
            this->assign (content);
            return *this;
        }

        assignable & operator = (typename
            utility::storage::pass_rvalue <Content>::type content)
        {
            this->assign (std::move (content));
            return *this;
        }

        typename storage::get <Content, assignable &>::type content() {
            assert (this->constructed());
            return *this->object();
        }

        typename storage::get <Content, assignable const &>::type
            content() const
        {
            assert (this->constructed());
            return *this->object();
        }

        typename storage::get <Content, assignable &&>::type move_content() {
            assert (this->constructed());
            return std::move (*this->object());
        }
    };

} // namespace utility

#endif // UTILITY_ASSIGNABLE_HPP_INCLUDED
