// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/convert_tool.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/roto_commands.hpp>

namespace ramen
{
namespace roto
{

convert_tool_t::convert_tool_t( image::roto_node_t& parent) : tool_t( parent)
{
	box_pick_mode_ = false;
}

void convert_tool_t::draw_overlay( const ui::paint_event_t& event) const
{
    if( box_pick_mode_)
	{
		gl_color3f( 1, 0, 0);
		event.view->frame_rect( box_);
	}
}

void convert_tool_t::mouse_press_event( const ui::mouse_press_event_t& event)
{
    box_pick_mode_ = false;

	shape_t *selected = parent().selected();
	
    if( !selected)
		return;
	
    if( event.modifiers & ui::event_t::control_modifier)
    {
		box_pick_mode_ = true;

		if( !(event.modifiers & ui::event_t::shift_modifier))
		    parent().selected()->deselect_all_points();

		event.view->update();
		return;
    }

    triple_t *active_point = pick_point( (parent().selected()), event);

    if( active_point)
    {			
		if( !active_point->selected())
		{
		    if( !(event.modifiers & ui::event_t::shift_modifier))
				selected->deselect_all_points();

		    active_point->select( true);
			
			if( event.modifiers & ui::event_t::shift_modifier)
				return;
		}

		std::auto_ptr<undo::command_t> cmd( new undo::modify_shape_command_t( parent(), selected));
		selected->toggle_corner_curve_selected_points();
		parent().notify();
		
		if( selected->autokey() || !selected->anim_curve().empty())
		{
			selected->set_shape_key();
			app().ui()->update_anim_editors();
		}
		
		app().document().undo_stack().push_back( cmd);
		app().ui()->update();
    }
	else
		selected->deselect_all_points();
}

void convert_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	if( box_pick_mode_)
	{
		box_ = Imath::Box2f( event.click_wpos);
		box_.extendBy( event.wpos);
		event.view->update();
	}
}

void convert_tool_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
    if( box_pick_mode_)
	{
		if( box_pick_mode_)
			select_points_in_box( parent().selected(), box_, event);
	}
	
	box_pick_mode_ = false;
	event.view->update();
}

} // namespace
} // namespace
