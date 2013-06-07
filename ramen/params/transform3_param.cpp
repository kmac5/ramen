// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/transform3_param.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/assert.hpp>

#include<ramen/math/constants.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/float3_param.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

transform3_param_t::transform3_param_t( const std::string& name, const std::string& id, bool image_mode) : composite_param_t( name)
{
	image_mode_ = image_mode;
    set_id( id);

	if( image_mode)
	{
	    std::auto_ptr<float2_param_t> p( new float2_param_t( "Center"));
	    p->set_id( id + std::string( "_center_xy"));
	    p->set_default_value( Imath::V2f( 0.5, 0.5));
	    p->set_numeric_type( numeric_param_t::relative_xy);
	    add_param( p);
		
		std::auto_ptr<float_param_t> q( new float_param_t( "Center Z"));
	    q->set_id( id + std::string( "_center_z"));
	    q->set_default_value( 0);
	    add_param( q);
	}
	else
	{
	    std::auto_ptr<float3_param_t> p( new float3_param_t( "Center"));
	    p->set_id( id + std::string( "_center"));
	    p->set_default_value( Imath::V3f( 0, 0, 0));
	    add_param( p);
	}

    std::auto_ptr<float3_param_t> p( new float3_param_t( "Pos"));
    p->set_id( id + std::string( "_pos"));
    p->set_default_value( Imath::V3f( 0, 0, 0));
    add_param( p);

    p.reset( new float3_param_t( "Scale"));
    p->set_id( id + std::string( "_scale"));
    p->set_default_value( Imath::V3f( 1, 1, 1));
    p->set_step( 0.05);
    p->set_proportional( true);
    add_param( p);

    p.reset( new float3_param_t( "Rotation"));
    p->set_id( id + std::string( "_rot"));
    p->set_default_value( Imath::V3f( 0, 0, 0));
    p->set_step( 0.5);
    add_param( p);
	
	set_create_track( false);
}

transform3_param_t::transform3_param_t( const transform3_param_t& other) : composite_param_t( other) {}

const param_t& transform3_param_t::center_param() const
{ 
	RAMEN_ASSERT( !image_mode());
	return params()[0];
}

param_t& transform3_param_t::center_param()
{ 
	RAMEN_ASSERT( !image_mode());
	return params()[0];
}

const param_t& transform3_param_t::center_xy_param() const
{ 
	RAMEN_ASSERT( image_mode());
	return params()[0];
}

param_t& transform3_param_t::center_xy_param()
{ 
	RAMEN_ASSERT( image_mode());
	return params()[0];
}

const param_t& transform3_param_t::center_z_param() const
{ 
	RAMEN_ASSERT( image_mode());
	return params()[1];
}

param_t& transform3_param_t::center_z_param()
{ 
	RAMEN_ASSERT( image_mode());
	return params()[1];
}

const param_t& transform3_param_t::translate_param() const
{ 
	if( image_mode())
		return params()[2];
	
	return params()[1];
}

param_t& transform3_param_t::translate_param()
{ 
	if( image_mode())
		return params()[2];
	
	return params()[1];
}

const param_t& transform3_param_t::scale_param() const
{ 
	if( image_mode())
		return params()[3];
	
	return params()[2];
}
		
param_t& transform3_param_t::scale_param()
{ 
	if( image_mode())
		return params()[3];
	
	return params()[2];
}

const param_t& transform3_param_t::rotate_param() const
{ 
	if( image_mode())
		return params()[4];
	
	return params()[3];
}

param_t& transform3_param_t::rotate_param()
{ 
	if( image_mode())
		return params()[4];
	
	return params()[3];
}
		
Imath::V3f transform3_param_t::get_center() const
{
	if( image_mode())
	{
		Imath::V2f xy = get_absolute_value<Imath::V2f>( center_xy_param());
		float z = get_value<float>( center_z_param());
		return Imath::V3f( xy.x, xy.y, z);
	}
	else
		return get_value<Imath::V3f>( center_param());
}

Imath::V3f transform3_param_t::get_center_at_frame( float frame) const
{
	if( image_mode())
	{
		Imath::V2f xy = get_absolute_value_at_frame<Imath::V2f>( center_xy_param(), frame);
		float z = get_value_at_frame<float>( center_z_param(), frame);
		return Imath::V3f( xy.x, xy.y, z);
	}
	else
		return get_value_at_frame<Imath::V3f>( center_param(), frame);
}

transform3_param_t::matrix_type transform3_param_t::matrix_at_frame( float frame, float aspect, int subsample) const
{
	Imath::V3f c = get_center_at_frame( frame) / subsample;
    Imath::V3f t = get_value_at_frame<Imath::V3f>( translate_param(), frame) / subsample;
    Imath::V3f s = get_value_at_frame<Imath::V3f>( scale_param(), frame);
    Imath::V3f r = get_value_at_frame<Imath::V3f>( rotate_param(), frame) * math::constants<float>::deg2rad();

	transform3_param_t::matrix_type m = matrix_type().setTranslation( -c) *
										matrix_type().setScale( Imath::V3f( aspect, 1.0f, 1.0f)) *
										matrix_type().setScale( s) *
										matrix_type().setEulerAngles( r) *
										matrix_type().setTranslation( t) *
										matrix_type().setScale( Imath::V3f( 1.0f / aspect, 1.0f, 1.0f)) *
										matrix_type().setTranslation( c);

    return m;
}

} // namespace
