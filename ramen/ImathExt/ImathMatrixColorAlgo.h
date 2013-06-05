// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_MATRIX_COLOR_ALGO_HPP
#define RAMEN_IMATHEXT_MATRIX_COLOR_ALGO_HPP

#include<OpenEXR/ImathMatrix.h>

namespace ramen
{
namespace ImathExt
{

Imath::M44f hueRotationMatrix( float angle);
Imath::M44f saturationMatrix( float sat);
Imath::M44f gainMatrix( const Imath::V3f& g);
Imath::M44f gainMatrix( float g);

} // ImathExt
} // ramen

#endif
