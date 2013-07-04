// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/color/cdl_node.hpp>

#include<iterator>
#include<algorithm>
#include<iomanip>

#include<boost/bind.hpp>
#include<boost/filesystem/fstream.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float3_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/string_param.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ocio/manager.hpp>
#include<ramen/image/ocio_transform.hpp>

#include<iostream>

namespace ramen
{
namespace image
{

cdl_node_t::cdl_node_t() : pointop_node_t()
{
    set_name("cdl");
}

cdl_node_t::cdl_node_t( const cdl_node_t& other) : pointop_node_t( other) {}

void cdl_node_t::do_create_params()
{
	std::auto_ptr<string_param_t> cccid( new string_param_t( "CCC Id"));
	cccid->set_id( "cccid");
	add_param( cccid);
	
	std::auto_ptr<float3_param_t> f3( new float3_param_t( "Slope"));
	f3->set_id( "slope");
	f3->set_default_value( Imath::V3f( 1, 1, 1));
	f3->set_min( 0);
	f3->set_step( 0.025);
	f3->set_static( true);
	f3->set_proportional( true);
	add_param( f3);

	f3.reset( new float3_param_t( "Offset"));
	f3->set_id( "offset");
	f3->set_default_value( Imath::V3f( 0, 0, 0));
	f3->set_step( 0.025);
	f3->set_static( true);
	f3->set_proportional( true);
	add_param( f3);

	f3.reset( new float3_param_t( "Power"));
	f3->set_id( "power");
	f3->set_default_value( Imath::V3f( 1, 1, 1));
	f3->set_step( 0.05);
	f3->set_min( 0.025);
	f3->set_static( true);
	f3->set_proportional( true);
	add_param( f3);

	std::auto_ptr<float_param_t> f( new float_param_t( "Saturation"));
	f->set_id( "saturation");
	f->set_default_value( 1);
	f->set_min( 0);
	f->set_step( 0.025);
	f->set_static( true);
	add_param( f);

	std::auto_ptr<bool_param_t> b( new bool_param_t( "Reverse"));
	b->set_id( "reverse");
	add_param( b);
}

void cdl_node_t::do_process( const image::const_image_view_t& src,
                             const image::image_view_t& dst,
                             const render::context_t& context)
{
	boost::gil::copy_pixels( src, dst);

	try
	{
		OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
		OCIO::CDLTransformRcPtr transform = cdl_transform();
        OCIO::ConstProcessorRcPtr proc;
		
		if( get_value<bool>( param( "reverse")))
			proc = config->getProcessor( transform, OCIO::TRANSFORM_DIR_INVERSE);
		else
			proc = config->getProcessor( transform, OCIO::TRANSFORM_DIR_FORWARD);
		
		image::ocio_transform( dst, proc);
	}
    catch(OCIO::Exception& e)
    {
		#ifndef NDEBUG
			std::cout << "Exception while converting colorspaces.\n";
		#endif
    }	
}

OCIO::CDLTransformRcPtr cdl_node_t::cdl_transform() const
{
	Imath::V3f slope = get_value<Imath::V3f>( param( "slope"));
	Imath::V3f offset = get_value<Imath::V3f>( param( "offset"));
	Imath::V3f power = get_value<Imath::V3f>( param( "power"));

	OCIO::CDLTransformRcPtr transform = OCIO::CDLTransform::Create();
	transform->setSlope( reinterpret_cast<const float*>( &slope));
	transform->setOffset( reinterpret_cast<const float*>( &offset));
	transform->setPower( reinterpret_cast<const float*>( &power));
	transform->setSat( get_value<float>( param( "saturation")));
	transform->setID( get_value<std::string>( param( "cccid")).c_str());
	return transform;
}

void cdl_node_t::read_from_file( const boost::filesystem::path& p)
{
	boost::filesystem::ifstream ifs( p);

	if( !ifs.is_open() || !ifs.good())
		throw std::runtime_error( "Can't read file " + filesystem::file_string( p));
	
	ifs >> std::noskipws;
	
	std::string contents;
    std::copy( std::istream_iterator<char>( ifs),
               std::istream_iterator<char>(),
               std::back_inserter( contents));
	
	OCIO::CDLTransformRcPtr transform = OCIO::CDLTransform::Create();
	transform->setXML( contents.c_str());
	
	Imath::V3f slope;
	transform->getSlope( reinterpret_cast<float*>( &slope));
	
	Imath::V3f offset;
	transform->getOffset( reinterpret_cast<float*>( &offset));

	Imath::V3f power;
	transform->getPower( reinterpret_cast<float*>( &power));

	float sat = transform->getSat();
	std::string id( transform->getID());
	
	float3_param_t *p3 = dynamic_cast<float3_param_t*>( &param( "slope"));
	RAMEN_ASSERT( p3);
	p3->set_value( slope);

	p3 = dynamic_cast<float3_param_t*>( &param( "offset"));
	RAMEN_ASSERT( p3);
	p3->set_value( offset);

	p3 = dynamic_cast<float3_param_t*>( &param( "power"));
	RAMEN_ASSERT( p3);
	p3->set_value( power);

	float_param_t *p1 = dynamic_cast<float_param_t*>( &param( "saturation"));
	RAMEN_ASSERT( p1);
	p1->set_value( sat);
	
	string_param_t *s = dynamic_cast<string_param_t*>( &param( "cccid"));
	RAMEN_ASSERT( s);
	s->set_value( id);
}

void cdl_node_t::write_to_file( const boost::filesystem::path& p) const
{
	OCIO::CDLTransformRcPtr transform = cdl_transform();
	
	boost::filesystem::ofstream ofs( p);

	if( !ofs.is_open() || !ofs.good())
        throw core::runtime_error( "Can't write to file");

	ofs << transform->getXML();
	std::flush( ofs);
	ofs.close();
}

// factory
node_t *create_cdl_node() { return new cdl_node_t();}

const node_metaclass_t *cdl_node_t::metaclass() const
{
    return &cdl_node_metaclass();
}

const node_metaclass_t& cdl_node_t::cdl_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.cdl";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "CDL";
        info.create = &create_cdl_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( cdl_node_t::cdl_node_metaclass());

} // image
} // ramen
