// Copyright (c) 2010 Esteban Tovagliari.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/base_output_node.hpp>

#include<ramen/image/ocio_transform.hpp>

#include<ramen/params/ocio_colorspace_param.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/render/image_node_renderer.hpp>

namespace ramen
{
namespace image
{

base_output_node_t::base_output_node_t() : image_node_t()
{
    add_input_plug( "Front", false, ui::palette_t::instance().color( "front plug"), "Front");
}

base_output_node_t::base_output_node_t( const base_output_node_t& other) : image_node_t( other) {}

void base_output_node_t::do_process( const render::context_t& context)
{
    input_defined_ = input_as<image_node_t>()->defined();

    Imath::Box2i area( ImathExt::intersect( input_defined_, defined()));

    if( !area.isEmpty())
	{
        boost::gil::copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));
		
		try
		{
			if( context.cancel())
				return;

			if( context.mode == render::process_render)
			{
				std::string cs( get_value<std::string>( param( "colorspace")));

				OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
				OCIO::ConstProcessorRcPtr proc = config->getProcessor( OCIO::ROLE_SCENE_LINEAR,
																		get_value<std::string>( param( "colorspace")).c_str());
				image::ocio_transform( image_view(), proc);
			}
		}
		catch( OCIO::Exception& exception)
		{
			// TODO: do something here!!
		}
	}
}

void base_output_node_t::process_and_write( const render::context_t& context)
{
	render::image_node_renderer_t r( context);
	r.render();
	write( context);
}

} // namespace
} // namespace
