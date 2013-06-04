// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_SOFTNESS_TOOL_HPP
#define	RAMEN_KEYER3D_SOFTNESS_TOOL_HPP

#include<ramen/nodes/image/key/keyer3d/tool.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/nodes/image/key/keyer3d/keyer3d_commands.hpp>

namespace ramen
{
namespace keyer3d
{

class softness_tool_t : public tool_t
{
public:

    softness_tool_t( image::keyer3d_node_t& parent);

    virtual void mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void mouse_release_event( const ui::mouse_release_event_t& event);

private:
	
	std::auto_ptr<undo::keyer3d_command_t> cmd_;
	Imath::Color3f col_;
	bool active_;
};

} // namespace
} // namespace

#endif
