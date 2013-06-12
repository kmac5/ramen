// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/core/types.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace core
{

const char *type_to_string( type_t t)
{
    #define RAMEN_CORE_TYPE2STRING_CASE( type_enum_k, type_name)\
        case type_enum_k:\
            return type_name;

    switch( t)
    {
        RAMEN_CORE_TYPE2STRING_CASE( bool_k, "bool")
        RAMEN_CORE_TYPE2STRING_CASE( half_k ,"half")
        RAMEN_CORE_TYPE2STRING_CASE( float_k, "float")
        RAMEN_CORE_TYPE2STRING_CASE( double_k, "double")

        RAMEN_CORE_TYPE2STRING_CASE( int8_k, "int8_t")
        RAMEN_CORE_TYPE2STRING_CASE( uint8_k, "uint8_t")
        RAMEN_CORE_TYPE2STRING_CASE( int16_k, "int16_t")
        RAMEN_CORE_TYPE2STRING_CASE( uint16_k, "uint16_t")
        RAMEN_CORE_TYPE2STRING_CASE( int32_k, "int32_t")
        RAMEN_CORE_TYPE2STRING_CASE( uint32_k, "uint32_t")

        RAMEN_CORE_TYPE2STRING_CASE( point2i_k, "point2i_t")
        RAMEN_CORE_TYPE2STRING_CASE( point2h_k, "point2h_t")
        RAMEN_CORE_TYPE2STRING_CASE( point2f_k, "point2f_t")

        RAMEN_CORE_TYPE2STRING_CASE( point3h_k, "point3h_t")
        RAMEN_CORE_TYPE2STRING_CASE( point3f_k, "point3f_t")

        RAMEN_CORE_TYPE2STRING_CASE( vector2i_k, "vector2i_t")
        RAMEN_CORE_TYPE2STRING_CASE( vector2h_k, "vector2h_t")
        RAMEN_CORE_TYPE2STRING_CASE( vector2f_k, "vector2f_t")

        RAMEN_CORE_TYPE2STRING_CASE( vector3h_k, "vector3h_t")
        RAMEN_CORE_TYPE2STRING_CASE( vector3f_k, "vector3f_t")

        RAMEN_CORE_TYPE2STRING_CASE( normalh_k, "normalh_t")
        RAMEN_CORE_TYPE2STRING_CASE( normalf_k, "normalf_t")

        RAMEN_CORE_TYPE2STRING_CASE( hpoint2f_k, "hpoint2f_t")

        RAMEN_CORE_TYPE2STRING_CASE( hpoint3f_k, "hpoint3f_t")

        RAMEN_CORE_TYPE2STRING_CASE( color3f_k, "color3f_t")
        RAMEN_CORE_TYPE2STRING_CASE( color4f_k, "color4f_t")

        RAMEN_CORE_TYPE2STRING_CASE( box2i_k, "box2i_t")

        RAMEN_CORE_TYPE2STRING_CASE( string_k, "string_t")

        default:
            RAMEN_ASSERT( false && "Unknown type in type_to_string");
            return 0;
    }

    #undef RAMEN_CORE_TYPE2STRING_CASE

    // for dumb compilers...
    return 0;
}

} // core
} // ramen

