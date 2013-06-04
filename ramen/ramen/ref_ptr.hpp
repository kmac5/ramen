// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_REF_PTR_HPP
#define RAMEN_REF_PTR_HPP

#include<cassert>

namespace ramen
{

/**
\ingroup utils
\brief smart pointer class that does not assume ownership of the held object
*/
template<class T>
class ref_ptr_t
{
public:

    typedef T element_type;

    ref_ptr_t() : p_( 0) {}

    ref_ptr_t( T *p) : p_( p) {}

    template<typename U>
    ref_ptr_t( const ref_ptr_t<U>& rhs) : p_( rhs.get()) {}

    ref_ptr_t( const ref_ptr_t<T>& rhs) : p_( rhs.p_) {}

    template<typename U>
    ref_ptr_t & operator=( const ref_ptr_t<U>& rhs)
    {
        p_ = rhs.p_;
        return *this;
    }

    ref_ptr_t & operator=( const ref_ptr_t<T>& rhs)
    {
        p_ = rhs.p_;
        return *this;
    }

    ref_ptr_t & operator=( T *rhs)
    {
        p_ = rhs.p_;
        return *this;
    }

    void reset( T *rhs = 0) { p_ = rhs;}

    T * get() const { return p_;}

    T & operator*() const
    {
		assert( p_ != 0);
        return *p_;
    }

    T * operator->() const
    {
        assert( p_ != 0);
        return p_;
    }

    template<class U>
    inline bool operator==( const ref_ptr_t<U>& b) const
    {
        return p_ == b.get();
    }

    template<class U>
    inline bool operator!=( const ref_ptr_t<U>& b) const
    {
        return p_ != b.get();
    }

    template<class U>
    inline bool operator==( U *b) const { return p_ == b;}

    template<class U>
    inline bool operator!=( U *b) const { return p_ != b;}

    inline bool operator<( const ref_ptr_t<T>& b) const
    {
        return p_ < b.get();
    }

    void swap( ref_ptr_t<T>& rhs)
    {
        T * tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;
    }

private:

    T *p_;
};

template<class T, class U>
inline bool operator==( T *a, const ref_ptr_t<U>& b) { return a == b.get();}

template<class T, class U>
inline bool operator!=( T *a, const ref_ptr_t<U>& b) { return a != b.get();}

template<class T>
void swap( ref_ptr_t<T> & lhs, ref_ptr_t<T> & rhs) { lhs.swap( rhs);}

} // namespace

#endif
