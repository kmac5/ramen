// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/generator_node.hpp>

#include<ramen/params/image_format_param.hpp>

namespace ramen
{
namespace image
{

generator_node_t::generator_node_t() : image_node_t() { add_output_plug();}

generator_node_t::generator_node_t( const generator_node_t& other) : image_node_t( other) {}

void generator_node_t::do_create_params()
{
    std::auto_ptr<image_format_param_t> p( new image_format_param_t( "Format"));
    p->set_id( "format");
    add_param( p);
}

void generator_node_t::do_calc_format( const render::context_t& context)
{
	image::format_t f( get_value<image::format_t>( param( "format")));
    Imath::Box2i area( f.area());
    --area.max.x;
    --area.max.y;
    set_format( area);
	set_aspect_ratio( f.aspect);
	set_proxy_scale( Imath::V2f( 1, 1));
}

void generator_node_t::do_calc_bounds( const render::context_t& context) { set_bounds( format());}

void generator_node_t::do_calc_defined( const render::context_t& context)
{
	//if( crop_to_format())
	    set_defined( ImathExt::intersect( bounds(), interest()));
	//else
		//set_defined( interest()); // infinite images
}

void generator_node_t::do_calc_hash_str( const render::context_t& context)
{
	image_node_t::do_calc_hash_str( context);
	hash_generator() << context.subsample;
}

} // namespace
} // namespace
