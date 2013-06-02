/*

	gauss_lut2d

*/

#ifndef _GAUSS_LUT2D
#define _GAUSS_LUT2D

#include<cstddef>
#include<cmath>

#include<boost/scoped_array.hpp>

template<class T>
class gauss_lut2d
{
public:

	gauss_lut2d( std::size_t size = 150) : size_(size)
	{
	T *p;
	
		lut_.reset( new T[size_ * size_]);
		
		for( int j=0;j<size_;++j)
		{
			p = lut_.get() + ( j * size_);
			
			for( int i=0;i<size_;++i)
				*p++ = gaussian( (i*i)+(j*j));
		}
	}

	inline T gaussian( T distance2) const
	{	
		return exp( - distance2 / 256.0) / 20.05302619705;
	}

	T operator()( int x, int y) const
	{
	int xn = x,yn = y;

		if( xn < 0) xn = -xn;
		if( yn < 0) yn = -yn;
				
		if( (xn > size_ - 1) || (yn > size_ - 1))
			return gaussian( (x*x)+(y*y));

		return lut_[(yn * size_) + xn];
	}
	
public:

	std::size_t size_;
	boost::scoped_array<T> lut_;	
};

#endif
