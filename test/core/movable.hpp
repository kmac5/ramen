// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CORE_TEST_MOVABLE_HPP
#define RAMEN_CORE_TEST_MOVABLE_HPP

#include<cassert>

#include<boost/move/move.hpp>

// A movable type, used in tests.
class movable_t
{
    BOOST_COPYABLE_AND_MOVABLE( movable_t)

public:

    movable_t()
    {
        ptr_ = new int( 0);
    }

    explicit movable_t( int x)
    {
        ptr_ = new int( x);
    }

    movable_t( const movable_t& other)
    {
        ptr_ = new int( other.value());
    }

   movable_t( BOOST_RV_REF( movable_t) other) : ptr_( other.ptr_)
   {
        assert( ptr_);
        assert( other.ptr_);

        other.ptr_ = 0;
   }

    ~movable_t()
    {
        delete ptr_;
    }

    movable_t& operator=( BOOST_COPY_ASSIGN_REF( movable_t) other) // Copy assignment
    {
        assert( ptr_);
        assert( other.ptr_);

        if( this != &other)
        {
            int *tmp_p = other.ptr_ ? new int( other.value()) : 0;
            delete ptr_;
            ptr_ = tmp_p;
        }

        return *this;
    }

   movable_t& operator=( BOOST_RV_REF( movable_t) other) //Move assignment
   {
       assert( ptr_);
       assert( other.ptr_);

       if (this != &other)
       {
           delete ptr_;
           ptr_ = other.ptr_;
           other.ptr_ = 0;
       }

       return *this;
   }

    int value() const
    {
        assert( ptr_);

        return *ptr_;
    }

    bool operator==( const movable_t& other)
    {
        if( ptr_ == 0 || other.ptr_ == 0)
            return ptr_ == other.ptr_;

        return *ptr_ == *other.ptr_;
    }

    bool operator!=( const movable_t& other)
    {
        return !( *this == other);
    }

    bool was_moved() const
    {
        return ptr_ == 0;
    }

private:

    int *ptr_;
};

movable_t make_movable( int x) { return movable_t( x);}

// traits
namespace boost
{

template<>
struct has_nothrow_move<movable_t>
{
   static const bool value = true;
};

} // boost

#endif
