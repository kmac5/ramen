// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/anim/anim_editor_select_tool.hpp>

#include<boost/foreach.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<OpenEXR/ImathFun.h>
#include<OpenEXR/ImathVec.h>

#include<QKeyEvent>
#include<QMouseEvent>
#include<QPainter>

#include<ramen/app/application.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/ui/anim/anim_curves_view.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/pick_curves_visitors.hpp>
#include<ramen/ui/anim/select_keys_visitors.hpp>
#include<ramen/ui/anim/edit_tangents_visitors.hpp>
#include<ramen/ui/anim/anim_editor_toolbar.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{

anim_editor_select_tool_t::anim_editor_select_tool_t()
{
	box_pick_mode_ = false;
	dragged_ = false;
	drag_keyframes_mode_ = false;
	drag_tangents_mode_ = false;
	insert_keyframe_ = false;
	track_ = 0;
	key_index_ = -1;
	left_ = false;
	drag_command_ = 0;
	break_tangents_ = false;
}

void anim_editor_select_tool_t::draw_overlay( anim_curves_view_t& view) const
{
    if( box_pick_mode_)
    {
        QPen pen;
        pen.setColor( QColor( 255, 0, 0, 255));
        view.painter()->setPen( pen);
        view.painter()->drawLine( push_x_, push_y_, last_x_, push_y_);
        view.painter()->drawLine( last_x_, push_y_, last_x_, last_y_);
        view.painter()->drawLine( last_x_, last_y_, push_x_, last_y_);
        view.painter()->drawLine( push_x_, last_y_, push_x_, push_y_);
    }
}

void anim_editor_select_tool_t::key_press_event( anim_curves_view_t& view, QKeyEvent *event)    { event->ignore();}
void anim_editor_select_tool_t::key_release_event( anim_curves_view_t& view, QKeyEvent *event)  { event->ignore();}

void anim_editor_select_tool_t::mouse_press_event( anim_curves_view_t& view, QMouseEvent *event)
{
    push_x_ = last_x_ = event->x();
    push_y_ = last_y_ = event->y();
    dragged_ = false;
    box_pick_mode_ = false;
    drag_tangents_mode_ = false;
    drag_keyframes_mode_ = false;
    left_ = false;
    insert_keyframe_ = false;
    key_index_ = -1;
	track_ = 0;
	
	bool shift_pressed = event->modifiers() & Qt::ShiftModifier;

    Imath::V2i p( push_x_, push_y_);

	if( view.show_tangents())
	{
		pick_tangents_visitor v0( view, p);
		BOOST_FOREACH( anim::track_t *t, app().ui()->anim_editor().active_tracks())
		{
			boost::apply_visitor( v0, t->curve().get());
	
			if( v0.key_index >= 0)
			{
				drag_tangents_mode_ = true;
				track_ = t;
				key_index_ = v0.key_index;
				left_ = v0.left;
				break_tangents_ = ( event->modifiers() & Qt::ControlModifier);
				return;
			}
		}
	}

    pick_keyframe_visitor v1( view, p);
    BOOST_FOREACH( anim::track_t *t, app().ui()->anim_editor().active_tracks())
    {
        boost::apply_visitor( v1, t->curve().get());

        if( v1.key_index >= 0)
        {
			track_ = t;
            key_index_ = v1.key_index;
			break;
        }
    }

	if( key_index_ < 0 && (event->modifiers() & Qt::ControlModifier))
	{
		can_insert_keyframe_visitor v0( view, p, view.snap_frames());
		
		BOOST_FOREACH( anim::track_t *t, app().ui()->anim_editor().active_tracks())
		{
			boost::apply_visitor( v0, t->curve().get());
			
			if( v0.can_insert)
			{
				app().ui()->anim_editor().create_command();
				app().ui()->anim_editor().command()->add_track( t);
				insert_keyframe_visitor v1( view, p, view.snap_frames());
				boost::apply_visitor( v1, t->curve().get());
				
				track_ = t;
				key_index_ = v1.key_index;
				track_->notify();
				
				app().ui()->anim_editor().push_command();
				app().ui()->active_node()->notify();
				view.update();
				break;
			}
		}
	}

    if( key_index_ >= 0)
    {
		if( !shift_pressed)
		{
		    if( !keyframe_is_selected( track_->curve().get(), key_index_))
		    {
				app().ui()->anim_editor().deselect_all();
				select_keyframe( track_->curve().get(), key_index_);
		    }

		    drag_keyframes_mode_ = true;
		}
		else
		{
			toggle_select_keyframe( track_->curve().get(), key_index_);

		    if( keyframe_is_selected( track_->curve().get(), key_index_))
				drag_keyframes_mode_ = true;
		}
		
		view.update();
		event->accept();
		return;
	}

    box_pick_mode_ = true;

    if( !shift_pressed)
        app().ui()->anim_editor().deselect_all();

    event->accept();
}

