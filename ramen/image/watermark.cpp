// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/watermark.hpp>

#include<algorithm>

#include<OpenEXR/ImathRandom.h>

namespace ramen
{
namespace image
{
namespace
{

Imath::Rand48 rng;
	
} // unnamed

void watermark( const image_view_t& dst)
{
	const int num_squares = 256;
	const int sq_size = 7;
	
	for( int i = 0; i < num_squares; ++i)
	{
		int x = rng.nextf() * ( dst.width() - sq_size - 1);
		int y = rng.nextf() * ( dst.height() - sq_size  - 1);

		image::pixel_t p( rng.nextf(), rng.nextf(), rng.nextf(), 1.0f);
		
		for( int j = 0; j < sq_size; ++j)
			for( int k = 0; k < sq_size; ++k)
				dst( x + k, y + j) = p;
	}
}

} // namespace
} // namespace
