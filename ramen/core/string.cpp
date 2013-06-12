// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/core/string.hpp>

#include<stdlib.h>
#include<string.h>

#include<algorithm>
#include<cstring>

#include<boost/container/vector.hpp>

#include<ramen/core/memory.hpp>
#include<ramen/core/exceptions.hpp>

namespace ramen
{
namespace core
{
namespace
{

template<class Char>
std::size_t string_length( const Char *str)
{
    RAMEN_ASSERT( str);

    const Char *p = str;
    std::size_t size = 0;

    while( *p)
    {
        ++size;
        ++p;
    }

    return size;
}

template<class Char>
int string_compare( const Char *a, const Char *b)
{
    RAMEN_ASSERT( a);
    RAMEN_ASSERT( b);

    while( true)
    {
        Char ac = *a;
        Char bc = *b;

        if( *a < *b)
            return -1;
        else
            if( *a > *b)
                return 1;

        if( ac == 0 || bc == 0)
            return 0;

        ++a;
        ++b;
    }

    return 0;
}

} // unnamed

struct string_t::impl
{
    boost::container::vector<string_t::char_type> str_;
};

string_t::string_t() : pimpl_( 0)
{
    init();
}

string_t::string_t( const char_type *str) : pimpl_( 0)
{
    from_c_string( str, string_length( str));
}

string_t::string_t( const char_type *str, std::size_t size) : pimpl_( 0)
{
    from_c_string( str, size);
}

string_t::string_t( const string_t& str, size_type pos, size_type n) : pimpl_( 0)
{
    init();
    size_type nn = std::min( n, str.length() - pos);
    assign( str.begin() + pos, str.begin() + pos + nn);
}

void string_t::from_c_string( const char *str, std::size_t size)
{
    init();
    pimpl_->str_.reserve( size + 1);
    pimpl_->str_.assign( str, str + size);
    pimpl_->str_.push_back( char_type( 0));
}

void string_t::init( impl *x)
{
    RAMEN_ASSERT( !pimpl_);

    if( x)
        pimpl_ = new impl( *x);
    else
        pimpl_ = new impl();
}

string_t::~string_t()
{
    delete pimpl_;
}

string_t::string_t( const string_t& other) : pimpl_( 0)
{
    RAMEN_ASSERT( other.pimpl_);

    init( other.pimpl_);
}

void string_t::swap( string_t& other)
{
    std::swap( pimpl_, other.pimpl_);
}

string_t& string_t::operator=( const char_type *str)
{
    string_t tmp( str);
    swap( tmp);
    return *this;
}

string_t::size_type string_t::size() const
{
    RAMEN_ASSERT( pimpl_);

    return pimpl_->str_.empty() ? 0 : pimpl_->str_.size() - 1;
}

string_t::size_type string_t::length() const
{
    return size();
}

bool string_t::empty() const
{
    RAMEN_ASSERT( pimpl_);

    return pimpl_->str_.empty();
}

void string_t::reserve( size_type n)
{
    RAMEN_ASSERT( pimpl_);

    pimpl_->str_.reserve( n == 0 ? 0 : n + 1);
}

void string_t::clear()
{
    RAMEN_ASSERT( pimpl_);

    pimpl_->str_.clear();
}

const string_t::char_type *string_t::c_str() const
{
    RAMEN_ASSERT( pimpl_);

    return pimpl_->str_.empty() ? "" : &(pimpl_->str_[0]);
}

void string_t::push_back( char_type c)
{
    RAMEN_ASSERT( pimpl_);

    if( !pimpl_->str_.empty())
        pimpl_->str_.pop_back();

    pimpl_->str_.push_back( c);
    pimpl_->str_.push_back( char_type( 0));
}

string_t::const_iterator string_t::begin() const
{
    RAMEN_ASSERT( pimpl_);

    return pimpl_->str_.begin().get_ptr();
}

string_t::const_iterator string_t::end() const
{
    RAMEN_ASSERT( pimpl_);

    if( !pimpl_->str_.empty())
        return pimpl_->str_.end().get_ptr() - 1;

    return pimpl_->str_.end().get_ptr();
}

string_t::iterator string_t::begin()
{
    RAMEN_ASSERT( pimpl_);

    return pimpl_->str_.begin().get_ptr();
}

string_t::iterator string_t::end()
{
    RAMEN_ASSERT( pimpl_);

    if( !pimpl_->str_.empty())
        return pimpl_->str_.end().get_ptr() - 1;

    return pimpl_->str_.end().get_ptr();
}

string_t& string_t::operator+=( const char_type *str)
{
    append( str, string_length( str));
    return *this;
}

string_t& string_t::operator+=( const string_t& str)
{
    append( str);
    return *this;
}

void string_t::append( const char_type *str, size_type len)
{
    RAMEN_ASSERT( pimpl_);

    if( len != 0)
    {
        if( !pimpl_->str_.empty())
            pimpl_->str_.pop_back();

        pimpl_->str_.insert( pimpl_->str_.end(), str, str + len);
        pimpl_->str_.push_back(0);
    }
}

void string_t::append( const string_t& str)
{
    append( str.c_str(), str.size());
}

string_t::char_type string_t::operator[]( size_type index) const
{
    RAMEN_ASSERT( pimpl_);
    RAMEN_ASSERT( index < size());

    return pimpl_->str_[index];
}

string_t::char_type& string_t::operator[]( size_type index)
{
    RAMEN_ASSERT( pimpl_);
    RAMEN_ASSERT( index < size());

    return pimpl_->str_[index];
}

string_t operator+( const string_t& a, const string_t& b)
{
    string_t result( a);
    result.append( b);
    return result;
}

string_t operator+( const string_t& a, const char *b)
{
    string_t result( a);
    result.append( b, string_length( b));
    return result;
}

bool operator==( const string_t& a, const string_t& b)
{
    return string_compare( a.c_str(), b.c_str()) == 0;
}

bool operator==( const string_t& a, const char *b)
{
    return string_compare( a.c_str(), b) == 0;
}

bool operator==( const char *a, const string_t& b)
{
    return string_compare( a, b.c_str()) == 0;
}

bool operator!=( const string_t& a, const string_t& b)
{
    return !( a == b);
}

bool operator!=( const string_t& a, const char *b)
{
    return !( a == b);
}

bool operator!=( const char *a, const string_t& b)
{
    return !( a == b);
}

bool operator<( const string_t& a, const string_t& b)
{
    return string_compare( a.c_str(), b.c_str()) < 0;
}

bool operator<( const string_t& a, const char *b)
{
    return string_compare( a.c_str(), b) < 0;
}

bool operator<( const char *a, const string_t& b)
{
    return string_compare( a, b.c_str()) < 0;
}

const string_t make_string( const char *a, const char *b, const char *c, const char *d)
{
    RAMEN_ASSERT( a);
    RAMEN_ASSERT( b);

    std::size_t a_len = string_length( a);
    std::size_t b_len = string_length( b);
    std::size_t c_len = ( c != 0) ? string_length( c) : 0;
    std::size_t d_len = ( d != 0) ? string_length( d) : 0;

    string_t str;
    str.reserve( a_len + b_len + c_len + d_len + 1);
    str.append( a, a_len);
    str.append( b, b_len);

    if( c)
        str.append( c, c_len);

    if( d)
        str.append( d, d_len);

    return str;
}

} // core
} // ramen
