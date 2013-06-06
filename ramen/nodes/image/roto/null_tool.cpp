// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/null_tool.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/GL/Imath_gl.hpp>
#include<ramen/manipulators/draw.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/roto_commands.hpp>

namespace ramen
{
namespace roto
{

null_tool_t::null_tool_t( image::roto_node_t& parent) : tool_t( parent), creating_( false) {}

void null_tool_t::draw_overlay( const ui::paint_event_t& event) const
{
	if( creating_)
	{
		Imath::Color3c col( 255, 0, 0);
		gl_line_width( manipulator_t::default_line_width());
		manipulators::draw_cross( p_, 3 / event.pixel_scale / event.aspect_ratio, 3 / event.pixel_scale, col, event.pixel_scale);
		manipulators::draw_ellipse(p_, 7 / event.pixel_scale, 7 / event.pixel_scale, col, 20);
	}
}

void null_tool_t::mouse_press_event( const ui::mouse_press_event_t& event)
{ 
	creating_ = true;
	p_ = event.wpos;
	event.view->update();
}

void null_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	p_ = event.wpos;
	event.view->update();
}

void null_tool_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( creating_)
	{
		creating_ = false;
		std::auto_ptr<roto::shape_t> s = parent().create_null();
		
		p_ = event.wpos;
		p_.x /= event.aspect_ratio;
		s->set_center( p_, false);
		s->set_translation( Imath::V2f( 0, 0));

		std::auto_ptr<undo::add_roto_command_t> cmd( new undo::add_roto_command_t( parent(), s));
		cmd->redo();
		app().document().undo_stack().push_back( cmd);
		app().ui()->update();
	}
}

} // namespace
} // namespace
