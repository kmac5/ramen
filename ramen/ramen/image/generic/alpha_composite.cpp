// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/alpha_composite.hpp>

#include<ramen/image/generic/composite.hpp>
#include<ramen/image/generic/alpha_layer_modes.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void alpha_composite_add( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
	generic::composite( back, front, dst, alpha_add_layer_mode_fun( opacity));
}
	
void alpha_composite_mul( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
	generic::composite( back, front, dst, alpha_mul_layer_mode_fun( opacity));
}
	
void alpha_composite_sub( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
	generic::composite( back, front, dst, alpha_sub_layer_mode_fun( opacity));
}
	
void alpha_composite_mix( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
	generic::composite( back, front, dst, alpha_mix_layer_mode_fun( opacity));
}
	
void alpha_composite_max( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
	generic::composite( back, front, dst, alpha_max_layer_mode_fun( opacity));
}
	
void alpha_composite_min( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
	generic::composite( back, front, dst, alpha_min_layer_mode_fun( opacity));
}

} // namespace
} // namespace
} // namespace
