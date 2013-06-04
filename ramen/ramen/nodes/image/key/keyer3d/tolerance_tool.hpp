// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_TOLERANCE_TOOL_HPP
#define	RAMEN_KEYER3D_TOLERANCE_TOOL_HPP

#include<ramen/nodes/image/key/keyer3d/tool.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/nodes/image/key/keyer3d/keyer3d_commands.hpp>

namespace ramen
{
namespace keyer3d
{

class tolerance_tool_t : public tool_t
{
public:

    tolerance_tool_t( image::keyer3d_node_t& parent);

    virtual void draw_overlay( const ui::paint_event_t& event) const;
	
    virtual void mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void mouse_release_event( const ui::mouse_release_event_t& event);
	
private:

	void do_tolerance( const ui::mouse_event_t& event);
	
	bool remove_mode_;
	
	bool box_mode_;
	Imath::Box2f area_;
	
	std::auto_ptr<undo::keyer3d_command_t> cmd_;
	bool any_change_;
};

} // namespace
} // namespace

#endif
