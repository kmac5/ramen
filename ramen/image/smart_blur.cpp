// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/smart_blur.hpp>

#include<algorithm>
#include<cmath>

#include<ramen/algorithm/clamp.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/image/color.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace
{

inline float abs( float x) { return x < 0 ? -x : x;}
	
void make_gauss_kernel( float *kernel, int size, float dev)
{
	if( dev == 0)
	{
		kernel[0] = 0.0f;
		kernel[1] = 1.0f;
		kernel[2] = 0.0f;
		return;
	}
	
	int radius = size / 2;
	float sum = 0;
	
	for (int i = 0; i < size; i++)
	{
		float diff = ( i - radius)/ dev;
		float value = std::exp(-diff * diff / 2);
		kernel[i] = value;
		sum += value;
	}

	float norm = 1.0f / sum;
	
	for (int i = 0; i < size; i++)
		kernel[i] *= norm;
}

struct smart_blur_fn
{
    smart_blur_fn( const const_image_view_t& src, const image_view_t& dst, float thereshold, float *kernel, int size) : src_(src), dst_(dst)
	{
		kernel_ = kernel;
		k_size_ = size;
		thereshold_ = thereshold;
	}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		using namespace boost::gil;
		
		int xoff = ( k_size_ - 1) / 2;

		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			const_image_view_t::x_iterator src_it( src_.row_begin( y));
			image_view_t::y_iterator dst_it( dst_.col_begin( y));

			for( int x = 0, e = src_.width(); x < e; ++x)
			{
				float cr = get_color( src_it[x], red_t());
				float cg = get_color( src_it[x], green_t());
				float cb = get_color( src_it[x], blue_t());
				float ca = get_color( src_it[x], alpha_t());

				float raccum = 0.0f, rweight = 0.0f;
				float gaccum = 0.0f, gweight = 0.0f;
				float baccum = 0.0f, bweight = 0.0f;
				float aaccum = 0.0f, aweight = 0.0f;

				float *k_it = kernel_;
				
				for( int i = -xoff; i <= xoff; ++i)
				{
                    int indx = clamp( x + i, 0, (int) src_.width() - 1);

					float r = get_color( src_it[indx], red_t());
					float g = get_color( src_it[indx], green_t());
					float b = get_color( src_it[indx], blue_t());
					float a = get_color( src_it[indx], alpha_t());
					float w = *k_it++;
					
					if( abs( r - cr) <= thereshold_)
					{
						raccum += w * r;
						rweight += w;
					}

					if( abs( g - cg) <= thereshold_)
					{
						gaccum += w * g;
						gweight += w;
					}

					if( abs( b - cb) <= thereshold_)
					{
						baccum += w * b;
						bweight += w;
					}

					if( abs( a - ca) <= thereshold_)
					{
						aaccum += w * a;
						aweight += w;
					}
				}
				
				image::pixel_t result( cr, cg, cb, ca);
				
				if( rweight != 0)
					get_color( result, red_t()) = raccum / rweight;

				if( gweight != 0)
					get_color( result, green_t()) = gaccum / gweight;

				if( bweight != 0)
					get_color( result, blue_t()) = baccum / bweight;

				if( aweight != 0)
					get_color( result, alpha_t()) = aaccum / aweight;
				
				dst_it[x] = result;
			}
		}
    }

    const const_image_view_t& src_;
    const image_view_t& dst_;
	float *kernel_;
	int k_size_;
	float thereshold_;
};

} // unnamed

void smart_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, const image_view_t& dst, 
					  float stddevx, float stddevy, float thereshold)
{
	// create kernel here
	int sizex = (int)( stddevx * 6 + 1) | 1;
	if( sizex == 1) sizex = 3;
	
	int sizey = (int)( stddevy * 6 + 1) | 1;
	if( sizey == 1) sizey = 3;
	
	float *kernel = new float[ std::max( sizex, sizey)];
	
	make_gauss_kernel( kernel, sizex, stddevx);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
					   smart_blur_fn( src, tmp, thereshold, kernel, sizex), tbb::auto_partitioner());

	make_gauss_kernel( kernel, sizey, stddevy);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
					   smart_blur_fn( tmp, dst, thereshold, kernel, sizey), tbb::auto_partitioner());

	delete[] kernel;
}

void smart_blur_rgba( const const_image_view_t& src, const image_view_t& dst, float stddevx, float stddevy, float thereshold)
{
	image::image_t tmp( src.height(), src.width());
	smart_blur_rgba( src, boost::gil::view( tmp), dst, stddevx, stddevy, thereshold);
}

} // namespace
} // namespace
