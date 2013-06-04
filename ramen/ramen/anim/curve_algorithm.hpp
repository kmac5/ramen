// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVE_ALGORITHM_HPP
#define	RAMEN_ANIM_CURVE_ALGORITHM_HPP

#include<ramen/anim/curve_fwd.hpp>

#include<ramen/assert.hpp>

#include<ramen/anim/curve.hpp>
#include<ramen/anim/keyframe.hpp>

namespace ramen
{
namespace anim
{
namespace detail
{

int round( double x);

template<class K>
void move_selected_keys_time_left( curve_t<K>& c, float offset, bool do_round = false)
{
	typedef typename curve_t<K>::iterator iterator;
	iterator it( c.begin()), prev( c.begin());
	
	float new_time;

	if( it->selected())
	{
		new_time = it->time() - offset;
		
		if( do_round)
			new_time = round( new_time);

		it->set_time( new_time);
	}

	++it;
	
	for( ; it != c.end(); ++it, ++prev)
	{
		if( it->selected())
		{
			new_time = it->time() - offset;

			if( do_round)
				new_time = round( new_time);
			
			it->set_time( std::max( new_time, prev->time() + keyframe_t::time_tolerance()));
		}
	}
}
	
template<class K>
void move_selected_keys_time_right( curve_t<K>& c, float offset, bool do_round = false)
{
	typedef typename curve_t<K>::reverse_iterator reverse_iterator;
	
	reverse_iterator it( c.rbegin()), prev( c.rbegin());
	float new_time;

	if( it->selected())
	{
		new_time = it->time() + offset;

		if( do_round)
			new_time = round( new_time);

		it->set_time( new_time);
	}
	
	++it;

	for( ; it != c.rend(); ++it, ++prev)
	{
		if( it->selected())
		{
			new_time = it->time() + offset;
			
			if( do_round)
				new_time = round( new_time);
			
			it->set_time( std::min( new_time, prev->time() - keyframe_t::time_tolerance()));
		}
	}
}

template<class Iter>
Iter move_to_next_selected( Iter it, Iter e_it, Iter end)
{
	while( it < e_it && it != end)
	{
		if( it->selected())
			return it;
		
		++it;
	}
	
	return end;
}

template<class Iter>
Iter move_to_prev_selected( Iter it, Iter s_it, Iter start)
{
	while( it > s_it && it != start)
	{
		if( it->selected())
			return it;
		
		--it;
	}
	
	return start;
}

template<class K>
void do_reverse_selected_keyframes( curve_t<K>& c)
{
	typedef typename curve_t<K>::iterator iterator;
	
	iterator start( c.begin());
	iterator end( c.end());

	iterator s_it = move_to_next_selected( start, end, end);	
	if( s_it == end)
		return;

	iterator e_it = move_to_prev_selected( end - 1, start, start);
	if( e_it == start)
		return;
	
	while( s_it != e_it)
	{
		s_it->swap_value( *e_it);

		++s_it;
		s_it = move_to_next_selected( s_it, e_it, end);
		
		if( s_it == end)
			return;

		--e_it;
		e_it = move_to_prev_selected( e_it, s_it, start);

		if( e_it == start)
			return;
	}
}

template<class K>
void do_reverse_keyframes( curve_t<K>& c)
{
	typedef typename curve_t<K>::iterator iterator;
	
	iterator s_it( c.begin());
	iterator e_it( c.end() - 1);

	while( s_it != e_it)
	{
		s_it->swap_value( *e_it);
		++s_it;
		--e_it;
	}
}

} // detail

template<class K>
void move_selected_keyframes_time( curve_t<K>& c, float d, bool do_round = false)
{
	if( d < 0.0f)
		detail::move_selected_keys_time_left( c, -d, do_round);
	else
		detail::move_selected_keys_time_right( c, d, do_round);
}

template<class K>
void move_selected_keyframes_value( curve_t<K>& c, float d)
{
	RAMEN_ASSERT( 0 && "Can't move non float curves keyframes in Y");
}

template<class K>
void reverse_keyframes( curve_t<K>& c, bool selected_only)
{
	if( c.empty() || c.size() == 1)
		return;
	
	if( selected_only)
		detail::do_reverse_selected_keyframes( c);
	else
		detail::do_reverse_keyframes( c);
}

void move_selected_keyframes_value( float_curve_t& c, float d);
void negate_keyframes( float_curve_t& c, bool selected_only);
void sample_keyframes( float_curve_t& c, bool selected_only);
void smooth_keyframes( float_curve_t& c, float stddev, bool selected_only);
void highpass_keyframes( float_curve_t& c, float stddev, bool selected_only);

// for some reason, gcc does not pick this overload...
void reverse_float_keyframes( float_curve_t& c, bool selected_only);

} // namespace
} // namespace

#endif
