// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ui/viewer/image_view/tiled_image_strategy.hpp>

#include<algorithm>
#include<iostream>

#include<boost/bind.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

tiled_image_strategy_t::tile_t::tile_t( const image::buffer_t& pixels, const Imath::Box2i& area) : texture_id_( 0)
{
	RAMEN_ASSERT( !area.isEmpty());

	area_ = area;
	alloc_tile( area_.size().x + 1, area_.size().y + 1);
	char *ptr = pixel_ptr( pixels, area_.min.x, area_.max.y);
	std::size_t rowsize = rowbytes( pixels);
	update_texture( area_, ptr, rowsize);
}

tiled_image_strategy_t::tile_t::~tile_t() { gl_delete_texture( &texture_id_);}

void tiled_image_strategy_t::tile_t::alloc_tile( int width, int height)
{
	RAMEN_ASSERT( texture_id_ == 0);

	texture_id_ = gl_gen_texture();
	gl_bind_texture( GL_TEXTURE_2D, texture_id_);

	gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	gl_tex_image2d( GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
}

void tiled_image_strategy_t::tile_t::update_texture( const Imath::Box2i& area, char *ptr, std::size_t rowsize)
{
	RAMEN_ASSERT( texture_id_ != 0);
	RAMEN_ASSERT( area_.size() == area.size());

	area_ = area;
	char *p = ptr;
	int width = area_.size().x + 1;
	int j = 0;

	gl_bind_texture( GL_TEXTURE_2D, texture_id_);
	gl_pixel_storei(GL_UNPACK_ALIGNMENT, 1);
	gl_pixel_storei( GL_UNPACK_ROW_LENGTH, rowsize / sizeof( image::pixel_t));

	for( int y = area_.max.y; y >= area_.min.y; --y)
	{
		gl_tex_subimage2d( GL_TEXTURE_2D, 0, 0, j, width, 1, GL_RGBA, GL_FLOAT, p);
		p -= rowsize;
		++j;
	}
}

char *tiled_image_strategy_t::tile_t::pixel_ptr( const image::buffer_t& pixels, int x, int y)
{
	RAMEN_ASSERT( y >= pixels.bounds().min.y && y <= pixels.bounds().max.y);
	RAMEN_ASSERT( x >= pixels.bounds().min.x && x <= pixels.bounds().max.x);
	return ( char *) &( pixels.const_rgba_view()( x - pixels.bounds().min.x, y - pixels.bounds().min.y)[0]);
}

std::size_t tiled_image_strategy_t::tile_t::rowbytes( const image::buffer_t& pixels)
{
	return pixels.const_rgba_view().pixels().row_size();
}

void tiled_image_strategy_t::tile_t::draw() const
{
	gl_bind_texture( GL_TEXTURE_2D, texture_id_);
	gl_textured_quad( area_.min.x, area_.min.y, area_.max.x + 1, area_.max.y + 1);
}

tiled_image_strategy_t::tiled_image_strategy_t( const image::buffer_t& pixels,
												const Imath::Box2i& display_window,
												const Imath::Box2i& data_window,
												GLenum texture_unit) : image_strategy_t( display_window, data_window, texture_unit)
{
	RAMEN_ASSERT( !data_window.isEmpty());
	RAMEN_ASSERT( !display_window.isEmpty());
	
	pixels_ = pixels;

	int xtiles = std::ceil( ( double) ( data_window.size().x + 1) / tile_size());
	int ytiles = std::ceil( ( double) ( data_window.size().y + 1) / tile_size());

	for( int j = 0; j < ytiles; ++j)
	{
		for( int i = 0; i < xtiles; ++i)
		{
			Imath::Box2i area( area_for_tile( i, j));
			tile_t *tile = new tile_t( pixels_, area);
			tiles_.push_back( tile);
		}
	}

	gl_finish();
	gl_bind_texture( GL_TEXTURE_2D, 0);
}

bool tiled_image_strategy_t::update_pixels( const image::buffer_t& pixels, const Imath::Box2i& display_window, const Imath::Box2i& data_window)
{
	//if( pixels.width() == width() && pixels.height() == height())
	if( data_window.size().x + 1 == width() && data_window.size().y + 1 == height())
	{
		pixels_ = pixels;
		data_window_ = data_window;
		display_window_ = display_window;

		int xtiles = std::ceil( ( double) ( data_window_.size().x + 1) / tile_size());
		int ytiles = std::ceil( ( double) ( data_window_.size().y + 1) / tile_size());
		RAMEN_ASSERT( tiles_.size() == ( xtiles * ytiles));

		std::size_t rowsize = tile_t::rowbytes( pixels_);

		int index = 0;
		for( int j = 0; j < ytiles; ++j)
		{
			for( int i = 0; i < xtiles; ++i)
			{
				Imath::Box2i area( area_for_tile( i, j));
				char *ptr = tile_t::pixel_ptr( pixels_, area.min.x, area.max.y);
				tiles_[index].update_texture( area, ptr, rowsize);
				++index;
			}
		}

		gl_finish();
		gl_bind_texture( GL_TEXTURE_2D, 0);
		return true;
	}

	return false;
}

void tiled_image_strategy_t::draw() const
{
	gl_active_texture( texture_unit_);
	gl_tex_envf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    boost::range::for_each( tiles_, boost::bind( &tile_t::draw, _1));
}

boost::optional<Imath::Color4f> tiled_image_strategy_t::color_at( const Imath::V2i &p) const
{
	if( p.x < data_window().min.x || p.x > data_window().max.x)
		return boost::optional<Imath::Color4f>();

	if( p.y < data_window().min.y || p.y > data_window().max.y)
		return boost::optional<Imath::Color4f>();

	image::pixel_t px( pixels_.const_rgba_view()( p.x - data_window().min.x, p.y - data_window().min.y));
	return Imath::Color4f( boost::gil::get_color( px, boost::gil::red_t()),
						   boost::gil::get_color( px, boost::gil::green_t()),
						   boost::gil::get_color( px, boost::gil::blue_t()),
						   boost::gil::get_color( px, boost::gil::alpha_t()));
}

int tiled_image_strategy_t::tile_size() { return 1024;}

Imath::Box2i tiled_image_strategy_t::area_for_tile( int x, int y) const
{
	Imath::Box2i area;
	area.min.x = ( x * tile_size()) + data_window().min.x;
	area.min.y = ( y * tile_size()) + data_window().min.y;
	area.max.x = std::min( area.min.x + tile_size() - 1, data_window().max.x);
	area.max.y = std::min( area.min.y + tile_size() - 1, data_window().max.y);
	return area;
}

} // viewer
} // ui
} // ramen
