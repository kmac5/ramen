// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/hash/generator.hpp>

#include<ramen/assert.hpp>

#include<iomanip>

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
        adobe::md5_t md5;
        md5.update( reinterpret_cast<void*>( const_cast<char*>( ss_.str().c_str())), ss_.str().size());
        digest_ = md5.final();
    }

    return digest_.get();
}

std::string generator_t::digest_as_string() const
{
    std::stringstream s;

    for( int i = 0; i < sizeof( digest_type); ++i)
        s << std::setfill( '0') << std::setw( 2) << std::hex << (int) digest()[i];

    return s.str();
}

generator_t::digest_type digest_from_string( const std::string& s)
{
    RAMEN_ASSERT( s.size() == sizeof( generator_t::digest_type) * 2);

    generator_t::digest_type digest;
    char tmp[6] = { '0', 'x', 0, 0, 0, 0};
    char *tmp2;

    for( int i = 0, j = 0; j < sizeof( digest);)
    {
        tmp[2] = s[i];
        tmp[3] = s[i+1];
        digest[j] = strtol( tmp, &tmp2, 16);

        i += 2;
        ++j;
    }

    return digest;
}

} // util
} // ramen
