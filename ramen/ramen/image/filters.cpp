// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/filters.hpp>

#include<algorithm>

#include<OpenEXR/ImathMath.h>

#include<ramen/assert.hpp>

#include<ramen/math/constants.hpp>
#include<ramen/math/fast_float.hpp>

namespace ramen
{
namespace image
{

filter_t::filter_t() {}
int filter_t::accuracy() { return 1024;}

/****************************************************/

std::vector<float> lanczos3_filter_t::ftable;

int lanczos3_filter_t::filter_area( float scale) const
{
	// ( x >> 1) << 1, means: if x is odd, then x = x - 1;
	// interesting idiom...
	return ((int)( std::max( scale, 1.0f) * 6 + 2) >> 1) << 1;
}

float lanczos3_filter_t::operator()( float x) const
{
	const int half_table_size = 4 * 1024;
	const float faccuracy = 1024.0f;
	return ftable[ math::fast_float_to_int( x * faccuracy) + half_table_size];
}

void lanczos3_filter_t::init_table()
{
    int size = accuracy() * 4 * 2;
	ftable.reserve( size);

	float faccuracy = accuracy();

	for( int i = 0; i < size; ++i)
	{
		float x = ( i - size / 2.0f + 0.5f) / faccuracy;
		x *= math::constants<float>::pi();
		float y = Imath::Math<float>::sin( x) / x * Imath::Math<float>::sin( x / 3.0f) / ( x / 3.0f);
		ftable.push_back( y);
	}
}

/****************************************************/

std::vector<float> mitchell_filter_t::ftable;

int mitchell_filter_t::filter_area( float scale) const
{
	// TODO: check this
	return ((int)( std::max( scale, 1.0f) * 4 + 2) >> 1) << 1;
}

float mitchell_filter_t::operator()( float x) const
{
	const int half_table_size = 3 * 1024;
	const float faccuracy = 1024.0f;
	return ftable[ math::fast_float_to_int( x * faccuracy) + half_table_size];
}

void mitchell_filter_t::init_table()
{
	float b = 1.0f / 3.0f;
	float c = 1.0f / 3.0f;
    float p0 = (6 - 2*b) / 6;
	float p2 = (-18 + 12*b + 6*c) / 6;
	float p3 = (12 - 9*b - 6*c) / 6;
	float q0 = (8*b + 24*c) / 6;
	float q1 = (-12*b - 48*c) / 6;
	float q2 = (6*b + 30*c) / 6;
	float q3 = (-b - 6*c) / 6;

    int size = accuracy() * 3 * 2;
	ftable.reserve( size);

	float faccuracy = accuracy();

	for( int i = 0; i < size; ++i)
	{
		float x = Imath::Math<float>::fabs( ( i - size / 2.0f + 0.5f) / faccuracy);
		float y;

	    if(x < 1.0f)
		    y = p0 + x*x*(p2 + x*p3);
		else
		{
		    if(x < 2.0f)
			    y = q0 + x*(q1 + x*(q2 + x*q3));
			else
				y = 0.0f;
		}

		ftable.push_back( y);
	}
}

/****************************************************/

std::vector<float> catrom_filter_t::ftable;

int catrom_filter_t::filter_area( float scale) const
{
	// TODO: check this
	return ((int)( std::max( scale, 1.0f) * 4 + 2) >> 1) << 1;
}

float catrom_filter_t::operator()( float x) const
{
	const int half_table_size = 3 * 1024;
	const float faccuracy = 1024.0f;
	return ftable[ math::fast_float_to_int( x * faccuracy) + half_table_size];
}

void catrom_filter_t::init_table()
{
    int size = accuracy() * 3 * 2;
	ftable.reserve( size);

	float faccuracy = accuracy();

	for( int i = 0; i < size; ++i)
	{
		float x = ( i - size / 2.0f + 0.5f) / faccuracy;
		ftable.push_back( catrom( x));
	}
}

float catrom_filter_t::catrom( float x)
{
  if (x < -2) return 0.0f;
  if (x < -1) return 0.5f * (4 + x*(8 + x*(5 + x)));
  if (x < 0)  return 0.5f * (2 + x*x*(-5 - 3*x));
  if (x < 1)  return 0.5f * (2 + x*x*(-5 + 3*x));
  if (x < 2)  return 0.5f * (4 + x*(-8 + x*(5 - x)));
  return 0.0f;
}

} // image
} // ramen
