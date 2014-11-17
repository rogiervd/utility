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

    /**
    Make a contained object assignable, by destructing and constructing it.
    If the contained object's constructor fails with an exception, try again
    to assign something to get this back in a valid state.
    */
    template <class Content> class assignable {
    private:
        typedef typename storage::store <Content>::type store_type;
        typedef typename std::aligned_storage <
            sizeof (store_type), alignof (store_type)>::type memory_type;

        memory_type memory_;
        bool constructed;

        void * memory() { return reinterpret_cast <void *> (&this->memory_); }

        void const * memory() const
        { return reinterpret_cast <void const *> (&this->memory_); }

        store_type * object()
        { return reinterpret_cast <store_type *> (memory()); }

        store_type const * object() const
        { return reinterpret_cast <store_type const *> (memory()); }

        template <class ... Arguments>
            void construct (Arguments && ... arguments)
        { new (object()) store_type (std::forward <Arguments> (arguments)...); }

        void destruct() {
            assert (this->constructed);
            object()->~store_type();
        }

        template <class Argument> void assign (Argument && argument) {
            if (this->constructed) {
                destruct();
                constructed = false;
            }
            construct (std::forward <Argument> (argument));
            constructed = true;
        }

    public:
        assignable (assignable const & that) : constructed (true)
        { construct (that.content()); }

        assignable (assignable && that) : constructed (true)
        { construct (std::move (that.content())); }

        template <class ... Arguments, class Enable =
            typename disable_if_variadic_same_or_derived <
                assignable, Arguments ...>::type>
        assignable (Arguments && ... arguments)
        : constructed (true)
        { construct (std::forward <Arguments> (arguments) ...); }

        ~assignable() {
            if (constructed)
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

        template <class Argument, class Enable = typename
            disable_if_variadic_same_or_derived <assignable, Argument>::type>
        assignable & operator = (Argument && argument)
        {
            this->assign (std::forward <Argument> (argument));
            return *this;
        }

        typename storage::get <Content, assignable &>::type content() {
            assert (this->constructed);
            return *this->object();
        }

        typename storage::get <Content, assignable const &>::type
            content() const
        {
            assert (this->constructed);
            return *this->object();
        }

        typename storage::get <Content, assignable &&>::type move_content() {
            assert (this->constructed);
            return std::move (*this->object());
        }
    };

} // namespace utility

#endif // UTILITY_ASSIGNABLE_HPP_INCLUDED
