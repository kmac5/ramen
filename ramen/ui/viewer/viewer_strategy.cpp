// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/viewer/viewer_strategy.hpp>

#include<ramen/GL/gl.hpp>

#include<ramen/ui/viewer/viewer_context.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

viewer_strategy_t::viewer_strategy_t( viewer_context_t *p) : parent_( p), active_( false)
{
	RAMEN_ASSERT( p != 0);
}

viewer_strategy_t::~viewer_strategy_t() {}
	
QWidget *viewer_strategy_t::toolbar() { return 0;}

void viewer_strategy_t::begin_active_view()
{
	set_active( true);
	do_begin_active_view();
}

void viewer_strategy_t::end_active_view()
{
	do_end_active_view();
	set_active( false);
}

void viewer_strategy_t::update()
{
    RAMEN_ASSERT( parent());
    parent()->update();
}

void viewer_strategy_t::paint() { gl_clear( GL_COLOR_BUFFER_BIT);}

void viewer_strategy_t::enter_event( QEvent *event) { event->accept();}
void viewer_strategy_t::leave_event( QEvent *event) { event->accept();}

void viewer_strategy_t::key_press_event( QKeyEvent *event)    { event->ignore();}
void viewer_strategy_t::key_release_event( QKeyEvent *event)  { event->ignore();}

void viewer_strategy_t::mouse_move_event( QMouseEvent *event)     { event->ignore();}
void viewer_strategy_t::mouse_press_event( QMouseEvent *event)    { event->ignore();}
void viewer_strategy_t::mouse_release_event( QMouseEvent *event)  { event->ignore();}

void viewer_strategy_t::wheel_event( QWheelEvent *event)  { event->ignore();}

Imath::Color4f viewer_strategy_t::color_at( int x, int y) const
{
	return Imath::Color4f( 0, 0, 0, 0);
}

void viewer_strategy_t::pick_colors_in_box( const Imath::Box2i& b,
                                            boost::function<void ( const Imath::Color4f&)> f) const
{
}

void viewer_strategy_t::frame_rect( const Imath::Box2f& b) const
{
	gl_begin( GL_LINE_LOOP);
		gl_vertex2f( b.min.x, b.min.y);
		gl_vertex2f( b.max.x, b.min.y);
		gl_vertex2f( b.max.x, b.max.y);
		gl_vertex2f( b.min.x, b.max.y);
	gl_end();
}

void viewer_strategy_t::frame_rect( const Imath::Box2i& b) const
{
	gl_begin( GL_LINE_LOOP);
		gl_vertex2i( b.min.x, b.min.y);
		gl_vertex2i( b.max.x, b.min.y);
		gl_vertex2i( b.max.x, b.max.y);
		gl_vertex2i( b.min.x, b.max.y);
	gl_end();
}

/*****************************************************/

default_viewer_strategy_t::default_viewer_strategy_t( viewer_context_t *p) : viewer_strategy_t( p)
{
}

bool default_viewer_strategy_t::can_display_node( node_t *n) const { return true;}

void default_viewer_strategy_t::set_active_node( node_t *n)  { parent()->update();}
void default_viewer_strategy_t::set_context_node( node_t *n) { parent()->update();}

} // viewer
} // ui
} // ramen
