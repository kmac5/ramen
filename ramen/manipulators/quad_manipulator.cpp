// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>

#include<ramen/manipulators/quad_manipulator.hpp>
#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/viewer_strategy.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

quad_manipulator_t::quad_manipulator_t( float2_param_t *topleft, float2_param_t *topright,
										float2_param_t *botleft, float2_param_t *botright) : manipulator_t()
{
	RAMEN_ASSERT( topleft && topright && botleft && botright);
	topleft_ = topleft;
	topright_ = topright;
	botleft_ = botleft;
	botright_ = botright;
	picked_corner_ = -1;
}

void quad_manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{	
	boost::array<Imath::V2f, 4> dst_pts;
	get_corners( dst_pts, event.aspect_ratio);

	gl_line_width( default_line_width());
	gl_point_size( default_control_point_size());	
	// shadow
	float off = manipulators::shadow_offset( event.pixel_scale);
	gl_color3ub( 0, 0, 0);
	gl_begin( GL_LINE_LOOP);
		for( int i = 0; i < 4; ++i)
			gl_vertex2f( dst_pts[i].x + off, dst_pts[i].y + off);
	gl_end();

	gl_begin( GL_POINTS);
		for( int i = 0; i < 4; ++i)
			gl_vertex2f( dst_pts[i].x + off, dst_pts[i].y + off);
	gl_end();
	
	// color	
	gl_color( default_color());
	gl_begin( GL_LINE_LOOP);
		for( int i = 0; i < 4; ++i)
			gl_vertex2f( dst_pts[i].x, dst_pts[i].y);
	gl_end();

	// draw corners
	gl_begin( GL_POINTS);
	for( int i = 0; i < 4; ++i)
	{
		if( i != picked_corner_)
			gl_vertex2f( dst_pts[i].x, dst_pts[i].y);
	}
	gl_end();
	
	for( int i = 0; i < 4; ++i)
	{
		if( i == picked_corner_)
			manipulators::draw_small_box( dst_pts[i], 3 / event.pixel_scale);
	}
}

bool quad_manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{
	picked_corner_ = -1;

	boost::array<Imath::V2f, 4> dst_pts;
	get_corners( dst_pts, event.aspect_ratio);
	
	for( int i = 0; i < 4; ++i)
	{
		if( manipulators::inside_pick_distance( event.wpos, dst_pts[i], event.pixel_scale))
		{
			picked_corner_ = i;
			break;
		}
	}

	event.view->update();
	return picked_corner_ != -1;
}

void quad_manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	RAMEN_ASSERT( picked_corner_ != -1);
	
	if( event.first_drag)
		topleft_->param_set()->begin_edit();

	float2_param_t *p;

	switch( picked_corner_)
	{
		case 0: // topleft
			p = topleft_;
		break;
			
		case 1: // topright
			p = topright_;
		break;
			
		case 2: // botright
			p = botright_;
		break;
			
		case 3: // botleft
			p = botleft_;
		break;
	}

	Imath::V2f offset( event.wpos - event.last_wpos);
	offset.x = offset.x / event.aspect_ratio;
	
	Imath::V2f q = get_absolute_value<Imath::V2f>( *p);
	p->set_absolute_value( q + offset);
	p->update_widgets();
	app().ui()->update_anim_editors();
	
	if( p->track_mouse())
		p->param_set()->notify_parent();

	event.view->update();
}

void quad_manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	picked_corner_ = -1;

	if( event.dragged)
	{
		topleft_->param_set()->end_edit( !topleft_->track_mouse());
		app().ui()->update();
	}
}

void quad_manipulator_t::get_corners( boost::array<Imath::V2f, 4>& pts, float aspect, int scale) const
{
    pts[0] = get_absolute_value<Imath::V2f>( *topleft_) * scale;
    pts[1] = get_absolute_value<Imath::V2f>( *topright_) * scale;
    pts[2] = get_absolute_value<Imath::V2f>( *botright_) * scale;
    pts[3] = get_absolute_value<Imath::V2f>( *botleft_) * scale;
	
	for( int i = 0; i < 4; ++i)
		pts[i].x *= aspect;
}

} // namespace
