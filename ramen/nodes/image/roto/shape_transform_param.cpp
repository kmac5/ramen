// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/shape_transform_param.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>

#include<ramen/math/constants.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{

shape_transform_param_t::shape_transform_param_t( const std::string& name, const std::string& id) : composite_param_t( name)
{
    set_id( id);

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Center"));
    p->set_id( id + std::string( "_center"));
    p->set_default_value( Imath::V2f( 0, 0));
    add_param( p);

    p.reset( new float2_param_t( "Pos"));
    p->set_id( id + std::string( "_pos"));
    p->set_default_value( Imath::V2f( 0, 0));
    add_param( p);

    p.reset( new float2_param_t( "Scale"));
    p->set_id( id + std::string( "_scale"));
    p->set_default_value( Imath::V2f( 1, 1));
    p->set_step( 0.05);
    p->set_proportional( true);
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Rotation"));
    q->set_id( id + std::string( "_rot"));
    q->set_default_value( 0);
    q->set_step( 0.5);
    add_param( q);
	
	set_create_track( false);
}

shape_transform_param_t::shape_transform_param_t( const shape_transform_param_t& other) : composite_param_t( other) {}

const param_t& shape_transform_param_t::center_param() const
{
    return params()[0];
}

void shape_transform_param_t::set_center( const Imath::V2f& c, change_reason reason)
{
	float2_param_t *p = dynamic_cast<float2_param_t*>( &params()[0]);
	RAMEN_ASSERT( p);	
	p->set_value( c);
	p->emit_param_changed( reason);
	p->update_widgets();
}

const param_t& shape_transform_param_t::translate_param() const		{ return params()[1];}
param_t& shape_transform_param_t::translate_param()					{ return params()[1];}

void shape_transform_param_t::set_translate( const Imath::V2f& t, change_reason reason)
{
	float2_param_t *p = dynamic_cast<float2_param_t*>( &params()[1]);
	RAMEN_ASSERT( p);
	p->set_value( t);
	p->emit_param_changed( reason);
	p->update_widgets();
}

const param_t& shape_transform_param_t::scale_param() const { return params()[2];}
param_t& shape_transform_param_t::scale_param()				{ return params()[2];}

const param_t& shape_transform_param_t::rotate_param() const	{ return params()[3];}
param_t& shape_transform_param_t::rotate_param()				{ return params()[3];}

Imath::M33f shape_transform_param_t::matrix_at_frame( float frame) const
{
    Imath::V2f c = get_value_at_frame<Imath::V2f>( center_param(), frame);
    Imath::V2f t = get_value_at_frame<Imath::V2f>( translate_param(), frame);
    float angle  = get_value_at_frame<float>( rotate_param(), frame);
    Imath::V2f s = get_value_at_frame<Imath::V2f>( scale_param(), frame);

    Imath::M33f m = Imath::M33f().setTranslation( -c) *
                    Imath::M33f().setScale( s) *
                    Imath::M33f().setRotation( angle * math::constants<float>::deg2rad()) *
                    Imath::M33f().setTranslation( t) *
                    Imath::M33f().setTranslation( c);

    return m;
}

Imath::M33f shape_transform_param_t::matrix() const
{
    Imath::V2f c = get_value<Imath::V2f>( center_param());
    Imath::V2f t = get_value<Imath::V2f>( translate_param());
    float angle  = get_value<float>( rotate_param());
    Imath::V2f s = get_value<Imath::V2f>( scale_param());

    Imath::M33f m = Imath::M33f().setTranslation( -c) *
                    Imath::M33f().setScale( s) *
                    Imath::M33f().setRotation( angle * math::constants<float>::deg2rad()) *
                    Imath::M33f().setTranslation( t) *
                    Imath::M33f().setTranslation( c);

    return m;
}

/*
void shape_transform_param_t::apply_track( float start_frame, float end_frame, const image::tracker_node_t *tracker, 
										   image::apply_track_mode mode, image::apply_track_use use)
{
    Imath::V2f center = get_absolute_value_at_frame<Imath::V2f>( center_param(), start_frame);	
	
	float2_param_t *trans = dynamic_cast<float2_param_t*>( &translate_param());
	float_param_t *rot = dynamic_cast<float_param_t*>( &rotate_param());
	float2_param_t *scale = dynamic_cast<float2_param_t*>( &scale_param());
	
	param_set()->begin_edit();	
	tracker->apply_track( start_frame, end_frame, mode, use, center, trans, rot, scale);
	param_set()->end_edit( true);

	parameterised()->update_widgets();
	app().ui()->update_anim_editors();
}
*/

} // ramen
