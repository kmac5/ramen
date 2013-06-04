// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_ROTO_SELECT_TOOL_HPP
#define	RAMEN_ROTO_SELECT_TOOL_HPP

#include<ramen/nodes/image/roto/tool.hpp>

#include<memory>

#include<ramen/undo/command.hpp>

namespace ramen
{
namespace roto
{

class select_tool_t : public tool_t
{
public:

    select_tool_t( image::roto_node_t& parent);

    virtual void draw_overlay( const ui::paint_event_t& event) const;
	
    virtual void key_press_event( const ui::key_press_event_t& event);
	
    virtual void mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void mouse_release_event( const ui::mouse_release_event_t& event);
	
private:

	shape_t *pick_nulls( const ui::mouse_press_event_t& event, bool& xaxis, bool& yaxis);
	shape_t *pick_shape( const ui::mouse_press_event_t& event);
	
	bool pick_axes( const shape_t& s, const ui::mouse_event_t& event, bool& xaxis, bool& yaxis) const;
	bool pick_span( const bezier::curve_t<Imath::V2f>& c, const Imath::V2f& p, float pixel_scale) const;
	
	triple_t *pick_tangent( shape_t& s, const ui::mouse_event_t& event, bool& left);
	
    // modes
    bool box_pick_mode_;
	Imath::Box2f pick_box_;

    bool drag_curve_mode_;
	bool drag_curve_x_, drag_curve_y_;
	
    bool drag_points_mode_;
    bool drag_tangents_mode_;
    bool left_tangent_;

	shape_t *selected_;
    triple_t *active_point_;
	
	Imath::M33f inv_xf_;

	std::auto_ptr<undo::command_t> cmd_;
};

} // namespace
} // namespace

#endif
