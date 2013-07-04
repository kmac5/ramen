// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_VIEWER_IMAGE_STRATEGY_HPP
#define	RAMEN_UI_VIEWER_IMAGE_STRATEGY_HPP

#include<OpenEXR/ImathColor.h>

#include<boost/noncopyable.hpp>
#include<boost/optional.hpp>

#include<ramen/GL/gl.hpp>

#include<ramen/image/buffer.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

class image_strategy_t : boost::noncopyable
{
public:

    image_strategy_t( const Imath::Box2i& display_window,
                      const Imath::Box2i& data_window,
                      GLenum texture_unit = GL_TEXTURE0);

    virtual ~image_strategy_t();

	int width()  const;
	int height() const;

    const Imath::Box2i& display_window() const	{ return display_window_;}
    const Imath::Box2i& data_window() const		{ return data_window_;}

    virtual bool update_pixels( const image::buffer_t& pixels,
                                const Imath::Box2i& display_window,
                                const Imath::Box2i& data_window);

	virtual void draw() const = 0;

	virtual boost::optional<Imath::Color4f> color_at( const Imath::V2i& p) const = 0;

protected:

	GLenum texture_unit_;
	Imath::Box2i display_window_, data_window_;
};

} // viewer
} // ui
} // ramen

#endif
