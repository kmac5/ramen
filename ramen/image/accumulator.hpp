// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_ACCUMULATOR_HPP
#define	RAMEN_IMAGE_ACCUMULATOR_HPP

#include<OpenEXR/ImathBox.h>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

class rgba_accumulator_t
{
public:

    rgba_accumulator_t( const image_view_t& view);
    rgba_accumulator_t( const image_view_t& view, const Imath::Box2i defined);

    void clear();

    void accumulate( const const_image_view_t& v, float factor = 1.0f);
    void accumulate( const const_image_view_t& v, const Imath::Box2i& area, float factor = 1.0f);
    void multiply( float f);

private:

    image_view_t view_;
    Imath::Box2i defined_;
};

class gray_accumulator_t
{
public:

    gray_accumulator_t( const gray_image_view_t& view);
    gray_accumulator_t( const gray_image_view_t& view, const Imath::Box2i defined);

    void clear();

    void accumulate( const const_gray_image_view_t& v, float factor = 1.0f);
    void accumulate( const const_gray_image_view_t& v, const Imath::Box2i& area, float factor = 1.0f);
    void multiply( float f);

private:

    gray_image_view_t view_;
    Imath::Box2i defined_;
};

} // namespace
} // namespace

#endif
