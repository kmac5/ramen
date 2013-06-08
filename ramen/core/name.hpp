// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NAME_HPP
#define RAMEN_NAME_HPP

#include<ramen/core/name_fwd.hpp>

#include<iostream>

namespace ramen
{
namespace core
{

/*!
\ingroup core
\brief Unique string class
*/
class RAMEN_API name_t
{
public:

    name_t();
    explicit name_t( const char *str);

    name_t& operator=( const name_t& other);

    const char *c_str() const { return data_;}

    bool empty() const;

    bool operator==( const name_t& other) const { return data_ == other.data_;}
    bool operator!=( const name_t& other) const { return data_ != other.data_;}
    bool operator<( const name_t& other) const	{ return data_ < other.data_;}

    void swap( name_t& other)
    {
        const char *tmp = data_;
        data_ = other.data_;
        other.data_ = tmp;
    }

private:

    void init( const char *str);

    const char *data_;
};

inline void swap( name_t& x, name_t& y)
{
    x.swap( y);
}

inline std::ostream& operator<<( std::ostream& os, const name_t& name)
{
    return os << name.c_str();
}

} // core
} // ramen

#endif
