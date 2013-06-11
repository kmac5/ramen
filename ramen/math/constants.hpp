// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_CONSTANTS_HPP
#define RAMEN_MATH_CONSTANTS_HPP

#include<ramen/config.hpp>

namespace ramen
{
namespace math
{

template<class T> struct constants {};

template<> struct constants<float>
{
	static float pi()		{ return 3.141592653589793238462643383279502884197f;}
	static float half_pi()	{ return 1.570796326794896619231321691639751442098f;}
	static float double_pi(){ return 6.283185307179586476925286766559005768394f;}
	static float deg2rad()	{ return 0.017453292519943295769236907684886127134f;}
	static float rad2deg()	{ return 57.29577951308232087679815481410517033381f;}
	static float sqrt2()	{ return 1.414213562373095048801688724209698078569f;}
	static float sqrt3()	{ return 1.732050807568877293527446341505872366942f;}
};

template<> struct constants<double>
{
	static double pi()		 { return 3.141592653589793238462643383279502884197;}
	static double half_pi()	 { return 1.570796326794896619231321691639751442098;}
	static double double_pi(){ return 6.283185307179586476925286766559005768394;}
	static double deg2rad()	 { return 0.017453292519943295769236907684886127134;}
	static double rad2deg()	 { return 57.29577951308232087679815481410517033381;}
	static double sqrt2()	 { return 1.414213562373095048801688724209698078570;}
	static double sqrt3()	 { return 1.732050807568877293527446341505872366940;}
};

} // math
} // ramen

#endif
