// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_BOX3_HPP
#define RAMEN_MATH_BOX3_HPP

#include<ramen/config.hpp>

#include<algorithm>
#include<limits>

#include<boost/operators.hpp>

#include<ramen/math/point3.hpp>
#include<ramen/math/matrix44.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief A three dimensional bounding box.
*/
template<class T>
class box3_t : boost::equality_comparable<box3_t<T> >
{
public:

    typedef T               value_type;
    typedef point3_t<T>     point_type;
    typedef vector3_t<T>    vector_type;

    box3_t()
    {
        reset();
    }

    box3_t( const point_type& p) : min( p), max( p) {}

    box3_t( const point_type& pmin, const point_type& pmax) : min( pmin), max( pmax) {}

    void reset()
    {
        min.x = min.y = min.z =  std::numeric_limits<T>::max();
        max.x = max.y = max.z = -std::numeric_limits<T>::max();
    }

    bool empty() const
    {
        return max.x < min.x || max.y < min.y || max.z < min.z;
    }

    vector_type size() const
    {
        return max - min;
    }

    point_type center() const
    {
        return min + ( size() * T(0.5));
    }

    void extend_by( const point_type& p)
    {
        min.x = std::min( min.x, p.x);
        max.x = std::max( max.x, p.x);
        min.y = std::min( min.y, p.y);
        max.y = std::max( max.y, p.y);
        min.z = std::min( min.z, p.z);
        max.z = std::max( max.z, p.z);
    }

    void extend_by( const box3_t<T>& b)
    {
        min.x = std::min( min.x, b.min.x);
        max.x = std::max( max.x, b.max.x);
        min.y = std::min( min.y, b.min.y);
        max.y = std::max( max.y, b.max.y);
        min.z = std::min( min.z, b.min.z);
        max.z = std::max( max.z, b.max.z);
    }

    void transform( const matrix44_t<T>& m)
    {
        // TODO: this could be optimized if m is affine
        box3_t<T> saved( *this);
        extend_by( saved.min * m);
        extend_by( point3_t<T>( saved.max.x, saved.min.y, saved.min.z) * m);
        extend_by( point3_t<T>( saved.max.x, saved.max.y, saved.min.z) * m);
        extend_by( point3_t<T>( saved.min.x, saved.max.y, saved.min.z) * m);
        extend_by( point3_t<T>( saved.max.x, saved.min.y, saved.max.z) * m);
        extend_by( point3_t<T>( saved.max.x, saved.max.y, saved.max.z) * m);
        extend_by( point3_t<T>( saved.min.x, saved.max.y, saved.max.z) * m);
        extend_by( saved.max * m);
    }

    // regular concept
    bool operator==( const box3_t<T>& other) const
    {
        return min == other.min && max == other.max;
    }

    point_type min, max;
};

template<class T>
box3_t<T> transform( const box3_t<T>& box, const matrix44_t<T>& m)
{
    box3_t<T> x( box);
    x.transform( m);
    return x;
}

typedef box3_t<int>     box3i_t;
typedef box3_t<float>   box3f_t;
typedef box3_t<double>  box3d_t;
typedef box3_t<half>    box3h_t;

} // math
} // ramen

#endif
