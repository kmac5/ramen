// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/viewer/image_view/empty_image_strategy.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

empty_image_strategy_t::empty_image_strategy_t( const Imath::Box2i& display_window ) : image_strategy_t( display_window, Imath::Box2i())
{
}

bool empty_image_strategy_t::update_pixels( const image::buffer_t& pixels,
                                            const Imath::Box2i& display_window,
                                            const Imath::Box2i& data_window)
{
	if( data_window.isEmpty())
		return true;

	return false;
}

void empty_image_strategy_t::draw() const {}

boost::optional<Imath::Color4f> empty_image_strategy_t::color_at( const Imath::V2i& p) const
{
	return boost::optional<Imath::Color4f>();
}

} // viewer
} // ui
} // ramen
