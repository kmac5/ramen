// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/curve_to_half_lut.hpp>

namespace ramen
{
namespace anim
{

eval_float_curve::eval_float_curve( const float_curve_t& c) : c_( c) {}
half eval_float_curve::operator()( half x) const { return c_.evaluate( x);}

void curve_to_half_lut( const float_curve_t& c, halfFunction<half>& lut)
{
    lut = halfFunction<half>( eval_float_curve( c));
}

} // namespace
} // namespace
