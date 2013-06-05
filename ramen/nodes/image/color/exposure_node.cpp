// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/color/exposure_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct exposure_fun
{
    exposure_fun( float e) : m_( std::pow( 2, e)) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
		return image::pixel_t( src[0] * m_, src[1] * m_, src[2] * m_, src[3]);
    }

    float m_;
};

} // unnamed

exposure_node_t::exposure_node_t() : pointop_node_t()
{
    set_name("exposure");
    add_input_plug( "mask", true, ui::palette_t::instance().color( "matte plug"), "Mask");
}

void exposure_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Exp"));
    p->set_id( "exp");
    p->set_default_value( 0.0f);
    p->set_step( 0.1);
    add_param( p);
}

bool exposure_node_t::do_is_identity() const
{
	return get_value<float>( param( "exp")) == 0.0f;
}

void exposure_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    image::const_image_view_t src_view;
    image::image_view_t dst_view;
    Imath::Box2i area;

    if( input( 1))
    {
		boost::gil::copy_pixels( src, dst);
		area = ImathExt::intersect( input_as<image_node_t>( 1)->defined(), defined());

		if( area.isEmpty())
		    return;

		src_view = input_as<image_node_t>( 0)->const_subimage_view( area);
		dst_view = subimage_view( area);
    }
    else
    {
		src_view = src;
		dst_view = dst;
    }

    boost::gil::tbb_transform_pixels( src_view, dst_view, exposure_fun( get_value<float>( param( "exp"))));

    if( input(1))
        image::key_mix( src_view, dst_view, boost::gil::nth_channel_view( input_as<image_node_t>( 1)->const_subimage_view( area), 3), dst_view);
}

// node factory
node_t *create_exposure_node() { return new exposure_node_t();}

const node_metaclass_t *exposure_node_t::metaclass() const { return &exposure_node_metaclass();}

const node_metaclass_t& exposure_node_t::exposure_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.exposure";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Color";
        m.menu_item = "Exposure";
        m.create = &create_exposure_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( exposure_node_t::exposure_node_metaclass());

} // namespace
} // namespace
