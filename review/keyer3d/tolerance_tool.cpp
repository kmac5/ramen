// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/key/keyer3d/tolerance_tool.hpp>

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

tolerance_tool_t::tolerance_tool_t( image::keyer3d_node_t& parent) : tool_t( parent), box_mode_( false), remove_mode_( false) {}

void tolerance_tool_t::draw_overlay( const ui::paint_event_t& event) const
{
	if( box_mode_)
	{
		gl_color3f( 1, 0, 0);
		gl_line_width( 1);
		event.view->frame_rect( area_);
	}
}

void tolerance_tool_t::mouse_press_event( const ui::mouse_press_event_t& event)
{
	any_change_ = false;
	remove_mode_ = event.modifiers & ui::event_t::shift_modifier;
	box_mode_ = event.modifiers & ui::event_t::control_modifier;
	
	if( !box_mode_)
	{
		cmd_.reset( new undo::keyer3d_command_t( parent()));
		do_tolerance( event);
	}
}

void tolerance_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{ 
	if( box_mode_)
	{
		area_ = Imath::Box2f( event.click_wpos);
		area_.extendBy( event.wpos);
		event.view->update();
	}
	else
		do_tolerance( event);
}

void tolerance_tool_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( box_mode_)
	{
		if( event.dragged)
		{
			area_.min.x = area_.min.x / event.aspect_ratio;
			area_.max.x = area_.max.x / event.aspect_ratio;
		
			if( !area_.isEmpty())
			{
				std::vector<Imath::Color3f> samples;
				parent().sample_input( Imath::Box2i( area_.min, area_.max), samples);
		
				if( !samples.empty())
				{
					cmd_.reset( new undo::keyer3d_command_t( parent()));
					
					if( remove_mode_)
						any_change_ = parent().remove_tolerance( samples);
					else
						any_change_ = parent().add_tolerance( samples);
		
					if( any_change_)
						parent().notify();
				}
			}
		}
	}

	if( any_change_)
	{
		app().document().undo_stack().push_back( cmd_);
		app().ui()->update();
	}
	else
		cmd_.reset();
	
	any_change_ = false;
	box_mode_ = false;
	remove_mode_ = false;
	event.view->update();
}

void tolerance_tool_t::do_tolerance( const ui::mouse_event_t& event)
{
	Imath::V2i p( event.wpos.x / event.aspect_ratio, event.wpos.y);
	boost::optional<Imath::Color3f> col( parent().sample_input( p));
	
	bool changed;
	
	if( col)
	{
		if( remove_mode_)
			changed = parent().remove_tolerance( col.get());
		else
			changed = parent().add_tolerance( col.get());
	}
	
	if( changed)
	{
		any_change_ = true;
		parent().notify();
	}
}

} // namespace
} // namespace
