// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULATORS_UTIL_HPP
#define RAMEN_MANIPULATORS_UTIL_HPP

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace manipulators
{

Imath::V2f rotate_point( const Imath::V2f& center, const Imath::V2f& p, float angle_in_degrees);
	
} // namespace
} // namespace

#endif
