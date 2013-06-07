// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/transform2_param.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>

#include<ramen/math/constants.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{

transform2_param_t::transform2_param_t( const std::string& name, const std::string& id) : composite_param_t( name)
{
    set_id( id);

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Center"));
    p->set_id( id + std::string( "_center"));
    p->set_default_value( Imath::V2f( 0.5, 0.5));
    p->set_numeric_type( numeric_param_t::relative_xy);
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

transform2_param_t::transform2_param_t( const transform2_param_t& other) : composite_param_t( other) {}

const param_t& transform2_param_t::center_param() const     { return params()[0];}
param_t& transform2_param_t::center_param()                 { return params()[0];}

void transform2_param_t::move_center( const Imath::V2f& c, change_reason reason)
{
	float2_param_t *p = dynamic_cast<float2_param_t*>( &center_param());
	RAMEN_ASSERT( p);
	Imath::V2f orig = get_absolute_value<Imath::V2f>( *p);
	p->set_absolute_value( c + orig);
	p->emit_param_changed( reason);
	p->update_widgets();
}

const param_t& transform2_param_t::translate_param() const  { return params()[1];}
param_t& transform2_param_t::translate_param()              { return params()[1];}

void transform2_param_t::translate( const Imath::V2f& t, change_reason reason)
{
	float2_param_t *p = dynamic_cast<float2_param_t*>( &translate_param());
	RAMEN_ASSERT( p);
	Imath::V2f orig = get_value<Imath::V2f>( *p);
	p->set_value( t + orig);
	p->emit_param_changed( reason);
	p->update_widgets();
}

const param_t& transform2_param_t::scale_param() const      { return params()[2];}
param_t& transform2_param_t::scale_param()                  { return params()[2];}

const param_t& transform2_param_t::rotate_param() const     { return params()[3];}
param_t& transform2_param_t::rotate_param()                 { return params()[3];}

transform2_param_t::matrix_type transform2_param_t::matrix_at_frame( float frame, float aspect, int subsample) const
{
    Imath::V2f c = get_absolute_value_at_frame<Imath::V2f>( center_param(), frame) / subsample;
    Imath::V2f t = get_value_at_frame<Imath::V2f>( translate_param(), frame) / subsample;
    float angle  = get_value_at_frame<float>( rotate_param(), frame);
    Imath::V2f s = get_value_at_frame<Imath::V2f>( scale_param(), frame);

    transform2_param_t::matrix_type m = matrix_type().setTranslation( -c) *
										matrix_type().setScale( Imath::V2d( aspect, 1.0)) *
										matrix_type().setScale( s) *
										matrix_type().setRotation( angle * math::constants<double>::deg2rad()) *
										matrix_type().setTranslation( t) *
										matrix_type().setScale( Imath::V2d( 1.0 / aspect, 1.0)) *
										matrix_type().setTranslation( c);

    return m;
}

transform2_param_t::matrix_type transform2_param_t::xform_blur_matrix_at_frame( float frame, float t, float aspect, int subsample) const
{
	RAMEN_ASSERT( aspect > 0.0f);

    Imath::V2f c = get_absolute_value_at_frame<Imath::V2f>( center_param(), frame) / subsample;
    Imath::V2f d = get_value_at_frame<Imath::V2f>( translate_param(), frame) / subsample;
    float angle  = get_value_at_frame<float>( rotate_param(), frame);
    Imath::V2f s = get_value_at_frame<Imath::V2f>( scale_param(), frame);

    s = Imath::lerp( Imath::V2f( 1, 1), s, t);
    d = Imath::lerp( Imath::V2f( 0, 0), d, t);
    angle = Imath::lerp( 0.0f, angle, t);

    transform2_param_t::matrix_type m = transform2_param_t::matrix_type().setScale( Imath::V2d( aspect, 1.0)) *
										transform2_param_t::matrix_type().setTranslation( -c) *
										transform2_param_t::matrix_type().setScale( s) *
										transform2_param_t::matrix_type().setRotation( angle * math::constants<double>::deg2rad()) *
										transform2_param_t::matrix_type().setTranslation( c + d) *
										transform2_param_t::matrix_type().setScale( Imath::V2d( 1.0 / aspect, 1.0));

    return m;
}

/*
void transform2_param_t::apply_track( float start_frame, float end_frame, const image::tracker_node_t *tracker, 
									  image::apply_track_mode mode, image::apply_track_use use)
{
	
    Imath::V2f center = get_absolute_value_at_frame<Imath::V2f>( center_param(), start_frame);
	float2_param_t *trans = dynamic_cast<float2_param_t*>( &translate_param());
	float_param_t *rot = dynamic_cast<float_param_t*>( &rotate_param());
	float2_param_t *scale = dynamic_cast<float2_param_t*>( &scale_param());
	
	param_set()->begin_edit();
	tracker->apply_track( start_frame, end_frame, mode, use, center, trans, rot, scale);
	param_set()->end_edit( true);
	app().ui()->update_anim_editors();
}
*/

} // namespace
