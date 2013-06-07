// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_MANIPULATOR_HPP
#define RAMEN_ROTO_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathColor.h>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/bezier/curve.hpp>

#include<ramen/nodes/image/roto/roto_node_fwd.hpp>
#include<ramen/nodes/image/roto/shape_fwd.hpp>

namespace ramen
{
namespace roto
{

class triple_t;	
	
class manipulator_t : public ramen::manipulator_t
{
public:

    manipulator_t();

private:
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;

    virtual bool do_key_press_event( const ui::key_press_event_t& event);
    virtual void do_key_release_event( const ui::key_release_event_t& event);

    virtual void do_mouse_enter_event( const ui::mouse_enter_event_t& event);
    virtual void do_mouse_leave_event( const ui::mouse_leave_event_t& event);

    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_move_event( const ui::mouse_move_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
		
public:

	static void draw_shape( const shape_t& s, const ui::paint_event_t& event, float node_aspect, bool axes = true);
	static void draw_axes( const shape_t& s, const ui::paint_event_t& event, const Imath::Color3c& col, float node_aspect);
	static void draw_control_polygon( const shape_t& s, float pixel_scale);
	static void draw_triple( const triple_t& t, const Imath::M33f& m, const Imath::V2f& shape_offset, float pixel_scale);
	
	static void draw_bezier_span( const bezier::curve_t<Imath::V2f>& c, const Imath::M33f& m, const Imath::V2f& shape_offset);
	static void draw_bezier_curve( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c, const Imath::V2f& d);
};

} // namespace
} // namespace

#endif
