// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_GGEMS_TBB_POLYSCAN_HPP
#define RAMEN_GGEMS_TBB_POLYSCAN_HPP

#include<algorithm>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/assert.hpp>

#include<ramen/ggems/polyscan/poly_scan.hpp>

namespace ramen
{
namespace polyscan
{
namespace detail
{

template<class PixelFun>
struct poly_scan_fun
{
	poly_scan_fun( Poly *p, Window *win, PixelFun pfun) : pfun_( pfun)
	{
		RAMEN_ASSERT( p);
		RAMEN_ASSERT( win);
			
		copy_poly( p, &p_);
		win_ = win;
	}
		
	void operator()( const tbb::blocked_range<int>& r) const
	{
		poly_clip_y( &p_, r.begin(), r.end());
		poly_scan( &p_, win_, pfun_);
	}
	
private:

	mutable Poly p_;
	Window *win_;
	PixelFun pfun_;
};
	
} // detail

template<class PixelFun>
void parallel_poly_scan( Poly *p, Window *win, PixelFun pfun)
{
	float ymin = std::numeric_limits<float>::max();
	float ymax = -ymin;

	for( int i = 0; i < p->n; ++i)
	{
		ymin = std::min( ymin, p->vert[i].sy);
		ymax = std::max( ymax, p->vert[i].sy);
	}
	
	int imin = Imath::Math<float>::floor( ymin);
	int imax = Imath::Math<float>::ceil( ymax);
	
	imin = std::max( imin, win->y0);
	imax = std::min( imax, win->y1);

	const int height_thereshold = 50;
	
	if( imin - imax > height_thereshold)
		tbb::parallel_for( tbb::blocked_range<int>( imin, imax + 1), detail::poly_scan_fun<PixelFun>( p, win, pfun));
	else
		poly_scan( p, win, pfun);
}

} // polyscan
} // ramen

#endif
