// Copyright (c) 2010 Esteban Tovagliari



#include<ramen/ui/viewer/image_view/image.hpp>

// concrete strategies
#include<ramen/ui/viewer/image_view/tiled_image_strategy.hpp>
#include<ramen/ui/viewer/image_view/empty_image_strategy.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

bool image_t::valid() const
{
	if( !strategy_.get())
		return false;

	// test more things here...
	return true;
}

void image_t::reset() { strategy_.reset();}

void image_t::reset( image::buffer_t pixels, const Imath::Box2i& display_window, const Imath::Box2i& data_window)
{
	if( strategy_.get() && strategy_->update_pixels( pixels, display_window, data_window))
		return;

	create_strategy( pixels, display_window, data_window);
}

void image_t::create_strategy( const image::buffer_t& pixels, const Imath::Box2i& display_window, const Imath::Box2i& data_window)
{
	if( display_window.isEmpty())
	{
		reset();
		return;
	}
	
	if( data_window.isEmpty())
	{
		strategy_.reset( new empty_image_strategy_t( display_window));
		return;
	}

	// by default, create a tiled gl texture.
	strategy_.reset( new tiled_image_strategy_t( pixels, display_window, data_window));
}

Imath::Box2i image_t::display_window() const
{
	if( strategy_.get())
		return strategy_->display_window();

	return Imath::Box2i();
}

Imath::Box2i image_t::data_window() const
{
	if( strategy_.get())
		return strategy_->data_window();

	return Imath::Box2i();
}

void image_t::draw() const
{
	if( strategy_.get())
		strategy_->draw();
}

void image_t::draw_background() const
{
	if( strategy_.get())
	{
		const Imath::Box2i& dw( strategy_->display_window());

		if( !dw.isEmpty())
		{
			gl_begin( GL_QUADS);
				gl_vertices_for_box( dw);
			gl_end();
		}
	}
}

void image_t::frame_display_window() const
{
	if( strategy_.get())
		frame_rect( strategy_->display_window());
}

void image_t::frame_data_window() const
{
	if( strategy_.get())
		frame_rect( strategy_->data_window());
}

boost::optional<Imath::Color4f> image_t::color_at( const Imath::V2i& p) const
{
	if( strategy_.get())
		return strategy_->color_at( p);

	return boost::optional<Imath::Color4f>();
}

// draw utils
void image_t::frame_rect( const Imath::Box2i& b) const
{
	if( !b.isEmpty())
	{
		gl_begin( GL_LINE_LOOP);
		gl_vertices_for_box( b);
		gl_end();
	}
}

void image_t::gl_vertices_for_box( const Imath::Box2i& b) const
{
	gl_vertex2i( b.min.x		, b.min.y);
	gl_vertex2i( b.max.x + 1	, b.min.y);
	gl_vertex2i( b.max.x + 1	, b.max.y + 1);
	gl_vertex2i( b.min.x		, b.max.y + 1);
}

} // viewer
} // ui
} // ramen
