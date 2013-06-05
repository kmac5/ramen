// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/keyer_node.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/render/image_node_renderer.hpp>

namespace ramen
{
namespace image
{

keyer_node_t::keyer_node_t( bool add_mask_input) : image_node_t()
{
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_output_plug();
	
	if( add_mask_input)
		add_input_plug( "mask", true, ui::palette_t::instance().color( "matte plug"), "Mask");
}

keyer_node_t::keyer_node_t( const keyer_node_t& other) : image_node_t( other) {}

void keyer_node_t::do_calc_bounds( const render::context_t& context)
{
	image_node_t *in0 = input_as<image_node_t>( 0);
	
    if( input( 1))
		set_bounds( ImathExt::intersect( in0->bounds(), input_as<image_node_t>( 1)->bounds()));
    else
		set_bounds( in0->bounds());
}

void keyer_node_t::do_calc_defined( const render::context_t& context)
{
	image_node_t *in0 = input_as<image_node_t>( 0);

    if( input( 1))
    {
		Imath::Box2i def( ImathExt::intersect( in0->defined(), input_as<image_node_t>( 1)->defined()));
		set_defined( def);
    }
    else
		set_defined( in0->defined());
}

void keyer_node_t::get_input_frame()
{
	image_node_t *in0 = input_as<image_node_t>( 0);
	
	// Render the input
	if( in0)
	{
		render::context_t context = composition()->current_context();
		context.result_node = in0;
		render::image_node_renderer_t r( context);
		r.render();

        input_data_window_ = ImathExt::intersect( in0->format(), in0->defined());
		
		if( !input_data_window_.isEmpty())
			input_pixels_ = in0->image();
	}
}

void keyer_node_t::free_input_frame()
{
	input_data_window_ = Imath::Box2i();
	input_pixels_.clear();
}

boost::optional<Imath::Color3f> keyer_node_t::sample_input( const Imath::V2i& p) const
{
	if( input_pixels_.empty())
		return boost::optional<Imath::Color3f>();
	
	if( p.x < input_data_window_.min.x || p.x > input_data_window_.max.x)
		return boost::optional<Imath::Color3f>();

	if( p.y < input_data_window_.min.y || p.y > input_data_window_.max.y)
		return boost::optional<Imath::Color3f>();

	image::pixel_t px( input_pixels_.const_rgba_view()( p.x - input_data_window_.min.x, p.y - input_data_window_.min.y));
	return Imath::Color3f( boost::gil::get_color( px, boost::gil::red_t()),
						   boost::gil::get_color( px, boost::gil::green_t()),
						   boost::gil::get_color( px, boost::gil::blue_t()));
}

void keyer_node_t::sample_input( const Imath::Box2i& area, std::vector<Imath::Color3f>& colors) const
{
	if( input_pixels_.empty())
		return;

	Imath::Box2i subarea = ImathExt::intersect( area, input_data_window_);

	if( subarea.isEmpty())
		return;

	Imath::V2i p;
	
	for( p.y = subarea.min.y; p.y <= subarea.max.y; ++p.y)
	{
		for( p.x = subarea.min.x; p.x <= subarea.max.x; ++p.x)
		{
			image::pixel_t px( input_pixels_.const_rgba_view()( p.x - input_data_window_.min.x, p.y - input_data_window_.min.y));
			colors.push_back( Imath::Color3f( boost::gil::get_color( px, boost::gil::red_t()),
											   boost::gil::get_color( px, boost::gil::green_t()),
											   boost::gil::get_color( px, boost::gil::blue_t())));
		}
	}
}

} // namespace
} // namespace
