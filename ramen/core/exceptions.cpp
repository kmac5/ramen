// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/core/exceptions.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace core
{

runtime_error::runtime_error( string_t message) : message_( boost::move( message)) {}

const char *runtime_error::what() const
{
    return message_.c_str();
}

bad_cast::bad_cast( const std::type_info& src_type, const std::type_info& dst_type)
{
    // TODO: do something with this...
    /*
    RAMEN_ASSERT( src_type_ != dst_type_);
    message_ = boost::move( make_string( "bad cast: src_type = ", type_to_string( src_type),
                                                    " dst_type = ", type_to_string( dst_type)));
    */
    message_ = "bad cast";
}

const char *bad_cast::what() const
{
    return message_.c_str();
}

bad_type_cast::bad_type_cast( const type_t& src_type, const type_t& dst_type)
{
    RAMEN_ASSERT( src_type != dst_type);
    message_ = boost::move( make_string( "bad cast: src_type = ", type_to_string( src_type),
                                                  " dst_type = ", type_to_string( dst_type)));
}

const char *bad_type_cast::what() const
{
    return message_.c_str();
}

key_not_found::key_not_found( const name_t& name) : message_( name.c_str()) {}
key_not_found::key_not_found( string_t message) : message_( boost::move( message)) {}

const char *key_not_found::what() const
{
    return message_.c_str();
}

not_implemented::not_implemented() {}

} // core
} // ramen
