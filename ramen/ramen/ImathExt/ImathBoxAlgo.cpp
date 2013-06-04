// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ImathExt/ImathBoxAlgo.h>

namespace Imath
{
	
Box2i transform( const Box2i& box, const M33f& m, bool round_up)
{
    Box2f b( box.min, box.max);
    b = transform( b, m);
	return roundBox( b, round_up);
}

Box2i transform( const Box2i& box, const M33d& m, bool round_up)
{
    Box2d b( box.min, box.max);
    b = transform( b, m);
	return roundBox( b, round_up);
}

Box2i scale( const Box2i& b, float s) { return scale( b, s, s);}

Box2i scale( const Box2i& b, float sx, float sy)
{
    return Box2i( V2i( Math<float>::floor( b.min.x * sx), Math<float>::floor( b.min.y * sy)),
			 V2i( Math<float>::ceil(  b.max.x * sx), Math<float>::ceil(  b.max.y * sy)));
}

Box2i scale( const Box2i& b, float sx, float sy, int xcenter, int ycenter)
{
    Box2i box = offsetBy( b, V2i( -xcenter, -ycenter));
    box = scale( box, sx, sy);
    box = offsetBy( box, V2i( xcenter, ycenter));
    return box;
}

Box2f scale( const Box2f& b, float s) { return scale( b, s, s);}

Box2f scale( const Box2f& b, float sx, float sy)
{
    return Box2f( V2f( b.min.x * sx, b.min.y * sy), V2f( b.max.x * sx, b.max.y * sy));
}

Box2f scale( const Box2f& b, float sx, float sy, float xcenter, float ycenter)
{
    Box2f box = offsetBy( b, V2f( -xcenter, -ycenter));
    box = scale( box, sx, sy);
    box = offsetBy( box, V2f( xcenter, ycenter));
    return box;
}

} // Imath
