// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_BOX_ALGO_HPP
#define RAMEN_IMATHEXT_BOX_ALGO_HPP

#include<ramen/config.hpp>

#include<algorithm>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>
#include<OpenEXR/ImathMath.h>

namespace Imath
{

template<class T>
bool isInside( const Box<Vec2<T> >& bounds, const Box<Vec2<T> >& box)
{
    return bounds.intersects( box.min) && bounds.intersects( box.max);
}

template<class T>
Box<Vec2<T> > intersect( const Box<Vec2<T> >& a, const Box<Vec2<T> >& b)
{
    if( !( a.min.x > b.max.x || a.max.x < b.min.x || a.min.y > b.max.y || a.max.y < b.min.y))
    {
        return Box<Vec2<T> >( Vec2<T>( std::max( a.min.x, b.min.x), std::max( a.min.y, b.min.y)),
                              Vec2<T>( std::min( a.max.x, b.max.x), std::min( a.max.y, b.max.y)));
    }
    else
        return Box<Vec2<T> >();
}

template<class T>
Box<T> offsetBy( const Box<T>& box, const T& d)
{
    Box<T> newbox( box);
    newbox.min += d;
    newbox.max += d;
    return newbox;
}

template<class T>
Box2i roundBox( const Box< Vec2<T> >& b, bool round_up = false)
{
	if( b.isEmpty())
		return Box2i();

    if( round_up)
    {
        return Box2i( V2i( Math<T>::floor( b.min.x), Math<T>::floor( b.min.y)),
                      V2i( Math<T>::ceil( b.max.x) , Math<T>::ceil( b.max.y)));
    }
    else
    {
        return Box2i( V2i( Math<T>::ceil( b.min.x) , Math<T>::ceil( b.min.y)),
                      V2i( Math<T>::floor( b.max.x), Math<T>::floor( b.max.y)));
    }
}	

template <class T>
Box< Vec2<T> > transform( const Box< Vec2<T> >& box, const Matrix33<T>& m)
{
    if( box.isEmpty())
		return box;

    if (m[0][2] == 0 && m[1][2] == 0 && m[2][2] == 1)
    {
		Box< Vec2<T> > newBox;

		for (int i = 0; i < 2; i++)
        {
		    newBox.min[i] = newBox.max[i] = m[2][i];

		    for (int j = 0; j < 2; j++)
            {
				float a = m[j][i] * box.min[j];
				float b = m[j][i] * box.max[j];

				if (a < b)
                {
				    newBox.min[i] += a;
				    newBox.max[i] += b;
				}
				else
                {
				    newBox.min[i] += b;
				    newBox.max[i] += a;
				}
		    }
		}

		return newBox;
    }

    Vec2<T> points[4];
    points[0].x = box.min.x;
    points[0].y = box.min.y;
    points[1].x = box.max.x;
    points[1].y = box.min.y;
    points[2].x = box.max.x;
    points[2].y = box.max.y;
    points[3].x = box.min.x;
    points[3].y = box.max.y;

    Box< Vec2<T> > newBox;

    for (int i = 0; i < 4; i++)
		newBox.extendBy (points[i] * m);

    return newBox;
}

RAMEN_API Box2i transform( const Box2i& box, const M33f& m, bool round_up = false);
RAMEN_API Box2i transform( const Box2i& box, const M33d& m, bool round_up = false);

RAMEN_API Box2i scale( const Box2i& b, float s);
RAMEN_API Box2i scale( const Box2i& b, float sx, float sy);
RAMEN_API Box2i scale( const Box2i& b, float sx, float sy, int xcenter, int ycenter);

RAMEN_API Box2f scale( const Box2f& b, float s);
RAMEN_API Box2f scale( const Box2f& b, float sx, float sy);
RAMEN_API Box2f scale( const Box2f& b, float sx, float sy, float xcenter, float ycenter);

template<class F> // -> Imath::V2f F::operator()( const Imath::V2f& p)
Box2f warpBox( const Box2f& b, F f)
{
	Box2f box;
	Imath::V2f p;
	
	for( int i = b.min.x; i <= b.max.x; ++i)
	{
		box.extendBy( f( Imath::V2f( i, b.min.y)));
		box.extendBy( f( Imath::V2f( i, b.max.y)));
	}

	for( int i = b.min.y; i <= b.max.y; ++i)
	{
		box.extendBy( f( Imath::V2f( b.min.x, i)));
		box.extendBy( f( Imath::V2f( b.max.x, i)));
	}

	return box;
}

template<class F> // -> Imath::V2f F::operator()( const Imath::V2f& p)
Box2i warpBox( const Box2i& b, F f, bool round_up = false)
{
	Box2f box( Imath::V2f( b.min), Imath::V2f( b.max));
	box = warpBox( box, f);
	
	if( box.isEmpty())
		return Imath::Box2i();
	
	return roundBox( box, round_up);
}

template<class T>
inline std::ostream& operator << ( std::ostream& os, const Box<T>& b)
{
    os << "[ " << b.min << "; " << b.max << "]";
    return os;
}

} // Imath

#endif
