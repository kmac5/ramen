// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/float_curve.hpp>

#include<OpenEXR/halfFunction.h>

namespace ramen
{
namespace anim
{

struct eval_float_curve
{
	eval_float_curve( const float_curve_t& c);
	half operator()( half x) const;

private:

	const float_curve_t& c_;
};

void curve_to_half_lut( const float_curve_t& c, halfFunction<half>& lut);

} // namespace
} // namespace
