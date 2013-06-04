// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_ANY_CURVE_HPP
#define	RAMEN_ANIM_ANY_CURVE_HPP

#include<boost/variant.hpp>

#include<ramen/anim/curve_fwd.hpp>

namespace ramen
{
namespace anim
{

typedef boost::variant<float_curve_t, shape_curve2f_t> any_curve_t;
typedef boost::variant<float_curve_t*, shape_curve2f_t*> any_curve_ptr_t;

void copy( const any_curve_ptr_t& src, any_curve_t& dst);
void copy( const any_curve_t& src, any_curve_ptr_t& dst);
void swap( any_curve_ptr_t& a, any_curve_t& b);

} // anim
} // ramen

#endif
