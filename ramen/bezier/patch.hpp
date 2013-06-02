// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BEZIER_PATCH_HPP
#define	RAMEN_BEZIER_PATCH_HPP

#include<cmath>

#include<vector>

#include<boost/config.hpp>
#include<boost/static_assert.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/bezier/detail.hpp>
#include<ramen/bezier/bernstein.hpp>

namespace ramen
{
namespace bezier
{

// P is Imath::Vec 2, 3 or a similar class
template<class P, int Degree = 3>
class patch_t
{
    BOOST_STATIC_ASSERT( Degree >= 1);

public:

    BOOST_STATIC_CONSTANT( int, degree = Degree);
    BOOST_STATIC_CONSTANT( int, order  = Degree + 1);

    typedef P                           point_type;
    typedef P                           vector_type;
    typedef typename P::BaseType        param_type;
    typedef typename P::BaseType        coord_type;
    typedef typename Imath::Box<P>      box_type;

    patch_t() {}

    const point_type *operator[]( int indx) const
    {
        assert( indx >= 0 && indx < order && "bezier patch operator[], index out of bounds");
        return p[indx];
    }

    point_type *operator[]( int indx)
    {
        assert( indx >= 0 && indx < order && "bezier patch operator[], index out of bounds");
        return p[indx];
    }

    point_type operator()( param_type t, param_type s) const
    {
        boost::array<param_type, order> Bt, Bs;
        all_bernstein<param_type, degree>( t, Bt);
        all_bernstein<param_type, degree>( s, Bs);

        point_type q[4];

		for( int j = 0; j < order; ++j)
		{
	        for( int k = 0; k < point_type::dimensions(); ++k)
	        {
	            q[j][k] = 0;
	
	            for( int i = 0; i < order; ++i)
	                q[j][k] += Bt[i] * p[j][i][k];
	        }
		}
		
		point_type r;
		
		for( int k = 0; k < point_type::dimensions(); ++k)
		{
			r[k] = 0;

			for( int i = 0; i < order; ++i)
				r[k] += Bs[i] * q[i][k];
		}
		
		return r;
    }

    box_type bounding_box() const
    {
        box_type b;

        for( int i = 0; i < order; ++i)
			for( int j = 0; j < order; ++j)
            b.extendBy( p[i][j]);

        return b;
    }

    point_type p[Degree+1][Degree+1];
};

template<class P, class T>
void tesselate_patch( const patch_t<P,3>& patch, const boost::array<T,4>& data, 
					  int usteps, int vsteps, std::vector<P>& pts, std::vector<T>& data_out)
{
	typedef typename patch_t<P,3>::param_type param_type;
	
	pts.clear();
	data_out.clear();
	
	for( int j = 0; j <= vsteps; ++j)
	{
		param_type v = ( param_type) j / vsteps;
		
		for( int i = 0; i < usteps; ++i)
		{
			param_type u = ( param_type) i / usteps;
			
			P pt = patch( u, v);
			T dt = detail::bilerp( data[0], data[1], data[2], data[3], u, v);

			pts.push_back( pt);
			data_out.push_back( dt);
		}
	}
}

} // namespace
} // namespace

#endif
