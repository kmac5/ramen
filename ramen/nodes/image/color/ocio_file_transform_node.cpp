// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/color/ocio_file_transform_node.hpp>

#include<ramen/app/application.hpp>

#include<ramen/params/file_param.hpp>
#include<ramen/params/string_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/image/ocio_transform.hpp>

#include<iostream>

namespace ramen
{
namespace image
{

ocio_file_transform_node_t::ocio_file_transform_node_t() : pointop_node_t()
{
    set_name( "ocio_file");
}

void ocio_file_transform_node_t::do_create_params()
{
	std::auto_ptr<file_param_t> file( new file_param_t( "File"));
	file->set_id( "file");
	file->set_extension_list_string( "Transform files (*.3dl *.cc *.ccc *.csp *.cub *.cube *.lut *.spi1d *.spi3d *.spimtx)");
	add_param( file);
	
	std::auto_ptr<string_param_t> cccid( new string_param_t( "CCC Id"));
	cccid->set_id( "cccid");
	add_param( cccid);
	
	std::auto_ptr<popup_param_t> pop( new popup_param_t( "Direction"));
	pop->set_id( "direction");
	pop->menu_items() = boost::assign::list_of( "Forward")( "Inverse");
	add_param( pop);

	pop.reset( new popup_param_t( "Interpolation"));
	pop->set_id( "interpolation");
	pop->menu_items() = boost::assign::list_of( "Nearest")( "Linear");
	add_param( pop);
}

bool ocio_file_transform_node_t::do_is_valid() const
{
	const file_param_t *p = dynamic_cast<const file_param_t*>( &param( "file"));
	RAMEN_ASSERT( p);
	return p->file_exists();
}

void ocio_file_transform_node_t::do_process( const image::const_image_view_t& src,
                                             const image::image_view_t& dst,
                                             const render::context_t& context)
{
    boost::gil::copy_pixels( src, dst);

	boost::filesystem::path path( get_value<boost::filesystem::path>( param( "file")));
	std::string cccid( get_value<std::string>( param( "cccid")));

    try
    {
	    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

        OCIO::FileTransformRcPtr transform = OCIO::FileTransform::Create();
        transform->setSrc( filesystem::file_cstring( path));
        transform->setCCCId( cccid.c_str());

		if( get_value<int>( param( "direction")) == 0)
			transform->setDirection(OCIO::TRANSFORM_DIR_FORWARD);
        else 
			transform->setDirection(OCIO::TRANSFORM_DIR_INVERSE);

		if( get_value<int>( param( "interpolation")) == 0)
	        transform->setInterpolation(OCIO::INTERP_NEAREST);
        else
			transform->setInterpolation(OCIO::INTERP_LINEAR);
        
        OCIO::ConstProcessorRcPtr proc = config->getProcessor( transform,
                                                               OCIO::TRANSFORM_DIR_FORWARD);
		image::ocio_transform( dst, proc);
    }
    catch(OCIO::Exception &e)
    {
		#ifndef NDEBUG
			std::cout << "Exception while converting colorspaces.\n";
		#endif
    }
}

// factory
node_t *create_ocio_file_transform_node()
{
    return new ocio_file_transform_node_t();
}

const node_metaclass_t *ocio_file_transform_node_t::metaclass() const
{
    return &ocio_file_transform_node_metaclass();
}

const node_metaclass_t& ocio_file_transform_node_t::ocio_file_transform_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.ocio_file_transform";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "OCIO File Transform";
        info.help = "Uses OpenColorIO to apply a specified Lut transform.";
        info.create = &create_ocio_file_transform_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( ocio_file_transform_node_t::ocio_file_transform_node_metaclass());

} // image
} // ramen
