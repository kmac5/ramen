// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_NODE_RENDERER_HPP
#define	RAMEN_IMAGE_NODE_RENDERER_HPP

#include<boost/noncopyable.hpp>

#include<ramen/nodes/node_fwd.hpp>
#include<ramen/image/buffer.hpp>

#include<ramen/render/context.hpp>

namespace ramen
{
namespace render
{

class RAMEN_API image_node_renderer_t : boost::noncopyable
{
public:

	image_node_renderer_t();
    explicit image_node_renderer_t( const context_t& new_context);

    ~image_node_renderer_t();

	void set_context( const context_t& context);

    const Imath::Box2i& format() const;
    const Imath::Box2i& bounds() const;

    void render();
    void render( const Imath::Box2i& roi);

    image::buffer_t image();
    image::const_image_view_t format_image_view() const;

private:

    context_t new_context_;
	bool has_context_;
    image_node_t *n_;
	bool render_done_;

public:

    static bool do_log;
};

} // namespace
} // namespace

#endif
