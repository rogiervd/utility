/*
Copyright 2006, 2007, 2009, 2014 Rogier van Dalen.

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

/** \file
Define policies for smart pointers.

These policies are nested: they take a base class parameter.
The following kinds of policies are defined:
-   Storage policies: where the object is stored and how it is constructed and
    destructed.
-   Lifetime policies: sharing; when the object is constructed and destructed.
-   Access policies: regulates access to the object.
-   Ordering policies: arranges for pointer comparisons.

Not all of these are orthogonal.
For example, if the object is kept inside the pointer (it might be better to
call it a "box" in this case), this probably forces both a storage and a
lifetime policy.

By swapping some of these in and out, it must be possible to produce a fast and
useful smart pointer for a specific purpose.

\todo Check that unique_ptr and shared_ptr could be implemented in this
framework.
*/

#ifndef UTILITY_POINTER_POLICY_HPP_INCLUDED
#define UTILITY_POINTER_POLICY_HPP_INCLUDED

#include <algorithm> // For std::swap
#include <type_traits>
#include <cassert>

#include <boost/compressed_pair.hpp>

#include "shared.hpp"

namespace utility { namespace pointer_policy {

    template <class ConstructType> struct construct_as {};

    /* Storage policies. */

    /**
    Storage policy that keeps a pointer to an object constructed with "new" and
    destructed with "delete".
    This provides strong exception-safety.
    If construction fails with an exception, the memory will be deallocated.
    */
    template <class Type> class use_new_delete {
    public:
        typedef Type value_type;
    public:
        use_new_delete() noexcept : pointer_ (nullptr) {}

        template <class Derived, class ... Arguments>
        use_new_delete (construct_as <Derived>, Arguments && ... arguments)
        : pointer_ (new Derived (std::forward <Arguments> (arguments)...)) {}

        // Use default copy, move, copy assignment, and move assignment.
        // Use default destructor.

    protected:
        Type * object() const { return pointer_; }

        void reset() noexcept { pointer_ = nullptr; }

        void destruct() noexcept { delete pointer_; }

        void swap (use_new_delete & that) {
            using std::swap;
            swap (this->pointer_, that.pointer_);
        }

    public:
        /**
        \return true iff this owns an object.
        */
        bool empty() const { return pointer_ == nullptr; }

    private:
        Type * pointer_;
    };

    /**
    Storage policy that uses an allocator to construct an object of type "Type"
    and return a pointer.
    This provides strong exception-safety.
    If construction fails with an exception, the memory will be deallocated.
    */
    template <class Type> class allocate_and_construct {
        /**
        Guard that deallocates memory if an exception is thrown.
        */
        template <class Allocator, bool safe> class guard {
            Type * object;
            Allocator & allocator;
            bool set;
        public:
            guard (Type * object, Allocator & allocator) noexcept
            : object (object), allocator (allocator), set (true) {}

            ~guard() noexcept {
                if (set)
                    allocator.deallocate (object, 1);
            }

            void dismiss() noexcept { set = false; }
        };

        // Version for non-throwing constructors.
        template <class Allocator>
            class guard <Allocator, true>
        {
        public:
            guard (Type *, Allocator &) noexcept {}
            ~guard() noexcept {}
            void dismiss() noexcept {}
        };

    public:
        template <class Allocator, class ... Arguments>
            Type * operator() (
                Allocator & allocator, Arguments && ... arguments) const
        {
            static_assert (std::is_same <typename Allocator::value_type,
                typename std::decay <Type>::type>::value,
                "The allocator value_type must match Type.");

            // This works in two steps, allocating memory and constructing the
            // object.
            // That gives a risk of memory leaks.

            // Step 1: allocate memory.
            // This might fail with, say, std::bad_alloc, but then no memory
            // is leaked.
            Type * object = allocator.allocate (1);

            // Step 2: construct the object.
            // But first tell our guard to deallocate the object if something
            // goes wrong.
            static bool const is_safe = noexcept (
                Type (std::forward <Arguments> (arguments) ...));
            guard <Allocator, is_safe> g (object, allocator);
            new (object) Type (std::forward <Arguments> (arguments) ...);
            // If we end up here, construction has succeeded, and the memory
            // does not need to be deallocated.
            g.dismiss();

            return object;
        }

        // With const allocator: copy the allocator.
        template <class Allocator, class ... Arguments>
            Type * operator() (
                Allocator const & allocator_, Arguments && ... arguments) const
        {
            Allocator allocator (allocator_);
            return (*this) (allocator,
                std::forward <Arguments> (arguments) ...);
        }
    };

