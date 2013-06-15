// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/select_tool.hpp>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<ramen/assert.hpp>

#include<ramen/manipulators/pick.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/shape.hpp>
#include<ramen/nodes/image/roto/roto_commands.hpp>

namespace ramen
{
namespace roto
{

select_tool_t::select_tool_t( image::roto_node_t& parent) : tool_t( parent)
{
	box_pick_mode_ = false;
	drag_curve_mode_ = false;
	drag_curve_x_ = true;
	drag_curve_y_ = true;
	drag_points_mode_ = false;
	drag_tangents_mode_ = false;
	left_tangent_ = false;
	selected_ = 0;
	active_point_ = 0;
}

void select_tool_t::draw_overlay( const ui::paint_event_t& event) const
{
    if( box_pick_mode_)
	{
		gl_color3f( 1, 0, 0);
		event.view->frame_rect( pick_box_);
	}
}

void select_tool_t::key_press_event( const ui::key_press_event_t& event)
{
	if( event.key == ui::key_event_t::backspace_key ||
		event.key == ui::key_event_t::delete_key)
	{
		// delete shapes or points here.
        selected_ = parent().selected();
        if( selected_)
		{
			std::auto_ptr<undo::command_t> cmd;
			
			if( selected_->can_delete_selected_points())
			{
				cmd.reset( new undo::modify_shape_command_t( parent(), selected_));
				selected_->delete_selected_points();
			}
			else
			{
				cmd.reset( new undo::delete_roto_command_t( parent(), selected_));
				cmd->redo();
			}
			
			app().document().undo_stack().push_back( cmd);
			app().ui()->update();
			parent().notify();
		}
	}
}

void select_tool_t::mouse_press_event( const ui::mouse_press_event_t& event)
{
    box_pick_mode_ = false;
    drag_curve_mode_ = false;
	drag_curve_x_ = true;
	drag_curve_y_ = true;
    drag_points_mode_ = false;
    drag_tangents_mode_ = false;
    left_tangent_ = false;
	inv_xf_ = Imath::M33f();

    selected_ = parent().selected();
    if( selected_)
	{
		if( pick_axes( *selected_, event, drag_curve_x_, drag_curve_y_))
		{
			drag_curve_mode_ = true;

			if( selected_->parent() && selected_->parent()->inv_global_xform())
				inv_xf_ = selected_->parent()->inv_global_xform().get();

			event.view->update();
			return;
		}
		
		if( event.modifiers & ui::event_t::control_modifier)
		{
			box_pick_mode_ = true;
			pick_box_ = Imath::Box2f( event.wpos);
	
			if( !(event.modifiers & ui::event_t::shift_modifier))
				selected_->deselect_all_points();

			goto finish;
		}
		
		// pick points
        active_point_ = pick_point( selected_, event);
        if( active_point_)
		{
		    if( !active_point_->selected())
		    {
				if( !(event.modifiers & ui::event_t::shift_modifier))
				    selected_->deselect_all_points();

				active_point_->toggle_selection();
		    }

		    if( active_point_->selected())
				drag_points_mode_ = true;

			goto finish;
		}

		// pick tangents
        active_point_ = pick_tangent( *selected_, event, left_tangent_);
        if( active_point_)
		{
		    drag_tangents_mode_ = true;
			goto finish;
		}
	}

	parent().deselect_all();
	
    selected_ = pick_nulls( event, drag_curve_x_, drag_curve_y_);
    if( selected_)
	{
		drag_curve_mode_ = true;

		if( selected_->parent() && selected_->parent()->inv_global_xform())
			inv_xf_ = selected_->parent()->inv_global_xform().get();
		
		selected_->select( true);
		parent().selection_changed();
		event.view->update();
		return;
	}
	else
		selected_ = pick_shape( event);
	
	if( selected_)
		selected_->select( true);

finish:

	if( selected_ && selected_->inv_global_xform())
		inv_xf_ = selected_->inv_global_xform().get();
	
	parent().selection_changed();
	event.view->update();
}

void select_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	if( event.first_drag)
	{
		if( drag_curve_mode_)
			selected_->begin_edit_params();
		else
		{
			if( drag_points_mode_ || drag_tangents_mode_)
				cmd_.reset( new undo::modify_shape_command_t( parent(), selected_));
		}
	}
	
	if( box_pick_mode_)
	{
		pick_box_ = Imath::Box2f( event.click_wpos);
		pick_box_.extendBy( event.wpos);
		event.view->update();
		return;
	}

	if( drag_curve_mode_)
	{
		Imath::V2f p( event.wpos - event.last_wpos);
		p.x /= event.aspect_ratio;		
		
		//if( !drag_curve_x_) p.x = 0;
		//if( !drag_curve_y_) p.y = 0;

		Imath::V2f off;
		inv_xf_.multDirMatrix( p, off);
		
		if( event.modifiers & ui::event_t::control_modifier)
			selected_->set_center( selected_->center() + off);
		else
			selected_->set_translation( selected_->translation() + off);

		if( selected_->track_mouse())
			parent().notify();
		else
			event.view->update();
		
		return;
	}
	
	// calc offset in shape global space
	Imath::V2f p( event.wpos - event.last_wpos);
	p.x /= event.aspect_ratio;
	Imath::V2f off;
	inv_xf_.multDirMatrix( p, off);
	
