// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/integer_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/util.hpp>

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

integer_param_t::integer_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : double_param_t( name, descriptor, instance)
{
}

OfxStatus integer_param_t::copy( const OFX::Host::Param::Instance& instance, OfxTime offset)
{
	return double_param_t::copy( instance, offset);
}

OfxStatus integer_param_t::copy( const OFX::Host::Param::Instance& instance, OfxTime offset, OfxRangeD range)
{
	return double_param_t::copy( instance, offset, range);
}

OfxStatus integer_param_t::get( int& v)
{
	double val;
	OfxStatus stat = double_param_t::get( val);
	v = val;
	return stat;
}

OfxStatus integer_param_t::get( OfxTime time, int& v)
{
	double val;
	OfxStatus stat = double_param_t::get( time, val);
	v = val;
	return stat;
}

OfxStatus integer_param_t::set( int v) { return double_param_t::set( v);}

OfxStatus integer_param_t::set( OfxTime time, int v)
{
	return double_param_t::set( time, v);	
}

OfxStatus integer_param_t::derive( OfxTime time, int& v)
{
	double val;
	OfxStatus stat = double_param_t::derive( time, val);
	v = val;
	return stat;
}

OfxStatus integer_param_t::integrate( OfxTime time1, OfxTime time2, int& v)
{
	double val;
	OfxStatus stat = double_param_t::integrate( time1, time2, val);
	v = val;
	return stat;
}

} // namespace
} // namespace