    /**
    Storage policy that keeps a pointer to an object in memory allocated and
    deallocated with an allocator.
    The life time of the underlying object is not tied to the pointer and must
    be managed by a higher-level policy.
    */
    template <class Type, class Allocator> class use_allocator {
    public:
        typedef Type value_type;
        typedef Allocator allocator_type;

    public:
        use_allocator (Allocator const & allocator)
        noexcept (noexcept (Allocator (allocator)))
        : pointer_and_allocator_ (nullptr, allocator) {}

        /**
        Construct with an object, which must have been allocated with
        \c allocator.
        The object's owner count is increased.
        It is possible to call this on a pointer that another pointer already
        owns; sharing will work.
        Of course, the allocator must be the same.
        However, this would be a great opportunity for bugs.
        Instead, it is usually best to use \c construct.
        */
        use_allocator (Type * object, Allocator const & allocator)
        noexcept (noexcept (Allocator (allocator)))
        : pointer_and_allocator_ (object, allocator) {}

        template <class ... Arguments>
        use_allocator (construct_as <Type>, Allocator const & allocator,
            Arguments && ... arguments)
        : pointer_and_allocator_ (allocate_and_construct <Type>() (
            allocator, std::forward <Arguments> (arguments) ... ), allocator) {}

        // Use default copy, move, copy assignment, and move assignment.
        // Use default destructor.

    protected:
        Type * object() const { return pointer_(); }

        /**
        Make the pointer empty.
        Does not destruct the object.
        */
        void reset() noexcept { pointer_() = nullptr; }

        /**
        Destruct the object and release the memory.
        Does not change the pointer.
        */
        void destruct() noexcept {
            pointer_()->~Type();
            allocator_().deallocate (pointer_(), 1);
        }

        void swap (use_allocator & that) noexcept {
            using std::swap;
            swap (this->pointer_and_allocator_, that.pointer_and_allocator_);
        }

    public:
        /**
        \return true iff this owns an object.
        */
        bool empty() const { return pointer_() == nullptr; }

        /**
        \return The allocator used for the memory for the object.
        */
        Allocator const & allocator() const
        { return allocator_(); }

    private:
        boost::compressed_pair <Type *, Allocator> pointer_and_allocator_;

        Type * & pointer_()
        { return pointer_and_allocator_.first(); }
        Type * pointer_() const
        { return pointer_and_allocator_.first(); }

        Allocator & allocator_()
        { return pointer_and_allocator_.second(); }
        Allocator const & allocator_() const
        { return pointer_and_allocator_.second(); }
    };

    // Dummy class.
    struct move_recursive_next_not_available {};

    /**
    Class that is default-constructed and called with a plain pointer
    (<c>Type *</c>) by with_recursive_type.
    It should return an rvalue reference to the smart pointer to the next object
    in the chain.

    Implement this for an object that is part of such a chain.
    \code
    template <> struct move_recursive_next <your_type> {
        ..._ptr <your_type> operator() (your_type * object) const
        { return object->next_; }
    };
    \endcode

    The default implementation is empty, but derives from
    move_recursive_next_not_available so this can be detected.
    */
    template <class Type> struct move_recursive_next
    : move_recursive_next_not_available {};