	if( drag_points_mode_)
	{
		RAMEN_ASSERT( !selected_->is_null());

		BOOST_FOREACH( triple_t& t, selected_->triples())
		{
			if( t.selected())
				t.move( off);
		}
		
		if( selected_->track_mouse())
			parent().notify();
		else
			event.view->update();

		return;
	}
	
	if( drag_tangents_mode_)
	{
		RAMEN_ASSERT( !selected_->is_null());
		
		if( event.modifiers & ui::event_t::shift_modifier)
			active_point_->set_broken( true);
	
		if( left_tangent_)
		{
			active_point_->move_left_tangent( off);
	
			if( !active_point_->broken())
				active_point_->adjust_right_tangent();
		}
		else
		{
			active_point_->move_right_tangent( off);
	
			if( !active_point_->broken())
				active_point_->adjust_left_tangent();
		}

		if( selected_->track_mouse())
			parent().notify();
		else
			event.view->update();
		
		return;
	}
}

void select_tool_t::mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( event.dragged)
	{
		if( box_pick_mode_)
			select_points_in_box( selected_, pick_box_, event);
		else
		{
			if( drag_curve_mode_)
				selected_->end_edit_params();
			else
			{
				if( cmd_.get())
				{
					// set a shape key here if needed
					if( selected_->autokey() || !selected_->anim_curve().empty())
					{
						selected_->set_shape_key();
						app().ui()->update_anim_editors();
					}
					
					app().document().undo_stack().push_back( cmd_);
					
					if( !selected_->track_mouse())
						parent().notify();
					
					app().ui()->update();
				}
			}
		}
	}

	box_pick_mode_ = false;
    drag_curve_mode_ = false;
    drag_points_mode_ = false;
    drag_tangents_mode_ = false;
    left_tangent_ = false;
	selected_ = 0;
	active_point_ = 0;
	event.view->update();
}

bool select_tool_t::pick_axes( const shape_t& s, const ui::mouse_event_t& event, bool& xaxis, bool& yaxis) const
{
	Imath::V2f p( event.wpos);
	p.x /= event.aspect_ratio;
	
	Imath::V2f c( s.center());
	Imath::V2f x( c.x + ( 70 / event.pixel_scale), c.y);
	Imath::V2f y( c.x, c.y - ( 70 / event.pixel_scale));
	
	c *= s.global_xform();
	x *= s.global_xform();
	y *= s.global_xform();
	
	x -= c;
	y -= c;

	switch( manipulators::pick_xy_axes( p, c, x, y, event.aspect_ratio / parent().aspect_ratio(), event.pixel_scale))
	{
		case manipulators::axes_center_picked:
			xaxis = true;
			yaxis = true;
		return true;
		
		case manipulators::axes_x_picked:
			xaxis = true;
			yaxis = false;
		return true;

		case manipulators::axes_y_picked:
			xaxis = false;
			yaxis = true;
		return true;

		default:
		return false;		
	}
}

shape_t *select_tool_t::pick_nulls( const ui::mouse_press_event_t& event, bool& xaxis, bool& yaxis)
{
	BOOST_FOREACH( roto::shape_t& s, parent().scene())
	{
		if( s.is_null())
		{
			if( pick_axes( s, event, xaxis, yaxis))
				return &s;
		}
	}
	
	return 0;
}

shape_t *select_tool_t::pick_shape( const ui::mouse_press_event_t& event)
{
	BOOST_FOREACH( roto::shape_t& s, parent().scene())
	{
		if( !s.is_null())
		{
			if( s.inv_global_xform())
			{
				Imath::V2f p( event.wpos.x / event.aspect_ratio, event.wpos.y);
				p = p * s.inv_global_xform().get();
				p -= s.offset();
				
				Imath::Box2f box( s.bbox());
				
				if( inside_pick_distance( box, p, event.pixel_scale * 2))
				{
					if( s.for_each_span_while( boost::bind( &select_tool_t::pick_span, this, _1, p, event.pixel_scale)))
						return &s;
				}
			}
		}
	}

	return 0;
}

bool select_tool_t::pick_span( const bezier::curve_t<Imath::V2f>& c, const Imath::V2f& p, float pixel_scale) const
{
	if( inside_pick_distance( c.bounding_box(), p, pixel_scale))
	{
		Imath::V2f q( bezier::nearest_point_on_curve( c, p));
		
		if( inside_pick_distance( q, p, pixel_scale))
			return true;
	}
	
	return false;
}

triple_t *select_tool_t::pick_tangent( shape_t& s, const ui::mouse_event_t& event, bool& left)
{
	if( s.inv_global_xform())
	{
		Imath::V2f p( event.wpos.x / event.aspect_ratio, event.wpos.y);
		p = p * s.inv_global_xform().get();
		p -= s.offset();
	
		BOOST_FOREACH( triple_t& t, s.triples())
		{
			if( inside_pick_distance( t.p0(), p, event.pixel_scale))
			{
				left = true;
				return &t;
			}

			if( inside_pick_distance( t.p2(), p, event.pixel_scale))
			{
				left = false;
				return &t;
			}
		}
	}
	
	left = false;
	return 0;
}

} // namespace
} // namespace
