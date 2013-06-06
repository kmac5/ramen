// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/rect_tool.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/roto_commands.hpp>

#include<ramen/app/document.hpp>

namespace ramen
{
namespace roto
{

rect_tool_t::rect_tool_t( image::roto_node_t& parent) : tool_t( parent), creating_( false) {}

void rect_tool_t::draw_overlay( const ui::paint_event_t& event) const
{
	if( creating_)
	{
		gl_color3f( 1, 0, 0);
		gl_line_width( 1);
		event.view->frame_rect( box_);
	}
}

void rect_tool_t::mouse_press_event( const ui::mouse_press_event_t& event) {}

void rect_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	if( event.first_drag)
		creating_ = true;

	box_ = Imath::Box2f( event.click_wpos);
	box_.extendBy( event.wpos);
	event.view->update();
}

void rect_tool_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( event.dragged)
	{
		box_.min.x /= event.aspect_ratio;
		box_.max.x /= event.aspect_ratio;
		std::auto_ptr<roto::shape_t> s = parent().create_shape( box_);
		
		s->set_center( s->bbox().center(), false);
		s->set_translation( Imath::V2f( 0, 0));

		if( s->autokey())
			s->set_shape_key( app().document().composition().frame());
		
		std::auto_ptr<undo::add_roto_command_t> cmd( new undo::add_roto_command_t( parent(), s));
		cmd->redo();
		app().document().undo_stack().push_back( cmd);
		app().ui()->update();
	}

	creating_ = false;
}

} // namespace
} // namespace
