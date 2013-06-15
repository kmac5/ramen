// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/areaop_node.hpp>

#include<cmath>

namespace ramen
{
namespace image
{

areaop_node_t::areaop_node_t() : image_node_t()
{
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_output_plug();
}

areaop_node_t::areaop_node_t( const areaop_node_t& other) : image_node_t( other) {}

bool areaop_node_t::expand_defined() const { return false;}

void areaop_node_t::do_calc_bounds( const render::context_t& context)
{
    get_expand_radius( hradius_, vradius_);
    Imath::Box2i bounds( input_as<image_node_t>()->bounds());

    if( expand_defined())
    {
		bounds.min.x -= hradius_; bounds.max.x += hradius_;
		bounds.min.y -= vradius_; bounds.max.y += vradius_;
    }

    set_bounds( bounds);
}

void areaop_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi = interest();
    roi.min.x -= hradius_; roi.min.y -= vradius_;
    roi.max.x += hradius_; roi.max.y += vradius_;
    input_as<image_node_t>()->add_interest( roi);
}

void areaop_node_t::do_calc_defined( const render::context_t& context)
{
    Imath::Box2i roi = interest();
	
	if( expand_defined())
	{
		roi.min.x -= hradius_; roi.min.y -= vradius_;
		roi.max.x += hradius_; roi.max.y += vradius_;		
	}

	set_defined( ImathExt::intersect( bounds(), roi));
}

void areaop_node_t::get_expand_radius( int& hradius, int& vradius) const
{
	hradius = vradius = 0;
}

} // image
} // ramen