void anim_editor_select_tool_t::mouse_move_event( anim_curves_view_t& view, QMouseEvent *event) { event->accept();}

void anim_editor_select_tool_t::mouse_drag_event( anim_curves_view_t& view, QMouseEvent *event)
{
    if( !dragged_)
    {
        // first time dragged init
        if( drag_tangents_mode_)
		{
			RAMEN_ASSERT( track_);
			RAMEN_ASSERT( key_index_ != -1);
			app().ui()->anim_editor().create_command();
			app().ui()->anim_editor().command()->add_track( track_);
		}
        else
        {
            if( drag_keyframes_mode_)
            {
                if( !insert_keyframe_) // command is already created
					create_drag_command();
            }
        }

        dragged_ = true;
    }

    if( drag_tangents_mode_)
    {		
		drag_tangents_visitor v( view, key_index_, left_, Imath::V2i( event->x(), event->y()), break_tangents_);
		boost::apply_visitor( v, track_->curve().get());
        track_->notify();
        app().ui()->active_node()->notify();
    }
    else
    {
        if( drag_keyframes_mode_)
        {
			bool drag_time_only = false;
			bool drag_value_only = false;
			
			if( event->modifiers() & Qt::ShiftModifier)
			{
				int dx = Imath::abs( event->x() - push_x_);
				int dy = Imath::abs( event->y() - push_y_);
				
				if( dx == dy)
				{
					last_x_ = event->x();
					last_y_ = event->y();
					event->accept();
					return;
				}
				
				if( dx > dy)
				{
					drag_time_only = true;
					drag_value_only = false;
				}
				else
				{
					drag_value_only = true;
					drag_time_only = false;
				}
			}

            Imath::V2f d( ( event->x() - push_x_) / view.time_scale(), -( event->y() - push_y_) / view.value_scale());

			if( drag_time_only)
				d.y = 0;
			else
			{
				if( drag_value_only)
					d.x = 0;
			}

			drag_command_->start_drag( d, view.snap_frames());
			BOOST_FOREACH( anim::track_t *t, app().ui()->anim_editor().active_tracks())
			{
				if( app().ui()->anim_editor().any_keyframe_selected( t->curve().get()))
					drag_command_->drag_curve( t);
			}
			
			drag_command_->end_drag();
        }
    }

    last_x_ = event->x();
    last_y_ = event->y();
    event->accept();
    view.update();
}

void anim_editor_select_tool_t::mouse_release_event( anim_curves_view_t& view, QMouseEvent *event)
{
    if( drag_tangents_mode_ || drag_keyframes_mode_)
		app().ui()->anim_editor().push_command();
    else
    {
		if( box_pick_mode_)
		{
			Imath::Box2i area;
			area.extendBy( Imath::V2i( push_x_, push_y_));
			area.extendBy( Imath::V2i( last_x_, last_y_));

			box_pick_keyframes_visitor v( view, area);
			BOOST_FOREACH( anim::track_t *t, app().ui()->anim_editor().active_tracks())
				boost::apply_visitor( v, t->curve().get());
		}
   }

    dragged_ = false;
	drag_tangents_mode_ = false;
	drag_keyframes_mode_ = false;
    insert_keyframe_ = false;
	box_pick_mode_ = false;
	track_ = 0;
	key_index_ = -1;
	left_ = false;
	drag_command_ = 0;
    event->accept();
	
	app().ui()->anim_editor().toolbar().selection_changed();
    view.update();
}

void anim_editor_select_tool_t::select_keyframe( anim::any_curve_ptr_t& c, int index)
{
	select_keyframe_visitor v( index);
	boost::apply_visitor( v, c);
}

void anim_editor_select_tool_t::toggle_select_keyframe( anim::any_curve_ptr_t& c, int index)
{
	toggle_select_keyframe_visitor v( index);
	boost::apply_visitor( v, c);
}

bool anim_editor_select_tool_t::keyframe_is_selected( anim::any_curve_ptr_t& c, int index) const
{
	keyframe_is_selected_visitor v( index);
	boost::apply_visitor( v, c);
	return v.selected;
}

void anim_editor_select_tool_t::create_drag_command()
{
	RAMEN_ASSERT( drag_command_ == 0);
	
	node_t *n = app().ui()->active_node();
	RAMEN_ASSERT( n);
	
	drag_command_ = new undo::drag_keys_command_t( n, app().ui()->anim_editor().track_model());
	app().ui()->anim_editor().set_command( drag_command_);
}

} // namespace
} // namespace
