// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_BOX2_HPP
#define RAMEN_MATH_BOX2_HPP

#include<ramen/config.hpp>

#include<boost/operators.hpp>

#include<ramen/math/point2.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief A two dimensional bounding box.
*/
template<class T>
class box2_t : boost::equality_comparable<box2_t<T> >
{
public:

    typedef T               value_type;
    typedef point2_t<T>     point_type;
    typedef vector2_t<T>    vector_type;

    box2_t()
    {
        reset();
    }

    box2_t( const point_type& p) : min( p), max( p) {}

    box2_t( const point_type& pmin, const point_type& pmax) : min( pmin), max( pmax) {}

    void reset()
    {
        min.x = min.y = std::numeric_limits<T>::max();
        max.x = max.y = -std::numeric_limits<T>::max();
    }

    bool empty() const
    {
        return max.x < min.x || max.y < min.y;
    }

    vector_type size() const
    {
        return max - min;
    }

    point_type center() const
    {
        return min + ( size() * T(0.5));
    }

    bool operator==( const box2_t<T>& other) const
    {
        return min == other.min && max == other.max;
    }

    point_type min, max;
};

typedef box2_t<int>     box2i_t;
typedef box2_t<float>   box2f_t;
typedef box2_t<double>  box2d_t;
typedef box2_t<half>    box2h_t;

} // math
} // ramen

#endif
