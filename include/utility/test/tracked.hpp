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
#include <iosfwd>

#include <type_traits>

#include <boost/test/test_tools.hpp>

namespace utility {

    template <class Content = void> class tracked;

    /**
    Counts for the number of items tracked by tracked_registry that are
    constructed, assigned, swapped or destructed.
    Objects of this class can be added and subtracted.
    They can be compared, and they can be streamed to an ostream.
    It is therefore possible to use testing code to check them for equality
    directly.
    */
    struct tracked_counts {
        int value_construct_count;
        int copy_count;
        int move_count;
        int copy_assign_count;
        int move_assign_count;
        int swap_count;
        int destruct_count;
        int destruct_moved_count;

        tracked_counts()
        : value_construct_count (0), copy_count (0), move_count (0),
        copy_assign_count (0), move_assign_count (0), swap_count (0),
        destruct_count (0), destruct_moved_count (0) {}

        tracked_counts (
            int value_construct_count, int copy_count, int move_count,
            int copy_assign_count, int move_assign_count,
            int swap_count, int destruct_count, int destruct_moved_count)
        : value_construct_count (value_construct_count),
            copy_count (copy_count), move_count (move_count),
            copy_assign_count (copy_assign_count),
            move_assign_count (move_assign_count),
            swap_count (swap_count), destruct_count (destruct_count),
            destruct_moved_count (destruct_moved_count) {}

        bool operator== (tracked_counts const & other) const {
            return this->value_construct_count == other.value_construct_count
                && this->copy_count == other.copy_count
                && this->move_count == other.move_count
                && this->copy_assign_count == other.copy_assign_count
                && this->move_assign_count == other.move_assign_count
                && this->swap_count == other.swap_count
                && this->destruct_count == other.destruct_count
                && this->destruct_moved_count == other.destruct_moved_count;
        }

        tracked_counts operator+ (tracked_counts const & other) const {
            return tracked_counts {
                this->value_construct_count + other.value_construct_count,
                this->copy_count + other.copy_count,
                this->move_count + other.move_count,
                this->copy_assign_count + other.copy_assign_count,
                this->move_assign_count + other.move_assign_count,
                this->swap_count + other.swap_count,
                this->destruct_count + other.destruct_count,
                this->destruct_moved_count + other.destruct_moved_count
            };
        }

        tracked_counts operator- (tracked_counts const & other) const {
            return tracked_counts {
                this->value_construct_count - other.value_construct_count,
                this->copy_count - other.copy_count,
                this->move_count - other.move_count,
                this->copy_assign_count - other.copy_assign_count,
                this->move_assign_count - other.move_assign_count,
                this->swap_count - other.swap_count,
                this->destruct_count - other.destruct_count,
                this->destruct_moved_count - other.destruct_moved_count
            };
        }

        int alive_count() const {
            return (value_construct_count + copy_count + move_count)
                - (destruct_count + destruct_moved_count);
        }
    };

