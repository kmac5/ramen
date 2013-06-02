// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_INTEGER_PARAM_HPP
#define RAMEN_OFX_INTEGER_PARAM_HPP

#include<ramen/ofx/params/double_param.hpp>

namespace ramen
{
namespace ofx
{

class image_effect_t;

class integer_param_t : public double_param_t
{
public:

    integer_param_t( const std::string& name, OFX::Host::Param::Descriptor& descriptor, OFX::Host::Param::SetInstance* instance = 0);

    OfxStatus copy( const OFX::Host::Param::Instance &instance, OfxTime offset);
    OfxStatus copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range);

    OfxStatus get( int& v);
    OfxStatus get( OfxTime time, int& v);
    OfxStatus set( int v);
    OfxStatus set( OfxTime time, int v);
    OfxStatus derive( OfxTime time, int& v);
    OfxStatus integrate( OfxTime time1, OfxTime time2, int& v);
};

} // namespace
} // namespace

#endif
