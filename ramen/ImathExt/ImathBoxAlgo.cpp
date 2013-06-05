// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ImathExt/ImathBoxAlgo.h>

namespace ramen
{
namespace ImathExt
{

Imath::Box2i transform( const Imath::Box2i& box, const Imath::M33f& m, bool round_up)
{
    Imath::Box2f b( box.min, box.max);
    b = transform( b, m);
	return roundBox( b, round_up);
}

Imath::Box2i transform( const Imath::Box2i& box, const Imath::M33d& m, bool round_up)
{
    Imath::Box2d b( box.min, box.max);
    b = transform( b, m);
	return roundBox( b, round_up);
}

Imath::Box2i scale( const Imath::Box2i& b, float s) { return scale( b, s, s);}

Imath::Box2i scale( const Imath::Box2i& b, float sx, float sy)
{
    return Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( b.min.x * sx), Imath::Math<float>::floor( b.min.y * sy)),
			 Imath::V2i( Imath::Math<float>::ceil(  b.max.x * sx), Imath::Math<float>::ceil(  b.max.y * sy)));
}

Imath::Box2i scale( const Imath::Box2i& b, float sx, float sy, int xcenter, int ycenter)
{
    Imath::Box2i box = offsetBy( b, Imath::V2i( -xcenter, -ycenter));
    box = scale( box, sx, sy);
    box = offsetBy( box, Imath::V2i( xcenter, ycenter));
    return box;
}

Imath::Box2f scale( const Imath::Box2f& b, float s) { return scale( b, s, s);}

Imath::Box2f scale( const Imath::Box2f& b, float sx, float sy)
{
    return Imath::Box2f( Imath::V2f( b.min.x * sx, b.min.y * sy), Imath::V2f( b.max.x * sx, b.max.y * sy));
}

Imath::Box2f scale( const Imath::Box2f& b, float sx, float sy, float xcenter, float ycenter)
{
    Imath::Box2f box = offsetBy( b, Imath::V2f( -xcenter, -ycenter));
    box = scale( box, sx, sy);
    box = offsetBy( box, Imath::V2f( xcenter, ycenter));
    return box;
}

} // ImathExt
} // ramen