    /// Return a tracked_counts with counts 0 except for value_construct_count.
    inline tracked_counts value_construct_count (int count) {
        tracked_counts result;
        result.value_construct_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for copy_count.
    inline tracked_counts copy_count (int count) {
        tracked_counts result;
        result.copy_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for move_count.
    inline tracked_counts move_count (int count) {
        tracked_counts result;
        result.move_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for copy_assign_count.
    inline tracked_counts copy_assign_count (int count) {
        tracked_counts result;
        result.copy_assign_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for move_assign_count.
    inline tracked_counts move_assign_count (int count) {
        tracked_counts result;
        result.move_assign_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for swap_count.
    inline tracked_counts swap_count (int count) {
        tracked_counts result;
        result.swap_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for destruct_count.
    inline tracked_counts destruct_count (int count) {
        tracked_counts result;
        result.destruct_count = count;
        return result;
    }

    /// Return a tracked_counts with counts 0 except for destruct_moved_count.
    inline tracked_counts destruct_moved_count (int count) {
        tracked_counts result;
        result.destruct_moved_count = count;
        return result;
    }

    inline std::ostream & operator<< (
        std::ostream & os, tracked_counts const & counts)
    {
        os << '(';
        if (counts.value_construct_count != 0)
            os << "value_construct: " << counts.value_construct_count << ", ";
        if (counts.copy_count != 0)
            os << "copy: " << counts.copy_count << ", ";
        if (counts.move_count != 0)
            os << "move: " << counts.move_count << ", ";
        if (counts.copy_assign_count != 0)
            os << "copy_assign: " << counts.copy_assign_count << ", ";
        if (counts.move_assign_count != 0)
            os << "move_assign: " << counts.move_assign_count << ", ";
        if (counts.swap_count != 0)
            os << "swap: " << counts.swap_count << ", ";
        if (counts.destruct_count != 0)
            os << "destruct: " << counts.destruct_count << ", ";
        if (counts.destruct_moved_count != 0)
            os << "destruct_moved: " << counts.destruct_moved_count << ", ";
        os << ")";
        return os;
    }

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

        tracked_counts counts_;

        friend class tracked<>;

        tracked_registry (const tracked_registry &) = delete;

    public:
        tracked_registry()
        : index(), counts_() {}

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

        /**
        Check that all constructed objects have been destructed and use
        BOOST_ERROR for those that are not.
        */
        void check_done() const {
            BOOST_CHECK (finished());
            std::for_each (registry.begin(), registry.end(), &exists_error);
        }

        /// Return whether all constructed objects have been destructed.
        bool finished() const {
            return counts_.alive_count() == 0;
        }

        /// Return the current counts.
        tracked_counts const & counts() const { return counts_; }

        /// Return the difference between the current counts and the counts at
        /// a previous time.
        tracked_counts since (tracked_counts const & start) const
        { return counts_ - start; }
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

        void set_valid() { state_ = valid; }
        void set_moved() { state_ = moved; }
        void invalidate() { state_ = invalid; }

    protected:
        bool is_valid() const { return state_ == valid; }
        bool is_invalid() const { return state_ == invalid; }
    public:
        bool is_moved() const { return state_ == moved; }

        tracked (tracked_registry & registry,
            std::type_info const & type = typeid (void))
        : registry (registry), type (type), state_ (valid)
        {
            registry.insert (this, type);
            ++ registry.counts_.value_construct_count;
        }

        // Copy.
        tracked (tracked const & other)
        : registry (other.registry), type (other.type), state_ (valid)
        {
            BOOST_CHECK (other.is_valid());
            registry.insert (this, type);
            ++ registry.counts_.copy_count;
        }

        // Move.
        tracked (tracked && other)
        : registry (other.registry), type (other.type), state_ (valid)
        {
            BOOST_CHECK (other.is_valid());
            other.set_moved();
            registry.insert (this, type);
            ++ registry.counts_.move_count;
        }

        ~tracked() {
            if (is_valid())
                ++ registry.counts_.destruct_count;
            else {
                BOOST_CHECK (is_moved());
                ++ registry.counts_.destruct_moved_count;
            }
            invalidate();
            registry.erase (this, type);
        }

        tracked & operator = (tracked const & other) {
            BOOST_CHECK_EQUAL (&registry, &other.registry);
            BOOST_CHECK (this->is_valid() || this->is_moved());
            BOOST_CHECK (other.is_valid());
            this->set_valid();

            ++ registry.counts_.copy_assign_count;

            registry.erase (this, type);
            type = other.type;
            registry.insert (this, type);
            return *this;
        }
        tracked & operator = (tracked && other) {
            BOOST_CHECK_EQUAL (&registry, &other.registry);
            BOOST_CHECK (this->is_valid() || this->is_moved());
            BOOST_CHECK (other.is_valid());
            this->set_valid();
            other.set_moved();

            ++ registry.counts_.move_assign_count;

            registry.erase (this, type);
            type = other.type;
            registry.insert (this, type);
            return *this;
        }

        void swap (tracked & other) {
            BOOST_CHECK_EQUAL (&registry, &other.registry);
            BOOST_CHECK (this->is_valid());
            BOOST_CHECK (other.is_valid());
            ++ registry.counts_.swap_count;
        }
    };

    template <class Content> class tracked
    : public tracked <void>
    {
        template <class Content2> friend class tracked;
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
        : tracked <void> (other), content_ (other.content_) {}

        template <class OtherContent>
        tracked (tracked <OtherContent> && other, typename
            boost::enable_if <std::is_convertible <
                OtherContent, Content>>::type * = 0)
        : tracked <void> (std::move (other)),
            content_ (std::move (other.content_)) {}

        /* Default-provided constructors and assignment operators are fine. */

        void swap (tracked & other) {
            tracked <void>::swap (other);
            using std::swap;
            swap (this->content_, other.content_);
        }

        Content const & content() const {
            BOOST_CHECK (this->is_valid());
            return content_;
        }

        Content & content() {
            BOOST_CHECK (this->is_valid());
            return content_;
        }

        template <class OtherContent>
            bool operator== (tracked <OtherContent> const & other) const
        { return this->content() == other.content(); }

        template <class OtherContent>
            bool operator!= (tracked <OtherContent> const & other) const
        { return this->content() != other.content(); }
    };

    template <class Content>
    inline void swap (tracked <Content> & t1, tracked <Content> & t2)
    { t1.swap (t2); }

} // namespace utility

#endif // UTILITY_TEST_TRACKED_HPP_INCLUDED
