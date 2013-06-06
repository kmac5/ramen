// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>

#include<ramen/manipulators/point2_manipulator.hpp>
#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/GL/gl.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/viewer_strategy.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

point2_manipulator_t::point2_manipulator_t( float2_param_t *param) : manipulator_t()
{
	RAMEN_ASSERT( param);
	param_ = param;
	selected_ = false;
}

void point2_manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
    Imath::V2f p = get_absolute_value<Imath::V2f>( *param_);
	p.x *= event.aspect_ratio;
	gl_line_width( default_line_width());

	float size = 10 / event.pixel_scale;
	Imath::Color3c color = selected_ ? selected_color() : default_color();
	manipulators::draw_cross( p, size, size, color, event.pixel_scale);
}

bool point2_manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{
	selected_ = false;
    Imath::V2f p = get_absolute_value<Imath::V2f>( *param_);
	p.x = p.x * event.aspect_ratio;

	if( manipulators::inside_pick_distance( event.wpos, p, event.pixel_scale))
		selected_ = true;
	
	event.view->update();
	return selected_;
}

void point2_manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	RAMEN_ASSERT( selected_);
	
	if( event.first_drag)
		param_->param_set()->begin_edit();

	Imath::V2f offset( event.wpos - event.last_wpos);
	offset.x = offset.x / event.aspect_ratio;
	
	Imath::V2f q = get_absolute_value<Imath::V2f>( *param_);
	param_->set_absolute_value( q + offset);
	param_->update_widgets();
	app().ui()->update_anim_editors();
	
	if( param_->track_mouse())
		param_->param_set()->notify_parent();

	event.view->update();
}

void point2_manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	selected_ = false;
	
	if( event.dragged)
	{
		param_->param_set()->end_edit( !param_->track_mouse());
		app().ui()->update();
	}

	event.view->update();
}

} // namespace
