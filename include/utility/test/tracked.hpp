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

/**
\file
Define an object to check the number of constructions and assignments, and to
track them in memory.
This is useful to test the behaviour of container-like types.
*/

#ifndef UTILITY_TEST_TRACKED_HPP_INCLUDED
#define UTILITY_TEST_TRACKED_HPP_INCLUDED

#include <type_traits>
#include <memory>
#include <utility>
#include <map>
#include <algorithm>

#include <type_traits>

#include <boost/test/test_tools.hpp>

namespace utility {

    template <class Content = void> class tracked;

    class tracked_registry {
        struct slot {
            const std::type_info & info;
            unsigned index;

            slot (const std::type_info & info, unsigned index)
            : info (info), index (index) {}
        };

        friend std::ostream & operator << (std::ostream & os,
            const std::pair <tracked<> *, slot> & object)
        {
            return os << "tracked<" << object.second.info.name()
                << "> object at " << object.first
                << " (allocation " << object.second.index << ")";
        }

        typedef std::map <tracked<> *, slot> registry_type;
        registry_type registry;

        // Emit error about non-destructed object
        static void exists_error (
            const std::pair <tracked<> *, slot> & object)
        {
            BOOST_ERROR ("tracked<" << object.second.info.name()
                << "> object at " << object.first
                << " (allocation " << object.second.index
                << ") not destructed.");
        }

        unsigned index;

        int value_construct_count_;
        int copy_count_;
        int move_count_;
        int copy_assign_count_;
        int move_assign_count_;
        int swap_count_;
        int destruct_count_;
        int destruct_moved_count_;

        friend class tracked<>;

        tracked_registry (const tracked_registry &) = delete;

    public:
        tracked_registry()
        : index(), value_construct_count_(), copy_count_(),
            move_count_(), copy_assign_count_(), move_assign_count_(),
            swap_count_(), destruct_count_(), destruct_moved_count_() {}

        ~tracked_registry() { check_done(); }

        void insert (tracked<> * object, const std::type_info & type) {
            registry_type::iterator i = registry.find (object);
            if (i != registry.end()) {
                BOOST_ERROR ("tracked<" << i->second.info.name()
                    << "> object at " << object
                    << " (allocation " << i->second.index
                    << ") already exists where a tracked<"
                    << type.name()
                    << "> object is now constructed (allocation "
                    << index << ")");
            } else
                registry.insert (i, std::make_pair
                    (object, slot (type, index)));
            ++ index;
        }

        void erase (tracked<> * object, const std::type_info & type) {
            registry_type::iterator i = registry.find (object);
            if (i == registry.end())
            {
                BOOST_ERROR ("tracked<" << type.name() << "> object at "
                    << object <<
                    " is being destructed without being constructed.");
            } else {
                if (type != i->second.info) {
                    BOOST_ERROR ("tracked<" << type.name()
                        << "> object at " << object
                        << " is being destructed while a tracked<"
                        << i->second.info.name()
                        << "> object was constructed there (allocation "
                        << i->second.index << ").");
                }
                registry.erase (i);
            }
        }

        void check_done() const {
            BOOST_CHECK (consistent());
            std::for_each (registry.begin(), registry.end(), &exists_error);
        }

        bool consistent() const {
            // All constructed objects must have been destructed.
            if (value_construct_count_ + copy_count_ + move_count_ !=
                destruct_count_ + destruct_moved_count_)
                return false;
            // All carcasses must have been destructed.
            if (move_count_ + move_assign_count_ != destruct_moved_count_)
                return false;
            return true;
        }

        int value_construct_count() const { return value_construct_count_; }
        int copy_count() const { return copy_count_; }
        int move_count() const { return move_count_; }
        int copy_assign_count() const { return copy_assign_count_; }
        int move_assign_count() const { return move_assign_count_; }
        int swap_count() const { return swap_count_; }
        int destruct_count() const { return destruct_count_; }
        int destruct_moved_count() const { return destruct_moved_count_; }

