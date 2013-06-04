// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_COLOR_BARS_HPP
#define RAMEN_COLOR_BARS_HPP

#include<OpenEXR/ImathBox.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void make_color_bars( const image_view_t& view);
void make_color_bars( const image_view_t& view, const Imath::Box2i& domain, const Imath::Box2i& defined);

} // namespace
} // namespace

#endif
