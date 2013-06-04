// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/edit_keys_visitors.hpp>

#include<ramen/anim/track.hpp>
#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

#include<ramen/anim/curve_algorithm.hpp>

#include<ramen/ui/anim/anim_curves_view.hpp>
#include<ramen/ui/anim/anim_editor_command.hpp>

namespace ramen
{
namespace ui
{

void delete_selected_keys_visitor::operator()( anim::float_curve_t *c)
{
	c->erase_selected_keyframes();
	c->recalc_tangents_and_coefficients();
}

can_drag_keys_visitor::can_drag_keys_visitor( const Imath::V2f& offset) : offset_( offset) {}
void can_drag_keys_visitor::operator()( const anim::float_curve_t *c)
{
	result = ( offset_.x != 0 || offset_.y != 0);
}

void can_drag_keys_visitor::operator()( const anim::shape_curve2f_t *c)
{
	result = ( offset_.x != 0);
}

drag_keys_visitor::drag_keys_visitor( const Imath::V2f& offset, bool snap_frames) : offset_( offset), snap_frames_( snap_frames) {}
void drag_keys_visitor::operator()( anim::float_curve_t *c)
{
	changed = true;
	
	if( offset_.y != 0.0f)
		anim::move_selected_keyframes_value( *c, offset_.y / c->scale());
	
	if( offset_.x != 0.0f)
		anim::move_selected_keyframes_time( *c, offset_.x, snap_frames_);

	c->recalc_tangents_and_coefficients();
}

void drag_keys_visitor::operator()( anim::shape_curve2f_t *c)
{
	if( offset_.x != 0.0f)
	{
		anim::move_selected_keyframes_time( *c, offset_.x, snap_frames_);
		changed = true;
	}
	else
		changed = false;
}

get_keyframe_value_visitor::get_keyframe_value_visitor( int index) : index_( index), value( 0), can_edit( false) {}
void get_keyframe_value_visitor::operator()( const anim::float_curve_t *c)
{ 
	value = c->relative_to_absolute( (*c)[index_].value());
	can_edit = true;
}

can_insert_keyframe_visitor::can_insert_keyframe_visitor( const anim_curves_view_t& view, const Imath::V2i& p, bool snap) : view_( view), p_( p)
{
	snap_ = snap;
	
	if( snap_)
	{
		Imath::V2f q( view_.screen_to_world( p_));
		q.x = (int) q.x;
		time_ = q.x;
		
		p_ = view_.world_to_screen( q);
	}
	else
		time_ = view_.screen_to_world( p).x;		
}

void can_insert_keyframe_visitor::operator()( const anim::float_curve_t *c)
{
	float val = c->evaluate( time_);
	val = c->relative_to_absolute( val);
	Imath::V2i qi( view_.world_to_screen( Imath::V2f( time_, val)));
	can_insert = view_.inside_pick_distance( p_, qi);
}

insert_keyframe_visitor::insert_keyframe_visitor( const anim_curves_view_t& view, const Imath::V2i& p, bool snap) : view_( view), p_( p)
{
	snap_ = snap;
	
	if( snap_)
	{
		Imath::V2f q( view_.screen_to_world( p_));
		q.x = (int) q.x;
		time_ = q.x;
		
		p_ = view_.world_to_screen( q);
	}
	else
		time_ = view_.screen_to_world( p).x;
	
	key_index = -1;
}

void insert_keyframe_visitor::operator()( anim::float_curve_t *c)
{
	anim::float_curve_t::iterator it( c->insert( time_, true));
	key_index = it - c->begin();
}

void negate_selected_keys_visitor::operator()( anim::float_curve_t *c)
{
	anim::negate_keyframes( *c, true);
}

void sample_selected_keys_visitor::operator()( anim::float_curve_t *c)
{
	anim::sample_keyframes( *c, true);
}

void smooth_selected_keys_visitor::operator()( anim::float_curve_t *c)
{
	if( resample_)
		anim::sample_keyframes( *c, true);

	anim::smooth_keyframes( *c, filter_size_, true);
}

void high_pass_selected_keys_visitor::operator()( anim::float_curve_t *c)
{
	if( resample_)
		anim::sample_keyframes( *c, true);
	
	anim::highpass_keyframes( *c, filter_size_, true);
}

} // namespace
} // namespace
