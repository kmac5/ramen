// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_VIEWER_IMAGE_HPP
#define	RAMEN_UI_VIEWER_IMAGE_HPP

#include<memory>

#include<ramen/ui/viewer/image_view/image_strategy.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

class image_t
{
public:

    image_t( GLenum texture_unit = GL_TEXTURE0) : texture_unit_( texture_unit) {}

	bool valid() const;

    void reset();
    void reset( image::buffer_t pixels,
                const Imath::Box2i& display_window,
                const Imath::Box2i& data_window);

	Imath::Box2i display_window() const;
	Imath::Box2i data_window() const;

	// draw
	void draw_background() const;
	void draw() const;

	// overlay draw
	void frame_display_window() const;
	void frame_data_window() const;

	// get colors
	boost::optional<Imath::Color4f> color_at( const Imath::V2i& p) const;

private:

    void create_strategy( const image::buffer_t& pixels,
                          const Imath::Box2i& display_window,
                          const Imath::Box2i& data_window);

	void frame_rect( const Imath::Box2i& b) const;
	void gl_vertices_for_box( const Imath::Box2i& b) const;

	GLenum texture_unit_;
    std::auto_ptr<image_strategy_t> strategy_;	
};

} // viewer
} // ui
} // ramen

#endif
