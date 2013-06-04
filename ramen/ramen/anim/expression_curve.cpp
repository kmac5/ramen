// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/expression_curve.hpp>

#include<limits>

#include<OpenEXR/ImathFun.h>

namespace ramen
{
namespace anim
{

expression_curve_t::expression_curve_t()
{
    min_ = -std::numeric_limits<float>::max();
    max_ =  std::numeric_limits<float>::max();	
	scale_ = 1.0f;
	offset_ = 0.0f;
}

float expression_curve_t::evaluate( float time) const
{
	// TODO: implement this	
	float val = 0;
	return Imath::clamp( val, min_, max_);
}

} // namespace
} // namespace
