// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UI_TOOL_HPP
#define	RAMEN_UI_TOOL_HPP

#include<OpenEXR/ImathBox.h>

#include<ramen/GL/gl.hpp>

#include<ramen/ui/events.hpp>
#include<ramen/ui/viewer/viewer_strategy.hpp>

namespace ramen
{
namespace ui
{

class tool_t
{
public:

    tool_t();
    virtual ~tool_t();

	virtual void begin_active();
	virtual void end_active();
	
    virtual void draw_overlay( const paint_event_t& event) const;
	
    virtual void key_press_event( const key_press_event_t& event);
    virtual void key_release_event( const key_release_event_t& event);

    virtual void mouse_enter_event( const mouse_enter_event_t& event);
    virtual void mouse_leave_event( const mouse_leave_event_t& event);

    virtual void mouse_press_event( const mouse_press_event_t& event);
    virtual void mouse_move_event( const mouse_move_event_t& event);
    virtual void mouse_drag_event( const mouse_drag_event_t& event);
    virtual void mouse_release_event( const mouse_release_event_t& event);

protected:

	bool inside_pick_distance( const Imath::V2f& p, const mouse_event_t& event) const;
	bool inside_pick_distance( const Imath::Box2f& b, const mouse_event_t& event) const;

	bool inside_pick_distance( const Imath::V2f& p, const Imath::V2f& q, float pixel_scale) const;
	bool inside_pick_distance( const Imath::Box2f& b, const Imath::V2f& p, float pixel_scale) const;
};

} // namespace
} // namespace

#endif
