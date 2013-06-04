// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_ALPHA_LAYER_MODES_HPP
#define RAMEN_IMAGE_GENERIC_ALPHA_LAYER_MODES_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

struct alpha_add_layer_mode_fun
{
    alpha_add_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct alpha_mul_layer_mode_fun
{
    alpha_mul_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct alpha_sub_layer_mode_fun
{
    alpha_sub_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct alpha_mix_layer_mode_fun
{
    alpha_mix_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct alpha_max_layer_mode_fun
{
    alpha_max_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct alpha_min_layer_mode_fun
{
    alpha_min_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

} // namespace
} // namespace
} // namespace

#endif
