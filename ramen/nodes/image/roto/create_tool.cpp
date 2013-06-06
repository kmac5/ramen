// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/create_tool.hpp>

#include<boost/foreach.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/shape.hpp>
#include<ramen/nodes/image/roto/manipulator.hpp>
#include<ramen/nodes/image/roto/roto_commands.hpp>

#include<ramen/app/document.hpp>

namespace ramen
{
namespace roto
{

create_tool_t::create_tool_t( image::roto_node_t& parent) : tool_t( parent), point_( 0) {}

void create_tool_t::end_active() { new_shape_.reset();}

void create_tool_t::draw_overlay( const ui::paint_event_t& event) const
{
	if( new_shape_.get() && !new_shape_->empty())
	{
		gl_push_matrix();
		gl_scalef( event.aspect_ratio, 1);

		glEnable( GL_MAP1_VERTEX_3);
		roto::manipulator_t::draw_shape( *new_shape_, event, parent().aspect_ratio(), false);
		glDisable( GL_MAP1_VERTEX_3);
		
		// clear gl errors
		while( glGetError() != GL_NO_ERROR)
			;

		gl_pop_matrix();
	}
}

void create_tool_t::key_press_event( const ui::key_press_event_t& event)
{
    switch( event.key)
    {
    case ui::key_event_t::escape_key:
		close_shape( false);
		event.view->update();
    break;

    case ui::key_event_t::enter_key:
    case ui::key_event_t::return_key:
        close_shape();
        event.view->update();
    break;
    }
}

void create_tool_t::mouse_press_event( const ui::mouse_press_event_t& event)
{
	if( !new_shape_.get())
	{
		parent().deselect_all();
		
		float t;
		int span_index;
		shape_t *s = pick_shape( event, span_index, t);
		
		if( s)
		{
			if( t == 0.0f || t == 1.0f)
			  return;

			s->select( true);
			parent().selection_changed();
			
			std::auto_ptr<undo::command_t> cmd( new undo::modify_shape_command_t( parent(), s));
			s->insert_point( span_index, t);
			
			app().document().undo_stack().push_back( cmd);
			app().ui()->update();
			parent().notify();
			return;
		}
		
		new_shape_ = parent().create_shape();
		new_shape_->select( true);
		point_ = 0;
	}
	
    if( !new_shape_->empty())
    {
		if( inside_pick_distance( new_shape_->triples().front().p1(), event))
		{
			close_shape();
		    event.view->update();
		    return;
		}
    }

	Imath::V2f p( event.wpos.x / event.aspect_ratio, event.wpos.y);
    new_shape_->triples().push_back( roto::triple_t( p));
    point_ = &( new_shape_->triples().back());
	event.view->update();
}

void create_tool_t::mouse_drag_event( const ui::mouse_drag_event_t& event)
{
    if( point_)
    {
		Imath::V2f v( event.wpos - event.last_wpos);
		v.x /= event.aspect_ratio;
		point_->move_left_tangent( -v);
		point_->move_right_tangent( v);
		point_->set_corner( false);
		point_->set_broken( false);
		event.view->update();
    }
}

void create_tool_t::mouse_release_event( const ui::mouse_release_event_t& event) { point_ = 0;}

void create_tool_t::close_shape( bool success)
{
    if( new_shape_.get() && success)
	{
		new_shape_->close();

		if( new_shape_->valid())
		{
			new_shape_->set_center( new_shape_->bbox().center(), false);
			new_shape_->set_translation( Imath::V2f( 0, 0), false);

			if( new_shape_->autokey())
				new_shape_->set_shape_key( app().document().composition().frame());
			
			std::auto_ptr<undo::add_roto_command_t> cmd( new undo::add_roto_command_t( parent(), new_shape_));
			cmd->redo();
			app().document().undo_stack().push_back( cmd);
			app().ui()->update();
		}
    }

    new_shape_.reset();
}

shape_t *create_tool_t::pick_shape( const ui::mouse_press_event_t& event, int& span_index, float& t)
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
					span_index = 0;
					bezier::curve_t<Imath::V2f> span;
					
					for( shape_t::const_triple_iterator it( s.triples().begin()); it != s.triples().end()-1; )
					{
						span.p[0] = it->p1();
						span.p[1] = it->p2();
						++it;
						span.p[2] = it->p0();
						span.p[3] = it->p1();
						
						Imath::V2f q( bezier::nearest_point_on_curve( span, p, t));

						if( inside_pick_distance( q, p, event.pixel_scale))
							return &s;
					
						++span_index;
					}
			
					if( s.closed())
					{
						span.p[0] = s.triples().back().p1();
						span.p[1] = s.triples().back().p2();
						span.p[2] = s.triples().front().p0();
						span.p[3] = s.triples().front().p1();

						Imath::V2f q( bezier::nearest_point_on_curve( span, p, t));

						if( inside_pick_distance( q, p, event.pixel_scale))
                            return &s;

						++span_index;
					}					
				}
			}
		}
	}

	return 0;
}

} // namespace
} // namespace
