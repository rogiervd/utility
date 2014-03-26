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

#ifndef UTILITY_TEST_TEST_OBJECT_HPP_INCLUDED
#define UTILITY_TEST_TEST_OBJECT_HPP_INCLUDED

#include <type_traits>
#include <memory>
#include <utility>
#include <map>
#include <algorithm>

#include <type_traits>

#include <boost/test/test_tools.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>

#include "thrower.hpp"

#include "utility/is_assignable.hpp"

namespace utility {

    namespace detail {
        class test_object_base;
    } // namespace detail

    class test_object_registry {
        struct slot {
            const std::type_info & info;
            unsigned index;

            slot (const std::type_info & info, unsigned index)
            : info (info), index (index) {}
        };

        friend std::ostream & operator << (std::ostream & os,
            const std::pair <detail::test_object_base *, slot> & object)
        {
            return os << "test_object<" << object.second.info.name()
                << "> object at " << object.first
                << " (allocation " << object.second.index << ")";
        }

        typedef std::map <detail::test_object_base *, slot> registry_type;
        registry_type registry;

        // Emit error about non-destructed object
        static void exists_error (
            const std::pair <detail::test_object_base *, slot> & object)
        {
            BOOST_ERROR ("test_object<" << object.second.info.name()
                << "> object at " << object.first
                << " (allocation " << object.second.index
                << ") not destructed.");
        }

        unsigned index;

        friend class detail::test_object_base;

        test_object_registry (const test_object_registry &) = delete;

    public:
        test_object_registry() : index (0) {}

        ~test_object_registry() { check_done(); }

        void insert (
            detail::test_object_base * object, const std::type_info & type)
        {
            registry_type::iterator i = registry.find (object);
            if (i != registry.end()) {
                BOOST_ERROR ("test_object<" << i->second.info.name()
                    << "> object at " << object
                    << " (allocation " << i->second.index
                    << ") already exists where a test_object<"
                    << type.name()
                    << "> object is now constructed (allocation "
                    << index << ")");
            } else
                registry.insert (i, std::make_pair
                    (object, slot (type, index)));
            ++ index;
        }

        void erase (
            detail::test_object_base * object, const std::type_info & type)
        {
            registry_type::iterator i = registry.find (object);
            if (i == registry.end())
            {
                BOOST_ERROR ("test_object<" << type.name() << "> object at "
                    << object <<
                    " is being destructed without begin constructed.");
            } else {
                if (type != i->second.info) {
                    BOOST_ERROR ("test_object<" << type.name()
                        << "> object at " << object
                        << " is being destructed while a test_object<"
                        << i->second.info.name()
                        << "> object was constructed there (allocation "
                        << i->second.index << ").");
                }
                registry.erase (i);
            }
        }

        void check_done() const
        { std::for_each (registry.begin(), registry.end(), &exists_error); }
    };

    namespace detail {

        class test_object_base {
            std::reference_wrapper <test_object_registry> registry;
            std::reference_wrapper <std::type_info const> type;

            enum state { constructed = 0x132a763d, destructed = 0x45293db6 };
            state state_;
        protected:
            test_object_base (test_object_registry & registry,
                std::type_info const & type)
            : registry (registry), type (type), state_ (constructed)
            { registry.insert (this, type); }

            // Copy (or move).
            test_object_base (test_object_base const & other)
            : registry (other.registry), type (other.type), state_ (constructed)
            { registry.get().insert (this, type); }

            ~test_object_base()
            {
                BOOST_CHECK_EQUAL (state_, constructed);
                state_ = destructed;
                registry.get().erase (this, type);
            }

            test_object_base & operator = (const test_object_base & other) {
                BOOST_CHECK_EQUAL (other.state_, constructed);
                registry.get().erase (this, type);
                registry = other.registry;
                registry.get().insert (this, type);
                type = other.type;
                return *this;
            }
        };

    } // namespace detail

    /**
    Test object that checks construction and destruction and throws as soon as
    it detects any problem.
    Additionally, it can be made to possibly throw exceptions in various
    circumstances.
    */
    template <class Content,
        bool ThrowOnConstruction = false,
        bool ThrowOnCopy = false,
        bool ThrowOnMove = false,
        bool ThrowOnCopyAssign = false,
        bool ThrowOnMoveAssign = false,
        bool ThrowOnConversion = false
        >
    class test_object : detail::test_object_base
    {
        thrower & thrower_;
        Content content;

    public:
        template <class ... Arguments>
        test_object (test_object_registry & registry, thrower & t,
            Arguments && ... arguments)
        noexcept (std::is_nothrow_constructible <Content, Arguments ...>::value
            && !ThrowOnConstruction)
        : detail::test_object_base (registry, typeid (Content)),
            thrower_ (t),
            content (std::forward <Arguments> (arguments) ...)
        { thrower_.template throw_point_if <ThrowOnConstruction>(); }

        test_object (test_object const & other)
        noexcept (std::is_nothrow_constructible <Content, Content const &
            >::value && !ThrowOnCopy)
        : detail::test_object_base (other), thrower_ (other.thrower_),
            content (other.content)
        { thrower_.template throw_point_if <ThrowOnCopy>(); }

        test_object (test_object && other)
        noexcept (std::is_nothrow_constructible <Content, Content &&
            >::value && !ThrowOnMove)
        : detail::test_object_base (std::move (other)),
            thrower_ (other.thrower_),
            content (std::move (other.content))
        { thrower_.template throw_point_if <ThrowOnMove>(); }

        ~test_object() {}

        test_object & operator = (test_object const & other)
        noexcept (utility::is_nothrow_assignable <Content &, Content const &
            >::value && !ThrowOnCopyAssign)
        {
            detail::test_object_base::operator = (other);
            content = other.content;
            thrower_.template throw_point_if <ThrowOnCopyAssign>();
            return *this;
        }

        test_object & operator = (test_object && other)
        noexcept (utility::is_nothrow_assignable <Content &, Content &&>::value
            && !ThrowOnMoveAssign)
        {
            detail::test_object_base::operator = (std::move (other));
            content = std::move (other.content);
            thrower_.template throw_point_if <ThrowOnMoveAssign>();
            return *this;
        }

        const Content & get() const { return content; }
        Content & get() { return content; }

        operator Content() const {
            thrower_.template throw_point_if <ThrowOnConversion>();
            return get();
        }

        bool operator == (const test_object & o) const
        { return content == o.content; }
    };

    template <class T> struct is_test_object
    : public boost::mpl::bool_ <false> {};

    template <class Content, bool B1, bool B2, bool B3, bool B4>
        struct is_test_object <test_object <Content, B1, B2, B3, B4> >
    : public boost::mpl::bool_ <true> {};

} // namespace utility

#endif // UTILITY_TEST_TEST_OBJECT_HPP_INCLUDED
