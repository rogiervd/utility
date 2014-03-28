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

#ifndef UTILITY_TEST_THROWER_HPP_INCLUDED
#define UTILITY_TEST_THROWER_HPP_INCLUDED

#include <functional>

#include <type_traits>

namespace utility {

    class thrower_exception : public std::exception {};

    /**
    Static class that throws every once in a while when throw_point_if is
    called.
    This is enabled by calling \c cycle.
    This is useful for checking exception-safety.
    */
    class thrower {
        int cycle_;
        int count_;
        int disabled_;

    public:
        thrower() : cycle_ (-1), count_ (0), disabled_ (0) {}

        // Don't copy by accident.
        thrower (thrower const &) = delete;

        int count() const { return count_; }
        void reset() { count_ = 0; }
        void set_cycle (int c) { cycle_ = c; }

        void throw_point() {
            if (!disabled_) {
                ++ count_;
                if (count_ == cycle_) {
                    count_ = 0;
                    throw thrower_exception();
                }
            }
        }


        template <bool enable>
        typename std::enable_if <enable>::type throw_point_if()
        { throw_point(); }

        template <bool enable>
        typename std::enable_if <!enable>::type throw_point_if()
        {}

        class pause {
            thrower & t;
        public:
            pause (thrower & t)
            : t (t)
            { ++ t.disabled_; }

            ~pause()
            { -- t.disabled_; }
        };
    };

    /**
    Call the function many times, each time throwing at the next occurance of
    throw_point.
    This is useful to perform a blanket check for exception-safety.
    */
    inline void check_all_throw_points (
        std::function <void (thrower &)> const & f)
    {
        int max_count;
        {
            thrower t;
            // Don't throw at first
            t.set_cycle (-1);
            try {
                f (t);
            } catch (...) {}
            max_count = t.count();
        }

        for (int cycle = 1; cycle <= max_count; ++ cycle) {
            thrower t;
            t.set_cycle (cycle);
            try {
                f (t);
            } catch (thrower_exception &) {}
        }
    }

} // namespace utility

#endif // UTILITY_TEST_THROWER_HPP_INCLUDED
