// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_INTERVAL_HPP
#define RAMEN_INTERVAL_HPP

#include<algorithm>
#include<limits>

namespace ramen
{

template<class T=float>
class interval_t
{
public:

	interval_t() { reset();}
	
	interval_t( T a, T b)
	{
		lower = a;
		upper = b;
	}
	
	bool empty() const { return lower > upper;}

	void reset()
	{
		lower = std::numeric_limits<T>::max();
		upper = -lower;
	}

	interval_t<T>& join( const interval_t<T>& other)
	{
		lower = std::min( other.lower, lower);
		upper = std::max( other.upper, upper);
		return *this;
	}

	interval_t<T>& intersect( const interval_t<T>& other)
	{
		if( !( lower > other.upper || upper < other.lower))
		{
			lower = std::max( lower, other.lower);
			upper = std::min( upper, other.upper);
		}
		else
			reset();
		
		return *this;
	}
	
	T lower;
	T upper;	
};

} // namespace

#endif
