// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_DENAN_HPP
#define RAMEN_IMAGE_DENAN_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void denan( const const_image_view_t& src, const image_view_t& dst, float replacement = 0.0f);

} // image
} // ramen

#endif
