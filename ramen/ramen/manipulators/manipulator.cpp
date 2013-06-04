// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/manipulator.hpp>

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/ui/palette.hpp>

namespace ramen
{

manipulator_t::manipulator_t()
{ 
	set_parent( 0);
	set_visible( true);
	set_active( true);
}

void manipulator_t::draw_overlay( const ui::paint_event_t& event) const
{
	RAMEN_ASSERT( event.view);
	
	if( visible())
		do_draw_overlay( event);
}

bool manipulator_t::key_press_event( const ui::key_press_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		return do_key_press_event( event);

	return false;
}

void manipulator_t::key_release_event( const ui::key_release_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		do_key_release_event( event);
}

void manipulator_t::mouse_enter_event( const ui::mouse_enter_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		do_mouse_enter_event( event);
}

void manipulator_t::mouse_leave_event( const ui::mouse_leave_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		do_mouse_leave_event( event);
}

bool manipulator_t::mouse_press_event( const ui::mouse_press_event_t& event)
{ 
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		return do_mouse_press_event( event);
	
	return false;
}

void manipulator_t::mouse_move_event( const ui::mouse_move_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		do_mouse_move_event( event);
}

void manipulator_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		do_mouse_drag_event( event);
}

void manipulator_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
	RAMEN_ASSERT( event.view);
	
	if( active() && visible())
		do_mouse_release_event( event);
}

void manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const {}

bool manipulator_t::do_key_press_event( const ui::key_press_event_t& event) { return false;}
void manipulator_t::do_key_release_event( const ui::key_release_event_t& event) {}

void manipulator_t::do_mouse_enter_event( const ui::mouse_enter_event_t& event) {}
void manipulator_t::do_mouse_leave_event( const ui::mouse_leave_event_t& event) {}

bool manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)     { return false;}
void manipulator_t::do_mouse_move_event( const ui::mouse_move_event_t& event)       {}
void manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)       {}
void manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event) {}

float manipulator_t::default_control_point_size()	{ return 5.0f;}
float manipulator_t::default_line_width() 			{ return 1.5f;}

const Imath::Color3c& manipulator_t::default_color()
{
	return ui::palette_t::instance().color( "manipulator");
}

const Imath::Color3c& manipulator_t::selected_color()
{
	return ui::palette_t::instance().color( "active_manipulator");
}

} // namespace
