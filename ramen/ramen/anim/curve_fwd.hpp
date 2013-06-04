// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVE_FWD_HPP
#define RAMEN_ANIM_CURVE_FWD_HPP

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace anim
{

class float_key_t;
class float_curve_t;

template<class T>
class shape_key_t;

typedef shape_key_t<Imath::V2f> shape_key2f_t;
typedef shape_key_t<Imath::V3f> shape_key3f_t;

template<class P>
class shape_curve_t;

typedef shape_curve_t<Imath::V2f> shape_curve2f_t;
typedef shape_curve_t<Imath::V3f> shape_curve3f_t;

} // namespace
} // namespace

#endif
