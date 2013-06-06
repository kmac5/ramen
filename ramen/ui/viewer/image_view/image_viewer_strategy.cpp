// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/viewer/image_view/image_viewer_strategy.hpp>

#include<sstream>

#include<ramen/GL/glew.hpp>

#include<boost/exception_ptr.hpp>

#include<ramen/GL/glu.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/viewer/viewer_context.hpp>
#include<ramen/ui/viewer/image_view/image_view_toolbar.hpp>
#include<ramen/ui/events_util.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/render/context_guard.hpp>
#include<ramen/render/image_node_renderer.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{
namespace viewer
{

image_viewer_strategy_t::image_viewer_strategy_t( viewer_context_t *p) : viewer_strategy_t( p)
{
    first_image_loaded_ = true;
    pending_update_ = false;

    subsample_ = 1;
	proxy_level_ = 0;
    checks_ = false;
    overlay_ = true;
    mblur_ = false;
	aspect_ = true;
	set_aspect_ratio( 1.0f);

    event_accepted_by_node_ = false;
    scroll_mode_ = false;
}

image_viewer_strategy_t::~image_viewer_strategy_t()
{
	// Do not delete. auto_ptr needs it
	// to handle correcty incomplete types.
}

void image_viewer_strategy_t::init()
{
    viewport_.reset( parent()->width(), parent()->height());
	display_.reset( new ocio::gl_display_manager_t( parent()->display_lut()));
}

QWidget *image_viewer_strategy_t::toolbar()
{
    if( !toolbar_.get())
	{
        toolbar_.reset( new image_view_toolbar_t( this));
		toolbar_->update_widgets( visible_node());
	}

    return toolbar_.get();
}

void image_viewer_strategy_t::do_begin_active_view()
{
	display_transform_changed();

    if( node_t *n = app().ui()->active_node())
    {
		active_connection_ = n->changed.connect( boost::bind( &image_viewer_strategy_t::active_node_changed, this));
        active_overlay_connection_ = n->overlay_changed.connect( boost::bind( &image_viewer_strategy_t::active_overlay_changed, this));
    }

    if( node_t *n = app().ui()->context_node())
    {
		context_connection_ = n->changed.connect( boost::bind( &image_viewer_strategy_t::context_node_changed, this));
        context_overlay_connection_ = n->overlay_changed.connect( boost::bind( &image_viewer_strategy_t::context_overlay_changed, this));
    }

	if( toolbar_.get())
		toolbar_->update_widgets( visible_node());
}

void image_viewer_strategy_t::do_end_active_view()
{
    active_connection_.disconnect();
    active_overlay_connection_.disconnect();
    context_connection_.disconnect();
    context_overlay_connection_.disconnect();
	clear_texture();
}

bool image_viewer_strategy_t::can_display_node( node_t *n) const
{
    return dynamic_cast<image_node_t*>( n) != 0;
}

void image_viewer_strategy_t::set_active_node( node_t *n, bool process)
{
    active_connection_.disconnect();
    active_overlay_connection_.disconnect();

    if( n)
    {
        active_connection_ = n->changed.connect( boost::bind( &image_viewer_strategy_t::active_node_changed, this));
        active_overlay_connection_ = n->overlay_changed.connect( boost::bind( &image_viewer_strategy_t::active_overlay_changed, this));

        if( process)
            active_node_changed();
    }
    else
    {
        if( parent()->view_mode() == viewer_context_t::view_active_node)
			clear_texture();
    }
	
	if( toolbar_.get())
		toolbar_->update_widgets( visible_node());
}

void image_viewer_strategy_t::active_node_changed()
{
    if( parent()->view_mode() != viewer_context_t::view_active_node)
    {
        parent()->update();
        return;
    }

    if( !app().ui()->active_node())
        return;

    if( !parent()->autoupdate())
    {
        pending_update_ = true;
        parent()->update();
        return;
    }

    render_visible_node();
}

void image_viewer_strategy_t::active_overlay_changed() { parent()->update();}

void image_viewer_strategy_t::set_context_node( node_t *n, bool process)
{
    context_connection_.disconnect();
    context_overlay_connection_.disconnect();

    if( n)
    {
        context_connection_ = n->changed.connect( boost::bind( &image_viewer_strategy_t::context_node_changed, this));
        context_overlay_connection_ = n->overlay_changed.connect( boost::bind( &image_viewer_strategy_t::context_overlay_changed, this));

        if( process)
            context_node_changed();
    }
    else
    {
        if( parent()->view_mode() == viewer_context_t::view_context_node)
			clear_texture();
    }

	if( toolbar_.get())
		toolbar_->update_widgets( visible_node());
}

void image_viewer_strategy_t::context_node_changed()
{
    if( parent()->view_mode() != viewer_context_t::view_context_node)
        return;

    if( !app().ui()->context_node())
        return;

    if( !parent()->autoupdate())
    {
        pending_update_ = true;
        parent()->update();
        return;
    }

    render_visible_node();
}

void image_viewer_strategy_t::context_overlay_changed() {}

void image_viewer_strategy_t::call_node_changed()
{
    if( parent()->view_mode() == viewer_context_t::view_active_node)
        active_node_changed();
    else
        context_node_changed();
}

node_t *image_viewer_strategy_t::visible_node()
{
    if( parent()->view_mode() == viewer_context_t::view_active_node)	
		return app().ui()->active_node();

	return app().ui()->context_node();
}

void image_viewer_strategy_t::render_visible_node()
{
    node_t *active_node  = app().ui()->active_node();
    node_t *context_node = app().ui()->context_node();

    render::context_t context = app().document().composition().current_context( render::interface_render);
	context.result_node = visible_node();
	
	render::context_guard_t guard( context, context.result_node);
	
	context.proxy_level = proxy_level();
    context.subsample = subsample();
	
    if( !mblur_)
        context.motion_blur_shutter_factor = 0;

    render::image_node_renderer_t renderer;
	boost::unique_future<bool>& future( app().ui()->render_image( context, renderer));
	
    if( future.has_value())
    {
        if( future.get())
        {
			image_node_t *inode = dynamic_cast<image_node_t*>( context.result_node);
            load_texture( inode);
            parent()->update();
            pending_update_ = false;
        }
        else // render was cancelled
        {
            // what should we do here?
        }
    }
    else
    {
        // an exception was thrown
        // out of memory, show a dialog here
        // app().ui()->error( "Out of memory");
    }
}

void image_viewer_strategy_t::load_texture( image_node_t *n)
{
	parent()->makeCurrent();

	set_aspect_ratio( n->aspect_ratio());
    Imath::Box2i data_window = ImathExt::intersect( n->format(), n->defined());
    image_.reset( n->image(), n->format(), data_window);

    if( first_image_loaded_)
    {
		Imath::V2i p( n->format().center());
		Imath::V2f q( p.x * aspect_ratio() * subsample(), p.y * subsample());
		viewport_.scroll_to_center_point( q);
        first_image_loaded_ = false;
	}
}

void image_viewer_strategy_t::clear_texture()
{
	set_aspect_ratio( 1.0f);
	image_.reset();
}

// options

int image_viewer_strategy_t::proxy_level() const	{ return proxy_level_;}
int image_viewer_strategy_t::subsample() const		{ return subsample_;}
bool image_viewer_strategy_t::mblur_active() const	{ return mblur_;}

void image_viewer_strategy_t::autoupdate_changed()
{
    if( parent()->autoupdate() && pending_update_)
        call_node_changed();
}

void image_viewer_strategy_t::view_mode_changed()   { call_node_changed();}
void image_viewer_strategy_t::frame_changed()       { call_node_changed();}

// ocio
void image_viewer_strategy_t::display_transform_changed()
{
	parent()->makeCurrent();
	display_->set_display_transform( parent()->ocio_config(), parent()->ocio_transform());
	display_->set_exposure( parent()->exposure());
	parent()->update();
}

void image_viewer_strategy_t::exposure_changed()
{
	display_->set_exposure( parent()->exposure());
	parent()->update();
}

void image_viewer_strategy_t::gamma_changed()
{
	display_->set_gamma( parent()->gamma());
	parent()->update();
}

// slots
void image_viewer_strategy_t::aspect_toggle( bool state)
{
	aspect_ = state;
	parent()->update();
}

void image_viewer_strategy_t::mblur_toggle( bool state)
{
    mblur_ = state;
    call_node_changed();
}

void image_viewer_strategy_t::change_resolution( int index)
{
    subsample_ = index + 1;
    call_node_changed();
}

void image_viewer_strategy_t::change_proxy_level( int index)
{
	proxy_level_ = index;
	call_node_changed();
}

void image_viewer_strategy_t::checks_toggle( bool state)
{
    checks_ = state;
    parent()->update();
}

void image_viewer_strategy_t::overlay_toggle( bool state)
{
    overlay_ = state;
    parent()->update();
}

void image_viewer_strategy_t::change_channels( int index)
{
    display_->set_view_channels( (ocio::gl_display_manager_t::view_channels_t) index);
    parent()->update();
}

void image_viewer_strategy_t::roi_toggle( bool state) {}

// coords, paint & resize

float image_viewer_strategy_t::aspect_ratio() const
{
	if( aspect_)
		return aspect_ratio_;

	return 1.0f;
}

void image_viewer_strategy_t::set_aspect_ratio( float asp)
{
	RAMEN_ASSERT( asp > 0);
	aspect_ratio_ = asp;
}

float image_viewer_strategy_t::pixel_scale() const { return viewport_.zoom_x();}

Imath::V2f image_viewer_strategy_t::screen_to_world( const Imath::V2i& p) const		{ return viewport_.screen_to_world( p);}
Imath::V2i image_viewer_strategy_t::world_to_screen( const Imath::V2f& p) const		{ return viewport_.world_to_screen( p);}
Imath::Box2f image_viewer_strategy_t::screen_to_world( const Imath::Box2i& b) const { return viewport_.screen_to_world( b);}
Imath::Box2i image_viewer_strategy_t::world_to_screen( const Imath::Box2f& b) const { return viewport_.world_to_screen( b);}
Imath::V2f image_viewer_strategy_t::screen_to_world_dir( const Imath::V2f& v) const { return viewport_.screen_to_world_dir( v);}

void image_viewer_strategy_t::save_projection()			{ parent()->save_projection();}
void image_viewer_strategy_t::restore_projection()		{ parent()->restore_projection();}
void image_viewer_strategy_t::set_screen_projection()	{ parent()->set_screen_projection();}

void image_viewer_strategy_t::resize( int w, int h) { viewport_.resize( w, h);}

void image_viewer_strategy_t::paint()
{
	paint_event_.view = this;
	paint_event_.aspect_ratio = aspect_ratio();
	paint_event_.pixel_scale = pixel_scale();
	paint_event_.subsample = subsample();

	// start clean
	clear_gl_errors();

    gl_clear( GL_COLOR_BUFFER_BIT);

    // put gl state in a known "state"
    gl_disable( GL_TEXTURE_2D);
	gl_disable( GL_TEXTURE_3D);

	bool use_blend = ( checks_ && display_->view_channels() == ocio::gl_display_manager_t::view_rgb_channels);

	if( use_blend)
	{
		set_screen_projection();
		parent()->draw_checks_background();
	}

    // world space
    gl_matrix_mode( GL_PROJECTION);
    gl_load_identity();
    gl_viewport( 0, 0, parent()->width(), parent()->height());
    glu_ortho2d( viewport_.world().min.x, viewport_.world().max.x, viewport_.world().max.y, viewport_.world().min.y);

    gl_matrix_mode( GL_MODELVIEW);
    gl_load_identity();
	
	gl_push_matrix();
    gl_scalef( subsample() * aspect_ratio(), subsample());
	
	// draw image background if needed
	if( !use_blend)
	{
		Imath::Color3f c = display_->black();
		
		switch( display_->view_channels())
		{
		case ocio::gl_display_manager_t::view_red_channel:
			c.y = c.z = 0;
		break;

		case ocio::gl_display_manager_t::view_green_channel:
			c.x = c.z = 0;
		break;

		case ocio::gl_display_manager_t::view_blue_channel:
			c.x = c.y = 0;
		break;
		
		case ocio::gl_display_manager_t::view_alpha_channel:
			c = Imath::Color3f( 0, 0, 0);
		break;
		}
		
		gl_color4f( c.x, c.y, c.z, 1);
		image_.draw_background();
	}

    gl_enable( GL_TEXTURE_2D);
	gl_enable( GL_TEXTURE_3D);

	display_->activate();

	if( use_blend)
	{
		gl_enable( GL_BLEND);
		gl_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	gl_color4f( 0, 0, 0, 1);
	image_.draw();
	display_->deactivate();

	gl_disable( GL_BLEND);
    gl_disable( GL_TEXTURE_2D);
	gl_disable( GL_TEXTURE_3D);
	
	// draw red frame
	gl_line_width( 1);
	gl_color4f( 0.75, 0.0, 0.0, 1.0);
	image_.frame_display_window();

    if( overlay_)
    {
		gl_color4f( 0.0, 0.0, 0.75, 1.0);
		image_.frame_data_window();
	}
	
	gl_pop_matrix();
	
    if( overlay_)
	{
		node_t *active_node  = app().ui()->active_node();

		if( active_node)
		{
			gl_enable( GL_BLEND);
			gl_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			gl_enable( GL_LINE_SMOOTH);
			
			active_node->draw_overlay( paint_event_);
			check_gl_errors();
			gl_disable( GL_LINE_SMOOTH);	
			gl_disable( GL_BLEND);
		}
    }

	gl_finish();
}

void image_viewer_strategy_t::enter_event( QEvent *event)
{
	enter_event_.view = this;
	enter_event_.aspect_ratio = aspect_ratio();
	enter_event_.pixel_scale = pixel_scale();
	enter_event_.subsample = subsample();	
	
    if( overlay_)
    {
        node_t *active_node  = app().ui()->active_node();

        if( active_node)
            active_node->mouse_enter_event( enter_event_);
    }

    event->accept();
}

void image_viewer_strategy_t::leave_event( QEvent *event)
{
	leave_event_.view = this;
	leave_event_.aspect_ratio = aspect_ratio();
	leave_event_.pixel_scale = pixel_scale();
	leave_event_.subsample = subsample();
	
    if( overlay_)
    {
        node_t *active_node  = app().ui()->active_node();

        if( active_node)
            active_node->mouse_leave_event( leave_event_);
    }

    event->accept();
}

void image_viewer_strategy_t::key_press_event( QKeyEvent *event)
{
    switch( event->key())
    {
		case Qt::Key_Home:
		{
			viewport_.reset();
	
			if( image_.valid())
			{
				Imath::V2i p( image_.display_window().center());
				Imath::V2f q( p.x * aspect_ratio() * subsample(), p.y * subsample());
				viewport_.scroll_to_center_point( q);
			}
	
			update();
			event->accept();
		}
		break;
	
		case Qt::Key_Comma:
		{
			Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
			viewport_.zoom( p, 2.0f);
			update();
			event->accept();
		}
		break;
	
		case Qt::Key_Period:
		{
			Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
			viewport_.zoom( p, 0.5f);
			update();
			event->accept();
		}
		break;
	
		default:
		{
			key_press_event_.view = this;
			key_press_event_.aspect_ratio = aspect_ratio();
			key_press_event_.pixel_scale = pixel_scale();
			key_press_event_.subsample = subsample();
			key_press_event_.modifiers = translate_modifiers( event);
			key_press_event_.key = translate_keys( event);
	
			if( overlay_)
			{
				node_t *active_node  = app().ui()->active_node();
				event_accepted_by_node_ = false;
	
				if( active_node)
					event_accepted_by_node_ = active_node->key_press_event( key_press_event_);
			}
	
			if( event_accepted_by_node_)
				event->accept();
			else
				event->ignore();
		}
    }
}

void image_viewer_strategy_t::key_release_event( QKeyEvent *event)
{
    switch( event->key())
    {
		case Qt::Key_Home:
		case Qt::Key_Comma:
		case Qt::Key_Period:
			event->accept();
		break;
	
		default:
		{
			key_release_event_.view = this;
			key_release_event_.aspect_ratio = aspect_ratio();
			key_release_event_.pixel_scale = pixel_scale();
			key_release_event_.subsample = subsample();
			key_release_event_.modifiers = translate_modifiers( event);
			key_release_event_.key = translate_keys( event);

			if( overlay_)
			{
				node_t *active_node  = app().ui()->active_node();
	
				if( active_node && event_accepted_by_node_)
					active_node->key_release_event( key_release_event_);
			}
	
			if( event_accepted_by_node_)
				event->accept();
			else
				event->ignore();
		}
    }
}

void image_viewer_strategy_t::mouse_press_event( QMouseEvent *event)
{
	if(  event->button() == Qt::LeftButton)
	{
		Imath::V2i p( event->x(), event->y());
		Imath::V2f q( screen_to_world( p));
	
		press_event_.view = this;
		press_event_.aspect_ratio = aspect_ratio();
		press_event_.pixel_scale = pixel_scale();
		press_event_.subsample = subsample();
		press_event_.pos = p;
		press_event_.wpos = q;
		press_event_.modifiers = translate_modifiers( event);
		press_event_.button = translate_mouse_buttons( event);
	
		drag_event_.first_drag = true;
		drag_event_.click_pos  = p;
		drag_event_.last_pos = p;
		drag_event_.click_wpos = q;
		drag_event_.last_wpos = q;
		drag_event_.modifiers = press_event_.modifiers;
		drag_event_.button = press_event_.button;
	
		release_event_.dragged = false;
		release_event_.click_pos = p;
		release_event_.pos = p;
		release_event_.click_wpos = q;
		release_event_.wpos = q;
		release_event_.modifiers = press_event_.modifiers;
		release_event_.button = press_event_.button;
	
		scroll_mode_ = false;
		zoom_mode_ = false;
	
		if( event->modifiers() & Qt::AltModifier)
		{
			if( event->modifiers() & Qt::ShiftModifier)
			{
				zoom_mode_ = true;
				zoom_center_ = press_event_.wpos;
			}
			else
				scroll_mode_ = true;
			
			event->accept();
			return;
		}
		else
		{
			if( overlay_)
			{
				node_t *active_node  = app().ui()->active_node();
				event_accepted_by_node_ = false;
	
				if( active_node)
					event_accepted_by_node_ = active_node->mouse_press_event( press_event_);
	
				if( event_accepted_by_node_)
				{
					app().ui()->begin_interaction();
					event->accept();
					return;
				}
			}
		}		
	}

	event->ignore();
}

void image_viewer_strategy_t::mouse_move_event( QMouseEvent *event)
{
    if( event->buttons() == Qt::NoButton)
    {
		Imath::V2f pw( screen_to_world( Imath::V2i( event->x(), event->y())));
		Imath::V2i p( pw.x / aspect_ratio() / subsample(),
					  pw.y / subsample());
		
		Imath::Color4f col( 0, 0, 0, 0);
		boost::optional<Imath::Color4f> ocol( image_.color_at( p));

		if( ocol)
			col = ocol.get();

        std::stringstream s;
        s << "X = " << (int) pw.x << " Y = " << (int) pw.y;
        s << " Color = " << col.r << ", " << col.g << ", " << col.b << ", " << col.a;

        app().ui()->viewer().set_status( s.str());

        if( parent()->view_mode() == viewer_context_t::view_active_node)
        {
			move_event_.view = this;
			move_event_.aspect_ratio = aspect_ratio();
			move_event_.pixel_scale = pixel_scale();
			move_event_.subsample = subsample();
			move_event_.pos = Imath::V2i( event->x(), event->y());
			move_event_.wpos = pw;
			move_event_.modifiers = translate_modifiers( event);

            node_t *active_node  = app().ui()->active_node();

            if( active_node)
                active_node->mouse_move_event( move_event_);
        }

        event->accept();
    }
	else
	{
		if( ( event->x() != drag_event_.last_pos.x) || ( event->y() != drag_event_.last_pos.y))
		{
			drag_event_.view = this;
			drag_event_.aspect_ratio = aspect_ratio();
			drag_event_.pixel_scale = pixel_scale();
			drag_event_.subsample = subsample();
			drag_event_.pos = Imath::V2i( event->x(), event->y());
			drag_event_.wpos = screen_to_world( drag_event_.pos);
			drag_event_.modifiers = translate_modifiers( event);
			drag_event_.button = translate_mouse_buttons( event);
			
			if( scroll_mode_)
			{
				viewport_.scroll( -( drag_event_.pos - drag_event_.last_pos));
				event->accept();
				update();
			}
			else
			{
				if( zoom_mode_)
				{
					const float zoom_speed = 0.05f;
					float zoom = 1.0f + ( zoom_speed * ( drag_event_.pos.x - drag_event_.last_pos.x));
					viewport_.zoom( zoom_center_, zoom, zoom);
					event->accept();
					update();
				}
				else
				{
					if( overlay_)
					{
						node_t *active_node  = app().ui()->active_node();
	
						if( active_node && event_accepted_by_node_)
						{
							active_node->mouse_drag_event( drag_event_);
							event->accept();
						}
						else
							event->ignore();
					}
				}
			}
	
			if( drag_event_.first_drag)
			{
				drag_event_.first_drag = false;
				release_event_.dragged = true;
			}
	
			drag_event_.last_pos = drag_event_.pos;
			drag_event_.last_wpos = drag_event_.wpos;			
		}
    }
}

void image_viewer_strategy_t::mouse_release_event( QMouseEvent *event)
{
	release_event_.view = this;
	release_event_.aspect_ratio = aspect_ratio();
	release_event_.pixel_scale = pixel_scale();
	release_event_.subsample = subsample();
    release_event_.pos = Imath::V2i( event->x(), event->y());
    release_event_.wpos = screen_to_world( release_event_.pos);
    release_event_.modifiers = translate_modifiers( event);
    release_event_.button = translate_mouse_buttons( event);

	if( scroll_mode_ || zoom_mode_)
		event->accept();
	else
	{
	    if( overlay_)
	    {
	        node_t *active_node  = app().ui()->active_node();

	        if( active_node && event_accepted_by_node_)
	        {
	            active_node->mouse_release_event( release_event_);
	            app().ui()->end_interaction();
				event->accept();
	        }
			else
				event->ignore();
	    }
	}

    scroll_mode_ = false;
    zoom_mode_ = false;
    event_accepted_by_node_ = false;
}

Imath::Color4f image_viewer_strategy_t::color_at( int x, int y) const
{
	Imath::V2i pos( x, y);
	Imath::V2f wpos = screen_to_world( pos);
	Imath::V2i p( wpos.x / aspect_ratio() / subsample(), wpos.y / subsample());
	
	boost::optional<Imath::Color4f> c( image_.color_at( p));
	
	if( c)
		return c.get();

	return Imath::Color4f( 0, 0, 0, 0);
}

void image_viewer_strategy_t::pick_colors_in_box( const Imath::Box2i& b, boost::function<void ( const Imath::Color4f&)> f) const
{
	Imath::Box2i xbox;
	Imath::V2f wpos = screen_to_world( b.min);
	Imath::V2i p( wpos.x / aspect_ratio() / subsample(), wpos.y / subsample());
	xbox.min = p;

	wpos = screen_to_world( b.max);
	p = Imath::V2i( wpos.x / aspect_ratio() / subsample(), wpos.y / subsample());
	xbox.max = p;
	
	for( p.y = b.min.y; p.y <= b.max.y; ++p.y)
	{
		for( p.x = b.min.x; p.x <= b.max.x; ++p.x)
		{
			boost::optional<Imath::Color4f> c = image_.color_at( p);
			
			if( c)
				f( c.get());
		}
	}
}

} // viewer
} // ui
} // ramen
