// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_VIEWER_TILED_IMAGE_STRATEGY_HPP
#define	RAMEN_UI_VIEWER_TILED_IMAGE_STRATEGY_HPP

#include<ramen/ui/viewer/image_view/image_strategy.hpp>

#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

class tiled_image_strategy_t : public image_strategy_t
{
public:

	tiled_image_strategy_t( const image::buffer_t& pixels, const Imath::Box2i& display_window,
							const Imath::Box2i& data_window, GLenum texture_unit = GL_TEXTURE0);

	virtual bool update_pixels( const image::buffer_t& pixels, const Imath::Box2i& display_window, const Imath::Box2i& data_window);

	virtual void draw() const;

	virtual boost::optional<Imath::Color4f> color_at( const Imath::V2i& p) const;

private:

	static int tile_size();

	Imath::Box2i area_for_tile( int x, int y) const;

	struct tile_t : boost::noncopyable
	{
	public:

		tile_t( const image::buffer_t& pixels, const Imath::Box2i& area);
		~tile_t();

		void update_texture( const Imath::Box2i& area, char *ptr, std::size_t rowsize);

		void draw() const;

		static char *pixel_ptr( const image::buffer_t& pixels, int x, int y);
		static std::size_t rowbytes( const image::buffer_t& pixels);

	private:

		void alloc_tile( int width, int height);

		GLuint texture_id_;
		Imath::Box2i area_;
	};

	image::buffer_t pixels_;
	boost::ptr_vector<tile_t> tiles_;
};

} // viewer
} // ui
} // ramen

#endif
