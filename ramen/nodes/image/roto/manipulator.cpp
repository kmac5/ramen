// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/manipulator.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/assert.hpp>

#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/shape.hpp>
#include<ramen/nodes/image/roto/toolbar.hpp>
#include<ramen/nodes/image/roto/tool.hpp>

namespace ramen
{
namespace roto
{

manipulator_t::manipulator_t() : ramen::manipulator_t() {}

void manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
	RAMEN_ASSERT( parent());

	const image::roto_node_t *roto = dynamic_cast<const image::roto_node_t*>( parent());
	RAMEN_ASSERT( roto);

    gl_point_size( 5);
    gl_line_width( 1);

	gl_push_matrix();
	gl_scalef( event.aspect_ratio, 1);
	
	glEnable( GL_MAP1_VERTEX_3);
    boost::range::for_each( roto->scene(), boost::bind( &roto::manipulator_t::draw_shape, _1, boost::cref( event), roto->aspect_ratio(), true));
	glDisable( GL_MAP1_VERTEX_3);
	
	// clear errors
	while( glGetError() != GL_NO_ERROR)
		;
	
	gl_pop_matrix();
	
	roto->toolbar().active_tool().draw_overlay( event);
}

bool manipulator_t::do_key_press_event( const ui::key_press_event_t& event)
{ 
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().key_press_event( event);
	return true;
}

void manipulator_t::do_key_release_event( const ui::key_release_event_t& event)
{
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().key_release_event( event);
}

void manipulator_t::do_mouse_enter_event( const ui::mouse_enter_event_t& event)
{
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().mouse_enter_event( event);
}

void manipulator_t::do_mouse_leave_event( const ui::mouse_leave_event_t& event)
{
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().mouse_leave_event( event);
}

bool manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{ 
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().mouse_press_event( event);
	return true;
}

void manipulator_t::do_mouse_move_event( const ui::mouse_move_event_t& event)
{
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().mouse_move_event( event);
}

void manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().mouse_drag_event( event);
}

void manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	image::roto_node_t *roto = dynamic_cast<image::roto_node_t*>( parent());
	roto->toolbar().active_tool().mouse_release_event( event);
}

void manipulator_t::draw_shape( const shape_t& s, const ui::paint_event_t& event, float node_aspect, bool axes)
{
	if( s.is_null())
	{
		Imath::Color3c col;
		
		if( s.selected())
			col = Imath::Color3c( 255, 0, 0);
		else
			col = s.display_color();

		draw_axes( s, event, col, node_aspect);
		return;
	}

    gl_line_width( 1);
    gl_color( s.display_color());
	s.for_each_span( boost::bind( &roto::manipulator_t::draw_bezier_span, _1, s.global_xform(), s.offset()));

    if( s.selected())
	{
		draw_control_polygon( s, event.pixel_scale);
	
	    if( axes)
			draw_axes( s, event, s.display_color(), node_aspect);
	}
}

void manipulator_t::draw_axes( const shape_t& s, const ui::paint_event_t& event, const Imath::Color3c& col, float node_aspect)
{	
	gl_line_width( default_line_width());
	
	Imath::V2f c( s.center());
	Imath::V2f x( c.x + ( 70 / event.pixel_scale), c.y);
	Imath::V2f y( c.x, c.y - ( 70 / event.pixel_scale));
	
	c *= s.global_xform();
	x *= s.global_xform();
	y *= s.global_xform();
	
	x -= c;
	y -= c;
	
    manipulators::draw_xy_axes( c, x, y, event.aspect_ratio / node_aspect, col, col, event.pixel_scale);
    manipulators::draw_cross( c, 3 / event.pixel_scale, 3 / event.pixel_scale, col, event.pixel_scale);
	manipulators::draw_ellipse( c, 7 / event.pixel_scale, 7 / event.pixel_scale, col, 20);
}

void manipulator_t::draw_control_polygon( const shape_t& s, float pixel_scale)
{
    boost::range::for_each( s.triples(), boost::bind( &manipulator_t::draw_triple, _1,
												boost::cref( s.global_xform()),
												s.offset(), pixel_scale));
}

void manipulator_t::draw_triple( const triple_t& t, const Imath::M33f& m, const Imath::V2f& shape_offset, float pixel_scale)
{
	Imath::V2f p0 = ( t.p0() + shape_offset) * m;
	Imath::V2f p1 = ( t.p1() + shape_offset) * m;
	Imath::V2f p2 = ( t.p2() + shape_offset) * m;

	if( !t.corner())
	{
		gl_begin( GL_LINES);
			gl_vertex( p0);
			gl_vertex( p1);
			gl_vertex( p1);
			gl_vertex( p2);
		gl_end();
	}
	
	if( t.selected())
	{
		gl_line_width( manipulator_t::default_line_width());
		manipulators::draw_small_box( p1, 4 / pixel_scale);
		
		if( !t.corner())
		{
			manipulators::draw_small_box( p0, 3 / pixel_scale);
			manipulators::draw_small_box( p2, 3 / pixel_scale);			
		}

		gl_line_width( 1);		
	}
	else
	{
		gl_begin( GL_POINTS);
			gl_vertex( p1);
			
			if( !t.corner())
			{
				gl_vertex( p0);
				gl_vertex( p2);
			}
		gl_end();
	}
}

void manipulator_t::draw_bezier_span( const bezier::curve_t<Imath::V2f>& c, const Imath::M33f& m, const Imath::V2f& shape_offset)
{
	draw_bezier_curve( ( c.p[0] + shape_offset) * m, 
					   ( c.p[1] + shape_offset) * m, 
					   ( c.p[2] + shape_offset) * m, 
					   ( c.p[3] + shape_offset) * m);
}

void manipulator_t::draw_bezier_curve( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c, const Imath::V2f& d)
{
	GLfloat ctrlpoints[4][3];

    ctrlpoints[0][0] = a.x;
    ctrlpoints[0][1] = a.y;
    ctrlpoints[0][2] = 0;

    ctrlpoints[1][0] = b.x;
    ctrlpoints[1][1] = b.y;
    ctrlpoints[1][2] = 0;

    ctrlpoints[2][0] = c.x;
    ctrlpoints[2][1] = c.y;
    ctrlpoints[2][2] = 0;

    ctrlpoints[3][0] = d.x;
    ctrlpoints[3][1] = d.y;
    ctrlpoints[3][2] = 0;

	// for some reason, this code generates 
	// an OpenGL error, and Ramen crashes...
    glMap1f( GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);

    glBegin( GL_LINE_STRIP);
    for (int i = 0; i <= 30; ++i)
		glEvalCoord1f( ( GLfloat) i/30.0f);
    glEnd();
}

} // namespace
} // namespace
