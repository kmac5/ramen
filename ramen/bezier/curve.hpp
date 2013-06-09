// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BEZIER_CURVE_HPP
#define	RAMEN_BEZIER_CURVE_HPP

#include<cmath>

#include<boost/config.hpp>
#include<boost/static_assert.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/bezier/bernstein.hpp>

namespace ramen
{
namespace bezier
{

// P is Imath::Vec 2, 3 or a similar class
template<class P = Imath::V2f, int Degree = 3>
class curve_t
{
    BOOST_STATIC_ASSERT( Degree >= 1);

public:

    BOOST_STATIC_CONSTANT( int, degree = Degree);
    BOOST_STATIC_CONSTANT( int, order  = Degree + 1);

    typedef P                           point_type;
    typedef P                           vector_type;
    typedef typename P::BaseType        param_type;
    typedef typename P::BaseType        coord_type;
    typedef Imath::Box<P>				box_type;
	typedef Imath::Matrix33<coord_type> matrix_type;
	
    curve_t() {}

    curve_t( const point_type& q0, const point_type& q1)
    {
        BOOST_STATIC_ASSERT( degree == 1);
        p[0] = q0;
        p[1] = q1;
    }

    curve_t( const point_type& q0, const point_type& q1, const point_type& q2)
    {
        BOOST_STATIC_ASSERT( degree == 2);
        p[0] = q0;
        p[1] = q1;
        p[2] = q2;
    }

    curve_t( const point_type& q0, const point_type& q1, const point_type& q2, const point_type& q3)
    {
        BOOST_STATIC_ASSERT( degree == 3);
        p[0] = q0;
        p[1] = q1;
        p[2] = q2;
        p[3] = q3;
    }

    const point_type& operator[]( int indx) const
    {
        assert( indx >= 0 && indx < order && "bezier curve operator[], index out of bounds");
        return p[indx];
    }

    point_type& operator[]( int indx)
    {
        assert( indx >= 0 && indx < order && "bezier curve operator[], index out of bounds");
        return p[indx];
    }

    point_type operator()( param_type t) const
    {
        boost::array<param_type, order> B;
        all_bernstein<param_type, degree>( t, B);

        point_type q;

        for( unsigned int j = 0; j < point_type::dimensions(); ++j)
        {
            q[j] = 0;

            for( int i = 0; i < order; ++i)
                q[j] += B[i] * p[i][j];
        }

        return q;
    }

    box_type bounding_box() const
    {
        box_type b;

        for( int i = 0; i < order; ++i)
            b.extendBy( p[i]);

        return b;
    }

	void translate( const vector_type& t)
	{
        for( int i = 0; i < order; ++i)
			p[i] += t;
	}

	void transform( const matrix_type& m)
	{
        for( int i = 0; i < order; ++i)
			p[i]  = m * p[i];
	}
	
    curve_t<P, Degree-1> derivative_curve() const
    {
        BOOST_STATIC_ASSERT( degree > 1);

        curve_t<P, Degree-1> result;

        for( int i = 0; i < Degree; ++i)
        {
            point_type q;

            for( unsigned int j = 0; j < point_type::dimensions(); ++j)
                q[j] = p[i+1][j] - p[i][j];

            result.p[i] = q;
        }

        return result;
    }

    vector_type start_derivative1() const
    {
        BOOST_STATIC_ASSERT( degree >= 1);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( p[1][i] - p[0][i]);

        return result;
    }

    vector_type end_derivative1() const
    {
        BOOST_STATIC_ASSERT( degree >= 1);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( p[Degree][i] - p[Degree-1][i]);

        return result;
    }

    vector_type start_derivative2() const
    {
        BOOST_STATIC_ASSERT( degree >= 2);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( Degree - 1) * ( p[2][i] - p[1][i] - p[1][i] + p[0][i]);

        return result;
    }

    vector_type end_derivative2() const
    {
        BOOST_STATIC_ASSERT( degree >= 2);

        vector_type result;

        for( unsigned int i = 0; i < P::dimensions(); ++i)
            result[i] = Degree * ( Degree - 1) * ( p[Degree][i] - p[Degree - 1][i] - p[Degree - 1][i] + p[Degree - 2][i]);

        return result;
    }

    coord_type start_curvature() const
    {
        vector_type d1( start_derivative1());
        vector_type d2( start_derivative2());
        coord_type k = ( d1.x * d2.y) - ( d1.y * d2.x);
        return k / ( std::pow( (double) d1.length2(), 3.0 / 2.0));
    }

    coord_type end_curvature() const
    {
        vector_type d1( end_derivative1());
        vector_type d2( end_derivative2());
        coord_type k = ( d1.x * d2.y) - ( d1.y * d2.x);
        return k / ( std::pow( (double) d1.length2(), 3.0 / 2.0));
    }

    point_type p[Degree+1];
};

} // bezier
} // ramen

#endif
