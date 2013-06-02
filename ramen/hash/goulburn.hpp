// License: The Goulburn Hashing Function, Mayur Patel.

#ifndef RAMEN_HASH_GOULBURN_HPP
#define RAMEN_HASH_GOULBURN_HPP

#include<boost/cstdint.hpp>

namespace ramen
{
namespace hash
{

class goulburn_t
{
public:

    typedef boost::uint32_t value_type;

    goulburn_t() {}

    value_type operator()( value_type x) const
    {
        return hash( reinterpret_cast<unsigned char*>( &x), sizeof( value_type));
    }

    value_type operator()( value_type x, value_type y) const
    {
        return hash( reinterpret_cast<unsigned char*>( &y), sizeof( value_type),
                    hash( reinterpret_cast<unsigned char*>( &x), sizeof( value_type)));
    }

    float operator()( float x) const
    {
        return to_float_01( hash( reinterpret_cast<unsigned char*>( &x), sizeof( float)));
    }

    float operator()( float x, float y) const
    {
        return to_float_01( hash( reinterpret_cast<unsigned char*>( &y), sizeof( float),
                            hash( reinterpret_cast<unsigned char*>( &x), sizeof( float))));
    }

    float operator()( float x, float y, float z) const
    {
        return to_float_01( hash( reinterpret_cast<unsigned char*>( &z), sizeof( float),
                            hash( reinterpret_cast<unsigned char*>( &y), sizeof( float),
                            hash( reinterpret_cast<unsigned char*>( &x), sizeof( float)))));
    }

    float operator()( float x, float y, float z, float w) const
    {
        return to_float_01( hash( reinterpret_cast<unsigned char*>( &w), sizeof( float),
                            hash( reinterpret_cast<unsigned char*>( &z), sizeof( float),
                            hash( reinterpret_cast<unsigned char*>( &y), sizeof( float),
                            hash( reinterpret_cast<unsigned char*>( &x), sizeof( float))))));
    }

    float gauss_rand( float v) const;

private:

    value_type hash( const unsigned char *cp, unsigned len, value_type last_value = 0) const
    {
        value_type h = last_value;

        for( unsigned int i = 0; i < len; ++i )
        {
            h += g_table0[cp[i]];
            h ^= ( h << 3) ^ ( h >> 29);
            h += g_table1[h >> 25];
            h ^= ( h << 14) ^ ( h >> 18);
            h += 1783936964UL;
        }

      return h;
    }

    float to_float_01( value_type x) const
    {
        union { float f; value_type i;} u;
        u.i = 0x3f800000 | ( x & 0x7fffff);
        return u.f - 1;
    }

    static const value_type max_value;
    static const value_type g_table0[256];
    static const value_type g_table1[128];
};

} // math
} // ramen

#endif