    /**
    Storage policy that wraps another storage policy and deals with objects that
    contain a pointer to an object of the same type.
    An example are nodes in a linked list.
    Normally, the pointer's destructor would indirectly call itself if a whole
    string of objects needs to be destructed at once.
    This can lead to stack overflows.
    When this class is used as a storage policy, the recursion is essentially
    turned into iteration.
    Just before the node object's destructor is called, move_recursive_next is
    called and the pointer to the next object is moved out.
    Then the destructor is called, and then the next object is dealt with.

    This policy requires move_recursive_next to be specialised for the value
    type.
    */
    template <class Storage> class with_recursive_type
    : public Storage
    {
    public:
        template <class ... Arguments>
        with_recursive_type (Arguments && ... arguments)
        noexcept (noexcept (Storage (std::forward <Arguments> (arguments) ...)))
        : Storage (std::forward <Arguments> (arguments) ...)
        {}

        // Use default copy, move, copy assignment, and move assignment.
        // Use default destructor.

    protected:
        void destruct() noexcept {
            // Deal with the chain following this object.
            move_recursive_next <typename Storage::value_type> move_next;
            auto && next = move_next (Storage::object());
            typedef typename std::decay <decltype (next)>::type pointer_type;
            if (!next.empty())
                pointer_type::release_chain (std::move (next));

            // Destruct this object (with the pointer to the next object set
            // to null.)
            Storage::destruct();

            // Static check.
            static_assert (
                std::is_base_of <with_recursive_type, pointer_type>::value,
                "move_recursive_next should return the same type as this.");
        }

        with_recursive_type & operator= (with_recursive_type const & that) {
            Storage::operator= (that);
            return *this;
        }

        with_recursive_type & operator= (with_recursive_type && that) {
            Storage::operator= (std::move (that));
            return *this;
        }
    };

    /* Lifetime policies. */

    /**
    Reference counting policy that uses intrusive reference counting.
    The contained object must be derived from shared.
    */
    template <class Storage> class reference_count_shared
    : public Storage
    {
    public:
        template <class ... Arguments>
        reference_count_shared (Arguments && ... arguments)
        noexcept (noexcept (Storage (std::forward <Arguments> (arguments) ...)))
        : Storage (std::forward <Arguments> (arguments) ...)
        {
            static_assert (
                std::is_base_of <shared, typename Storage::value_type>::value,
                "The referenced object must derive from utility::shared.");
            acquire();
        }

        /**
        Copy-construct from another pointer.
        This increases the use count on the underlying object.
        */
        reference_count_shared (reference_count_shared const & that) noexcept
        : Storage (that)
        { acquire(); }

        /**
        Move-construct from another pointer.
        Afterwards, the original pointer will be empty.
        No atomic operations are used.
        */
        reference_count_shared (reference_count_shared && that)
        noexcept (noexcept (Storage (std::declval <Storage &&>())))
        : Storage (std::move (that))
        {
            static_assert (noexcept (that.Storage::reset()), "By definition");
            that.Storage::reset();
        }

        reference_count_shared & operator= (
            reference_count_shared const & that)
        {
            // Save and acquire "that" in case releasing the object in "this"
            // releases that.
            // This can happen in a linked list, or if this == &that.
            that.acquire();
            Storage save = that;
            release();
            Storage::operator= (save);
            return *this;
        }

        reference_count_shared & operator= (reference_count_shared && that) {
            Storage save = std::move (that);
            that.Storage::reset();
            release();
            Storage::operator= (std::move (save));
            return *this;
        }

        ~reference_count_shared() noexcept { release(); }

        /**
        \return The number of objects owning the object this owns.
        0 if this is empty.
        */
        long use_count() const {
            if (!Storage::empty())
                return shared::get_count (Storage::object());
            else
                return 0;
        }

        /**
        \return true iff this is the only owner of the object.
        If this is empty, return false.
        */
        bool unique() const { return use_count() == 1; }

    private:
        void acquire() const noexcept {
            if (!Storage::empty())
                shared::acquire (Storage::object());
        }

        void release() noexcept {
            if (!Storage::empty()
                    && shared::release_count (Storage::object()))
                Storage::destruct();
        }

        template <class Storage2> friend class with_recursive_type;

        /**
        Release a chain of objects iteratively.
        Useful when recursive destruction might cause stack overflows.

        Move the pointer to the next object out before destructing the current
        object.
        Do this to each object in the chain, until the object is shared.

        \pre !p.empty()
        */
        template <class Pointer, class Enable = typename boost::enable_if <
            std::is_base_of <reference_count_shared, Pointer>>::type>
        static void release_chain (Pointer && p) noexcept
        {
            Pointer current = std::move (p);
            move_recursive_next <typename Storage::value_type> move_next;

            // "current" will be manually destructed if necessary.
            assert (!current.empty());
            do {
                bool must_be_destructed
                    = shared::release_count (current.object());

                if (!must_be_destructed) {
                    // Reset the pointer ...
                    current.reset();
                    // ...  because here it goes out of scope, and that would
                    // decrease the use count again.
                    return;
                }

                auto next = move_next (current.object());
                current.destruct();
                // Reset the pointer ...
                current.reset();
                // ... because otherwise the use count will be decreased again
                // here.
                current = std::move (next);
            } while (!current.empty());
        }
    };

