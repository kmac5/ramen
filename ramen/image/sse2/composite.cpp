// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/composite.hpp>
#include<ramen/image/sse2/layer_modes.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void composite_over( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::over_layer_mode_fun( opacity));
}

void composite_add( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::add_layer_mode_fun( opacity));
}

void composite_mul( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::mul_layer_mode_fun( opacity));
}

void composite_sub( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::sub_layer_mode_fun( opacity));
}

void composite_mix( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::mix_layer_mode_fun( opacity));
}

void composite_max( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::max_layer_mode_fun( opacity));
}

void composite_min( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    generic::composite( back, front, dst, sse2::min_layer_mode_fun( opacity));
}

} // namespace
} // namespace
} // namespace
