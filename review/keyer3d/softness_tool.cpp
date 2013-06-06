// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/key/keyer3d/softness_tool.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node.hpp>
#include<ramen/nodes/image/key/keyer3d/toolbar.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace keyer3d
{

softness_tool_t::softness_tool_t( image::keyer3d_node_t& parent) : tool_t( parent) {}

void softness_tool_t::mouse_press_event( const ui::mouse_press_event_t& event)
{
	active_ = false;
	
	Imath::V2i p( event.wpos.x / event.aspect_ratio, event.wpos.y);
	boost::optional<Imath::Color3f> col( parent().sample_input( p));
	
	if( col)
	{
		col_ = col.get();
		float a = parent().keyer().calc_alpha( col_);
		active_ = ( a != 0.0f);
	}
}

void softness_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	if( event.first_drag && active_)
		cmd_.reset( new undo::keyer3d_command_t( parent()));
	
	int offset = event.pos.x - event.last_pos.x;
	
	if( offset)
	{
		parent().add_softness( col_, offset * 0.005f);
		parent().notify();
	}
}

void softness_tool_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( event.dragged)
	{
		if( cmd_.get())
		{
			app().document().undo_stack().push_back( cmd_);
			app().ui()->update();
		}
	}
	
	active_ = false;
}

} // namespace
} // namespace
