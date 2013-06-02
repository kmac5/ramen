// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ui/tool.hpp>

namespace ramen
{
namespace ui
{

tool_t::tool_t() {}
tool_t::~tool_t() {}

void tool_t::begin_active() {}
void tool_t::end_active() {}

void tool_t::draw_overlay( const paint_event_t& event) const {}

void tool_t::key_press_event( const key_press_event_t& event) {}
void tool_t::key_release_event( const key_release_event_t& event) {}

void tool_t::mouse_enter_event( const mouse_enter_event_t& event) {}

void tool_t::mouse_leave_event( const mouse_leave_event_t& event) {}
void tool_t::mouse_press_event( const mouse_press_event_t& event) {}
void tool_t::mouse_move_event( const mouse_move_event_t& event) {}
void tool_t::mouse_drag_event( const mouse_drag_event_t& event) {}
void tool_t::mouse_release_event( const mouse_release_event_t& event) {}

bool tool_t::inside_pick_distance( const Imath::V2f& p, const mouse_event_t& event) const
{
	Imath::V2f q( p);
	q.x /= event.aspect_ratio;
	return inside_pick_distance( q, event.wpos, event.pixel_scale);
}

bool tool_t::inside_pick_distance( const Imath::Box2f& b, const mouse_event_t& event) const
{
	if( b.isEmpty())
		return false;

	float dtol = 5 * event.pixel_scale;
	
	Imath::Box2f bb( b);
	bb.min.x  = ( bb.min.x / event.aspect_ratio) - dtol;
	bb.min.y -= dtol;
	bb.max.x  = ( bb.max.x / event.aspect_ratio) + dtol;
	bb.max.y += dtol;
	return bb.intersects( event.wpos);
}

bool tool_t::inside_pick_distance( const Imath::V2f& p, const Imath::V2f& q, float pixel_scale) const
{
	float d = ( p - q).length() * pixel_scale;
	return d <= 5;	
}

bool tool_t::inside_pick_distance( const Imath::Box2f& b, const Imath::V2f& p, float pixel_scale) const
{
	float dtol = 5 * pixel_scale;
	Imath::Box2f bb( b);
	bb.min.x -= dtol;
	bb.min.y -= dtol;
	bb.max.x += dtol;
	bb.max.y += dtol;
	return bb.intersects( p);
}

} // namespace
} // namespace
