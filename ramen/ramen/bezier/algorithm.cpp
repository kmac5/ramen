// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/bezier/algorithm.hpp>

#include<ramen/ggems/nearestpoint.h>

namespace ramen
{
namespace bezier
{

Imath::V2f nearest_point_on_curve( const curve_t<Imath::V2f>& c, const Imath::V2f& p, float& t)
{
    Point2 q;
    Point2 cp[4];
    double tt;

    q.x = p.x;
    q.y = p.y;

    for( int i = 0; i < 4; ++i)
    {
        cp[i].x = c[i].x;
        cp[i].y = c[i].y;
    }

    Point2 result = NearestPointOnCurve( q, cp, &tt);
	t = tt;
    return Imath::V2f( result.x, result.y);
}

Imath::V2f nearest_point_on_curve( const curve_t<Imath::V2f>& c, const Imath::V2f& p)
{
	float t;
	return nearest_point_on_curve( c, p, t);
}

} // namespace
} // namespace
