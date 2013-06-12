// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CORE_STRING_HPP
#define RAMEN_CORE_STRING_HPP

#include<ramen/core/string_fwd.hpp>

#include<cstddef>
#include<string>
#include<iostream>

#include<boost/move/move.hpp>
#include<boost/functional/hash.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace core
{

/*!
\ingroup core
\brief string class.
*/
class RAMEN_API string_t
{
    BOOST_COPYABLE_AND_MOVABLE( string_t)

public:

    typedef char                char_type;
    typedef std::size_t         size_type;
    typedef const char_type&    const_reference;
    typedef char_type&          reference;
    typedef const char_type*    const_iterator;
    typedef char_type*          iterator;

    string_t();

    string_t( const char_type *str);
    string_t( const char_type *str, std::size_t size);

    template<class Iter>
    string_t( Iter first, Iter last)
    {
        init();
        assign( first, last);
    }

    string_t( const string_t& str, size_type pos, size_type n);

    // from STL string
    explicit string_t( const std::string& s) : pimpl_( 0)
    {
        from_c_string( s.c_str(), s.size());
    }

    ~string_t();

    // Copy constructor
    string_t( const string_t& other);

    // Copy assignment
    string_t& operator=( BOOST_COPY_ASSIGN_REF( string_t) other)
    {
        string_t tmp( other);
        swap( tmp);
        return *this;
    }

    // Move constructor
    string_t( BOOST_RV_REF( string_t) other) : pimpl_( 0)
    {
        RAMEN_ASSERT( other.pimpl_);

        swap( other);
    }

    // Move assignment
    string_t& operator=( BOOST_RV_REF( string_t) other)
    {
        RAMEN_ASSERT( other.pimpl_);

        swap( other);
        return *this;
    }

    template<class Iter>
    void assign( Iter first, Iter last)
    {
        clear();
        // TODO: check if this works ok with boost tokenizer.
        //std::size_t n = std::distance( first, last);
        //reserve( size() + n);

        while( first != last)
            push_back( *first++);
    }

    void swap( string_t& other);

    string_t& operator=( const char *str);

    size_type size() const;

    size_type length() const;

    bool empty() const;

	void reserve( size_type n);

	void clear();

    const char_type *c_str() const;

    void push_back( char_type c);

    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

    // append
    string_t& operator+=( const char_type *str);
    string_t& operator+=( const string_t& str);

    void append( const char_type *str, size_type len);
    void append( const string_t& str);

    std::string to_std_string() const
    {
        RAMEN_ASSERT( pimpl_);

        return std::string( c_str());
    }

    char_type operator[]( size_type index) const;
    char_type& operator[]( size_type index);

private:

    struct impl;
    impl *pimpl_;

    void init( impl *x = 0);

    void from_c_string( const char *str, std::size_t size);
};

inline void swap( string_t& x, string_t& y)
{
    x.swap( y);
}

RAMEN_API string_t operator+( const string_t& a, const string_t& b);
RAMEN_API string_t operator+( const string_t& a, const char *b);

RAMEN_API bool operator==( const string_t& a, const string_t& b);
RAMEN_API bool operator==( const string_t& a, const char *b);
RAMEN_API bool operator==( const char *a, const string_t& b);

RAMEN_API bool operator!=( const string_t& a, const string_t& b);
RAMEN_API bool operator!=( const string_t& a, const char *b);
RAMEN_API bool operator!=( const char *a, const string_t& b);

RAMEN_API bool operator<( const string_t& a, const string_t& b);
RAMEN_API bool operator<( const string_t& a, const char *b);
RAMEN_API bool operator<( const char *a, const string_t& b);

RAMEN_API const string_t make_string( const char *a, const char *b, const char *c = 0, const char *d = 0);

inline std::ostream& operator<<( std::ostream& os, const string_t& str)
{
    return os << str.c_str();
}

inline std::istream& operator>>( std::istream& is, string_t& str)
{
    std::string tmp;
    is >> tmp;
    str.assign( tmp.begin(), tmp.end());
    return is;
}

inline std::size_t hash_value( const string_t& str)
{
    return boost::hash_range( str.begin(), str.end());
}

} // core
} // ramen

#endif
