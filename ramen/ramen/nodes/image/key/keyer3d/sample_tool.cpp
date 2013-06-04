// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/key/keyer3d/sample_tool.hpp>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node.hpp>
#include<ramen/nodes/image/key/keyer3d/toolbar.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace keyer3d
{

sample_tool_t::sample_tool_t( image::keyer3d_node_t& parent) : tool_t( parent) {}

void sample_tool_t::mouse_press_event( const ui::mouse_press_event_t& event) {}
void sample_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event) {}
void sample_tool_t::mouse_release_event( const ui::mouse_release_event_t& event) {}

} // namespace
} // namespace