        void check_counts (int expected_value_construct_count,
            int expected_copy_count, int expected_move_count,
            int expected_copy_assign_count, int expected_move_assign_count,
            int expected_swap_count,
            int expected_destruct_count, int expected_destruct_moved_count)
        const
        {
            BOOST_CHECK_EQUAL (value_construct_count(),
                expected_value_construct_count);
            BOOST_CHECK_EQUAL (copy_count(), expected_copy_count);
            BOOST_CHECK_EQUAL (move_count(), expected_move_count);
            BOOST_CHECK_EQUAL (copy_assign_count(), expected_copy_assign_count);
            BOOST_CHECK_EQUAL (move_assign_count(), expected_move_assign_count);
            BOOST_CHECK_EQUAL (swap_count(), expected_swap_count);
            BOOST_CHECK_EQUAL (destruct_count(), expected_destruct_count);
            BOOST_CHECK_EQUAL (destruct_moved_count(),
                expected_destruct_moved_count);
        }
    };

    template<> class tracked<> {
        tracked_registry & registry;
        std::reference_wrapper <std::type_info const> type;

        enum state {
            // These values are actually from random.org, so even though they
            // might not look random, they are.
            valid = 0xCBFCDDFB,
            moved = 0x00633222,
            invalid = 0x45AE60C4
        };
        state state_;

        bool is_valid() const { return state_ == valid; }
        bool is_moved() const { return state_ == moved; }
        bool is_invalid() const { return state_ == invalid; }

        void set_moved() { state_ = moved; }
        void invalidate() { state_ = invalid; }
    public:
        tracked (tracked_registry & registry,
            std::type_info const & type = typeid (void))
        : registry (registry), type (type), state_ (valid)
        {
            registry.insert (this, type);
            ++ registry.value_construct_count_;
        }

        // Copy.
        tracked (tracked const & other)
        : registry (other.registry), type (other.type), state_ (valid)
        {
            BOOST_CHECK (other.is_valid());
            registry.insert (this, type);
            ++ registry.copy_count_;
        }

        // Move.
        tracked (tracked && other)
        : registry (other.registry), type (other.type), state_ (valid)
        {
            BOOST_CHECK (other.is_valid());
            other.set_moved();
            registry.insert (this, type);
            ++ registry.move_count_;
        }

        ~tracked() {
            if (is_valid())
                ++ registry.destruct_count_;
            else {
                BOOST_CHECK (is_moved());
                ++ registry.destruct_moved_count_;
            }
            invalidate();
            registry.erase (this, type);
        }

        tracked & operator = (tracked const & other) {
            BOOST_CHECK_EQUAL (&registry, &other.registry);
            BOOST_CHECK (this->is_valid());
            BOOST_CHECK (other.is_valid());

            ++ registry.copy_assign_count_;

            registry.erase (this, type);
            type = other.type;
            registry.insert (this, type);
            return *this;
        }
        tracked & operator = (tracked && other) {
            BOOST_CHECK_EQUAL (&registry, &other.registry);
            BOOST_CHECK (this->is_valid());
            BOOST_CHECK (other.is_valid());
            other.set_moved();

            ++ registry.move_assign_count_;

            registry.erase (this, type);
            type = other.type;
            registry.insert (this, type);
            return *this;
        }

        void swap (tracked & other) {
            BOOST_CHECK_EQUAL (&registry, &other.registry);
            BOOST_CHECK (this->is_valid());
            BOOST_CHECK (other.is_valid());
            ++ registry.swap_count_;
        }
    };

    template <class Content> class tracked
    : public tracked <void>
    {
        Content content_;
    public:
        template <class ... Arguments>
        tracked (tracked_registry & counter, Arguments && ... arguments)
        : tracked <void> (counter, typeid (Content)),
            content_ (std::forward <Arguments> (arguments) ...) {}

        template <class OtherContent>
        tracked (tracked <OtherContent> const & other, typename
            boost::enable_if <std::is_convertible <
                OtherContent const &, Content>>::type * = 0)
        : tracked <void> (other), content_ (other.content()) {}

        template <class OtherContent>
        tracked (tracked <OtherContent> && other, typename
            boost::enable_if <std::is_convertible <
                OtherContent, Content>>::type * = 0)
        : tracked <void> (std::move (other)),
            content_ (std::move (other.content())) {}

        /* Default-provided constructors and assignment operators are fine. */

        void swap (tracked & other) {
            tracked <void>::swap (other);
            using std::swap;
            swap (this->content_, other.content_);
        }

        Content const & content() const { return content_; }
        Content & content() { return content_; }
    };

    template <class Content>
    inline void swap (tracked <Content> & t1, tracked <Content> & t2)
    { t1.swap (t2); }

} // namespace utility

#endif // UTILITY_TEST_TRACKED_HPP_INCLUDED
