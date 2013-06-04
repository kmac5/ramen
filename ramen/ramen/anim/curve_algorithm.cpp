// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/curve_algorithm.hpp>

#include<vector>
#include<cmath>

#include<boost/foreach.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/anim/float_curve.hpp>

namespace ramen
{
namespace anim
{
namespace detail
{

int round( double x)
{
	return( x > 0.0) ? std::floor( x + 0.5) : std::ceil( x - 0.5);
}
	
} // detail

namespace
{

void make_gauss_kernel( float stddev, std::vector<float>& kernel)
{
	int size = (int)( stddev * 6 + 1) | 1;

	if( size == 1)
		size = 3;

	kernel.clear();
	kernel.reserve( size);

	if( stddev == 0)
	{
		kernel.push_back( 0.0f);
		kernel.push_back( 1.0f);
		kernel.push_back( 0.0f);
		return;
	}
	
	int radius = size / 2;
	float sum = 0;
	
	for (int i = 0; i < size; i++)
	{
		float diff = ( i - radius)/ stddev;
		float value = std::exp(-diff * diff / 2);
		kernel.push_back( value);
		sum += value;
	}

	float norm = 1.0f / sum;
	
	for (int i = 0; i < size; i++)
		kernel[i] *= norm;
}

float smooth_keyframe( const float_curve_t& c, float_curve_t::const_iterator it, const std::vector<float>& kernel)
{
	int off = ( kernel.size() - 1) / 2;
	
	float t = it->time();
	float val = 0;
	
	for( int i = -off; i <= off; ++i)
	{
		float w = kernel[i + off];
		val += c.evaluate( t + i) * w;
	}
	
	return val;
}

void do_sample_curve( const float_curve_t& src, float_curve_t& dst)
{	
	int start = std::floor( src.start_time());
	int end = std::ceil( src.end_time());

	for( int i = start; i <= end; ++i)
	{
		float val = src.evaluate( i);
		dst.insert( i, val, false);
	}
}

template<class Iter>
Iter find_selected_range_end( Iter it, Iter end)
{
	RAMEN_ASSERT( it != end);
	RAMEN_ASSERT( it->selected());
		
	Iter range_end( it);
	
	while( 1)
	{
		++range_end;
		
		if( range_end == end || !range_end->selected())
			return range_end - 1;
	}
}

void do_sample_selected_keys( const float_curve_t& src, float_curve_t& dst)
{
	float_curve_t::const_iterator it( src.begin()), end( src.end());
	float_curve_t::iterator new_it;
	
	while( it != end)
	{
		if( !it->selected())
			dst.insert( *it, false);
		else
		{
			// find selected range end
			float_curve_t::const_iterator range_end = find_selected_range_end( it, end);
			
			if( range_end == it)
			{
				new_it = dst.insert( *it, false);
				new_it->select( true);
			}
			else
			{
				int start = std::floor( it->time());
				int end = std::ceil( range_end->time());
			
				for( int i = start; i <= end; ++i)
				{
					float val = src.evaluate( i);
					new_it = dst.insert( i, val, false);
					new_it->select( true);
				}
				
			}
		}
		
		++it;
	}	
}

void do_reverse_selected_float_keyframes( float_curve_t& c)
{
	typedef float_curve_t::iterator iterator;
	
	iterator start( c.begin());
	iterator end( c.end());

	iterator s_it = detail::move_to_next_selected( start, end, end);	
	if( s_it == end)
		return;

	iterator e_it = detail::move_to_prev_selected( end - 1, start, start);
	if( e_it == start)
		return;
	
	while( s_it != e_it)
	{
		s_it->swap_value( *e_it);

		++s_it;
		s_it = detail::move_to_next_selected( s_it, e_it, end);
		
		if( s_it == end)
			return;

		--e_it;
		e_it = detail::move_to_prev_selected( e_it, s_it, start);

		if( e_it == start)
			return;
	}
}

} // unnamed

void move_selected_keyframes_value( float_curve_t& c, float d)
{
    BOOST_FOREACH( float_key_t& k, c.keys())
    {
        if( k.selected())
            k.set_value( Imath::clamp( k.value() + d, c.get_min(), c.get_max()));
    }
}

void negate_keyframes( float_curve_t& c, bool selected_only)
{
    BOOST_FOREACH( float_key_t& k, c.keys())
    {
		if( selected_only && !k.selected())
			continue;

		k.set_value( Imath::clamp( -k.value(), c.get_min(), c.get_max()));
	}
	
	c.recalc_tangents_and_coefficients();
}

void sample_keyframes( float_curve_t& c, bool selected_only)
{
	if( c.size() < 2)
		return;
	
	float_curve_t tmp( c);
	c.clear();
	
	if( !selected_only)
		do_sample_curve( tmp, c);
	else
		do_sample_selected_keys( tmp, c);

	c.recalc_tangents_and_coefficients();
}

void smooth_keyframes( float_curve_t& c, float stddev, bool selected_only)
{
	std::vector<float> kernel;
	make_gauss_kernel( stddev, kernel);

	float_curve_t tmp( c);
	
	for( float_curve_t::iterator it( c.begin()); it != c.end(); ++it)
	{
		if( selected_only && !it->selected())
			continue;

		float val = smooth_keyframe( tmp, it, kernel);
		it->set_value( Imath::clamp( val, c.get_min(), c.get_max()));
	}
	
	c.recalc_tangents_and_coefficients();
}

void highpass_keyframes( float_curve_t& c, float stddev, bool selected_only)
{
	std::vector<float> kernel;
	make_gauss_kernel( stddev, kernel);

	float_curve_t tmp( c);
	
	for( float_curve_t::iterator it( c.begin()); it != c.end(); ++it)
	{
		if( selected_only && !it->selected())
			continue;
		
		float val = smooth_keyframe( tmp, it, kernel);
		it->set_value( Imath::clamp( it->value() - val, c.get_min(), c.get_max()));
	}
	
	c.recalc_tangents_and_coefficients();
}

void reverse_float_keyframes( float_curve_t& c, bool selected_only)
{
	if( c.empty() || c.size() == 1)
		return;
	
	do_reverse_selected_float_keyframes( c);
	c.recalc_tangents_and_coefficients();
}

} // namespace
} // namespace