    /**
    Access policy that works like a standard pointer: the object is accessible
    as a mutable object whether or not this is const.
    */
    template <class Lifetime> class pointer_access
    : public Lifetime {
    public:
        template <class ... Arguments>
        pointer_access (Arguments && ... arguments)
            noexcept (noexcept (Lifetime (std::declval <Arguments &&>() ...)))
        : Lifetime (std::forward <Arguments> (arguments) ...) {}

        typedef typename Lifetime::value_type value_type;

        value_type & operator* () const { return *Lifetime::object(); }

        value_type * operator-> () const { return Lifetime::object(); }

        /**
        \return A normal pointer to the owned object, or nullptr if this is
        empty.
        */
        value_type * get() const { return Lifetime::object(); }
    };

    /* Ordering policies. */

    /**
    Owner-based equality.
    If two small_ptr's own the same object in memory, they compare equal.
    If they are both empty, they compare equal.
    Otherwise, they compare unequal.
    */
    template <class Access> class strict_weak_ordered
    : public Access {
    public:
        template <class ... Arguments>
        explicit strict_weak_ordered (Arguments && ... arguments)
            noexcept (noexcept (Access (std::declval <Arguments &&>() ...)))
        : Access (std::forward <Arguments> (arguments) ...) {}

        bool operator== (strict_weak_ordered const & that) const
        { return this->get() == that.get(); }
        bool operator!= (strict_weak_ordered const & that) const
        { return this->get() != that.get(); }

        bool operator< (strict_weak_ordered const & that) const
        { return this->get() < that.get(); }
        bool operator<= (strict_weak_ordered const & that) const
        { return this->get() <= that.get(); }

        bool operator> (strict_weak_ordered const & that) const
        { return this->get() > that.get(); }
        bool operator>= (strict_weak_ordered const & that) const
        { return this->get() >= that.get(); }
    };

    /**
    Pointer type that takes policies.
    Its derived class must be constructible from an rvalue "pointer", so that
    the static "construct" can work.
    */
    template <class Policies, class Derived> class pointer
    : public Policies
    {
    public:
        typedef typename Policies::value_type value_type;

        template <class ... Arguments>
        explicit pointer (Arguments && ... arguments)
            noexcept (noexcept (Policies (std::declval <Arguments &&>() ...)))
        : Policies (std::forward <Arguments> (arguments) ...) {}

        /**
        The preferred way of constructing an object of type value_type,
        returning an owner.
        */
        template <class ... Arguments>
            static Derived construct (Arguments && ... arguments)
        {
            return Derived (pointer (construct_as <value_type>(),
                std::forward <Arguments> (arguments) ...));
        }

        /**
        The preferred way of constructing an object of type Type, returning an
        owner.
        */
        template <class Type, class ... Arguments>
            static Derived construct (Arguments && ... arguments)
        {
            return Derived (pointer (construct_as <Type>(),
                std::forward <Arguments> (arguments) ...));
        }

        explicit operator bool() const { return !Policies::empty(); }

        void swap (pointer & that) noexcept { Policies::swap (that); }
    };

    template <class Policies, class Derived> inline
        void swap (pointer <Policies, Derived> & one,
            pointer <Policies, Derived> & other) noexcept
    { one.swap (other); }

}} // namespace utility::pointer_policy

#endif // UTILITY_POINTER_POLICY_HPP_INCLUDED
