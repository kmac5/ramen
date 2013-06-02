// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FUNCTIONAL_CONSTANTS_HPP
#define RAMEN_FUNCTIONAL_CONSTANTS_HPP

#include<functional>

namespace ramen
{

struct always_one
{
	template<class T>
	T operator()( T x) const { return T( 1);}
};

struct always_zero
{
	template<class T>
	T operator()( T x) const { return T( 0);}
};

} // namespace

#endif
