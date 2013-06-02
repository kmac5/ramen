// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/util.hpp>

#include<cmath>

#include<ramen/assert.hpp>

namespace ramen
{
namespace anim
{

double repeat_time( double t, double start_time, double end_time)
{	
	double ipart;
	double new_time = std::fabs( std::modf( (double) ( t - start_time) / ( end_time - start_time), &ipart));
		
	if( t < start_time)
		new_time = 1.0 - new_time;

	new_time = ( new_time * end_time) + ( ( 1.0 - new_time) * start_time);
	
	RAMEN_ASSERT( new_time >= start_time);
	RAMEN_ASSERT( new_time <= end_time);
	return new_time;
}
	
} // anim
} // ramen
