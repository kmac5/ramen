// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/hash/generator.hpp>

#include<ramen/assert.hpp>

#include<iomanip>

#include<ramen/hash/MurmurHash3.h>

namespace ramen
{
namespace hash
{

generator_t::generator_t() { set_empty( true);}

void generator_t::reset()
{
    digest_.reset();
    ss_.str( std::string());
    set_empty( true);
}

std::string generator_t::str() const { return ss_.str();}

const generator_t::digest_type& generator_t::digest() const
{
    if( !digest_)
    {
        digest_ = digest_type();
        MurmurHash3_x64_128( reinterpret_cast<void*>( const_cast<char*>( ss_.str().c_str())),
                             ss_.str().size(),
                             0, // seed
                             reinterpret_cast<void*>( const_cast<digest_type::iterator>( digest().begin())));
    }

    return digest_.get();
}

std::string generator_t::digest_as_string() const
{
    const generator_t::digest_type& d( digest());

    std::stringstream s;
    for( int i = 0; i < d.size(); ++i)
        s << std::setfill( '0') << std::setw( 2) << std::hex << d[i];

    /*
    const char *ptr = reinterpret_cast<const char*>( digest().begin());
    for( int i = 0; i < sizeof( digest_type); ++i)
        s << std::setfill( '0') << std::setw( 2) << std::hex << static_cast<int>( ptr[i]);
    */

    return s.str();
}

} // hash
} // ramen
