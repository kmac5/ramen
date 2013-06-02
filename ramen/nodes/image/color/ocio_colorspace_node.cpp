// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/ocio_colorspace_node.hpp>

#include<ramen/app/application.hpp>

#include<ramen/params/ocio_colorspace_param.hpp>
#include<ramen/params/group_param.hpp>
#include<ramen/params/string_param.hpp>

#include<ramen/ocio/manager.hpp>
#include<ramen/image/ocio_transform.hpp>

#ifndef NDEBUG
	#include<iostream>
#endif

namespace ramen
{
namespace image
{

ocio_colorspace_node_t::ocio_colorspace_node_t() : pointop_node_t() { set_name( "ocio_cspace");}

void ocio_colorspace_node_t::do_create_params()
{
    std::auto_ptr<ocio_colorspace_param_t> p( new ocio_colorspace_param_t( "In Colorspace"));
    p->set_id( "in_colorspace");
    add_param( p);

    p.reset( new ocio_colorspace_param_t( "Out Colorspace"));
    p->set_id( "out_colorspace");
    add_param( p);
	
	std::auto_ptr<group_param_t> g( new group_param_t( "Context"));
	{
		std::auto_ptr<string_param_t> s( new string_param_t( "Key1"));
		s->set_id( "key1");
		s->set_include_in_hash( false);
		g->add_param( s);

		s.reset( new string_param_t( "Value1"));
		s->set_id( "value1");
		s->set_include_in_hash( false);
		g->add_param( s);
		
		s.reset( new string_param_t( "Key2"));
		s->set_id( "key2");
		s->set_include_in_hash( false);
		g->add_param( s);

		s.reset( new string_param_t( "Value2"));
		s->set_id( "value2");
		s->set_include_in_hash( false);
		g->add_param( s);
		
		s.reset( new string_param_t( "Key3"));
		s->set_id( "key3");
		s->set_include_in_hash( false);
		g->add_param( s);

		s.reset( new string_param_t( "Value3"));
		s->set_id( "value3");
		s->set_include_in_hash( false);
		g->add_param( s);
		
		s.reset( new string_param_t( "Key4"));
		s->set_id( "key4");
		s->set_include_in_hash( false);
		g->add_param( s);

		s.reset( new string_param_t( "Value4"));
		s->set_id( "value4");
		s->set_include_in_hash( false);
		g->add_param( s);		
	}
	add_param( g);
}

void ocio_colorspace_node_t::do_calc_hash_str( const render::context_t& context)
{
	pointop_node_t::do_calc_hash_str( context);
	
	try
	{
	    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
		OCIO::ConstContextRcPtr context = get_local_context();
		hash_generator() << config->getCacheID( context);
	}
	catch( OCIO::Exception &e)
	{
		hash_generator() << "error";
	}
}

void ocio_colorspace_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    boost::gil::copy_pixels( src, dst);

	std::string in_cs = get_value<std::string>( param( "in_colorspace"));
	std::string out_cs = get_value<std::string>( param( "out_colorspace"));

	try
	{
	    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
		OCIO::ConstContextRcPtr context = get_local_context();
	    OCIO::ConstProcessorRcPtr proc = config->getProcessor( context, in_cs.c_str(), out_cs.c_str());
		image::ocio_transform( dst, proc);
	}
	catch( OCIO::Exception& exception)
	{
		#ifndef NDEBUG
			std::cout << "Exception while converting colorspaces: in = " << in_cs << ", out = " << out_cs << "\n";
		#endif
	}
}

OCIO::ConstContextRcPtr ocio_colorspace_node_t::get_local_context()
{
	OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
	OCIO::ConstContextRcPtr context = config->getCurrentContext();
	
    OCIO::ContextRcPtr mutable_context;
	
	std::string key = get_value<std::string>( param( "key1"));
	
    if( !key.empty())
    {
        if( !mutable_context)
			mutable_context = context->createEditableCopy();

		std::string value = get_value<std::string>( param( "value1"));
        mutable_context->setStringVar( key.c_str(), value.c_str());		
    }
	
	key = get_value<std::string>( param( "key2"));
	
    if( !key.empty())
	{
		if( !mutable_context)
			mutable_context = context->createEditableCopy();
		
		std::string value = get_value<std::string>( param( "value2"));
        mutable_context->setStringVar( key.c_str(), value.c_str());		
    }

	key = get_value<std::string>( param( "key3"));
	
    if( !key.empty())
    {
		if( !mutable_context)
			mutable_context = context->createEditableCopy();
		
		std::string value = get_value<std::string>( param( "value3"));
        mutable_context->setStringVar( key.c_str(), value.c_str());		
    }
	
	key = get_value<std::string>( param( "key4"));
	
    if( !key.empty())		
    {
        if(!mutable_context)
			mutable_context = context->createEditableCopy();

		std::string value = get_value<std::string>( param( "value4"));
        mutable_context->setStringVar( key.c_str(), value.c_str());
    }
	
    if( mutable_context)
		context = mutable_context;
	
	return context;
}

// factory
node_t *create_ocio_colorspace_node() { return new ocio_colorspace_node_t();}

const node_metaclass_t *ocio_colorspace_node_t::metaclass() const { return &ocio_colorspace_node_metaclass();}

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
