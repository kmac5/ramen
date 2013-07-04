// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/color/ocio_colorspace_node.hpp>

#include<ramen/app/application.hpp>

#include<ramen/params/ocio_colorspace_param.hpp>

#include<ramen/ocio/manager.hpp>
#include<ramen/image/ocio_transform.hpp>

#ifndef NDEBUG
	#include<iostream>
#endif

namespace ramen
{
namespace image
{

ocio_colorspace_node_t::ocio_colorspace_node_t() : pointop_node_t()
{
    set_name( "ocio_cspace");
}

void ocio_colorspace_node_t::do_create_params()
{
    std::auto_ptr<ocio_colorspace_param_t> p( new ocio_colorspace_param_t( "In Colorspace"));
    p->set_id( "in_colorspace");
    add_param( p);

    p.reset( new ocio_colorspace_param_t( "Out Colorspace"));
    p->set_id( "out_colorspace");
    add_param( p);
}

void ocio_colorspace_node_t::do_process( const image::const_image_view_t& src,
                                         const image::image_view_t& dst,
                                         const render::context_t& context)
{
    boost::gil::copy_pixels( src, dst);

	std::string in_cs = get_value<std::string>( param( "in_colorspace"));
	std::string out_cs = get_value<std::string>( param( "out_colorspace"));

	try
	{
	    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
        OCIO::ConstContextRcPtr context = config->getCurrentContext();
        OCIO::ConstProcessorRcPtr proc = config->getProcessor( context,
                                                               in_cs.c_str(),
                                                               out_cs.c_str());
		image::ocio_transform( dst, proc);
	}
	catch( OCIO::Exception& exception)
	{
		#ifndef NDEBUG
			std::cout << "Exception while converting colorspaces: in = " << in_cs << ", out = " << out_cs << "\n";
		#endif
	}
}

// factory
node_t *create_ocio_colorspace_node()
{
    return new ocio_colorspace_node_t();
}

const node_metaclass_t *ocio_colorspace_node_t::metaclass() const
{
    return &ocio_colorspace_node_metaclass();
}

const node_metaclass_t& ocio_colorspace_node_t::ocio_colorspace_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.ocio_colorspace";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "OCIO Colorspace";
        info.help = "Converts between color spaces using OpenColorIO.";
        info.create = &create_ocio_colorspace_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( ocio_colorspace_node_t::ocio_colorspace_node_metaclass());

} // namespace
} // namespace
