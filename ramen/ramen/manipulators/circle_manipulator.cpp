// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/assert.hpp>

#include<ramen/manipulators/circle_manipulator.hpp>
#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/GL/gl.hpp>

#include<ramen/app/application.hpp>

#include<ramen/nodes/image_node.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/viewer_strategy.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

enum
{
	no_pick = 0,
	center_picked,
	circle_picked
};
	
circle_manipulator_t::circle_manipulator_t( float_param_t *radius, float2_param_t *center) : manipulator_t()
{
	RAMEN_ASSERT( radius);
	
	center_ = center;
	radius_ = radius;
	selected_ = no_pick;
}

void circle_manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
	const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
	RAMEN_ASSERT( node);
	
	Imath::V2f p( get_center());

	p.x *= event.aspect_ratio;
	gl_line_width( default_line_width());

	float size = 10 / event.pixel_scale;
	Imath::Color3c color( default_color());

	if( selected_ != no_pick)
		color = selected_color();

	manipulators::draw_cross( p, size, size, color, event.pixel_scale);
	
	float radius = get_absolute_value<float>( *radius_);
	manipulators::draw_ellipse( p, radius * event.aspect_ratio / node->aspect_ratio(),
									radius, color, event.pixel_scale);
}

bool circle_manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{
	const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
	RAMEN_ASSERT( node);
	
	selected_ = no_pick;
	
    Imath::V2f p = get_center();
	p.x *= event.aspect_ratio;

	Imath::V2f q( event.wpos - p);
	q.x = q.x * node->aspect_ratio() / event.aspect_ratio;
	
	float radius = get_absolute_value<float>( *radius_);
	float d = q.length();

	if( Imath::abs( d - radius) <= manipulators::pick_distance())
		selected_ = circle_picked;
	else
	{
		if( center_ && manipulators::inside_pick_distance( event.wpos, p, event.pixel_scale))
			selected_ = center_picked;
	}
			
	event.view->update();

	return selected_ != no_pick;
}

void circle_manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	RAMEN_ASSERT( selected_ != no_pick);

	if( event.first_drag)
		radius_->param_set()->begin_edit();

	Imath::V2f c = get_center();
	
	if( selected_ == circle_picked)
	{
		c.x *= event.aspect_ratio;
		radius_->set_absolute_value( ( event.wpos - c).length());
		radius_->update_widgets();
	}
	else
	{
		RAMEN_ASSERT( center_);
		Imath::V2f offset( event.wpos - event.last_wpos);
		offset.x = offset.x / event.aspect_ratio;
	
		center_->set_absolute_value( c + offset);
		center_->update_widgets();
	}

	app().ui()->update_anim_editors();

	if( radius_->track_mouse())
		radius_->param_set()->notify_parent();
	
	event.view->update();
}

void circle_manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	selected_ = no_pick;
	
	if( event.dragged)
	{
		radius_->param_set()->end_edit( !radius_->track_mouse());
		app().ui()->update();
	}

	event.view->update();
}

Imath::V2f circle_manipulator_t::get_center() const
{
	if( center_)
		return get_absolute_value<Imath::V2f>( *center_);

	const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
	RAMEN_ASSERT( node);
		
	return Imath::V2f( ( node->format().max.x + node->format().min.x) * 0.5f,
						( node->format().max.y + node->format().min.y) * 0.5f);
}

} // namespace
