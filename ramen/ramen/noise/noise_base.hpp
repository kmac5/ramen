// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#ifndef RAMEN_NOISE_BASE_HPP
#define RAMEN_NOISE_BASE_HPP

#include<boost/config.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathRandom.h>

#include<ramen/noise/permutation_array.hpp>

#include<ramen/math/fast_float.hpp>

namespace ramen
{
namespace noise
{

class noise_base_t
{
public:
	
	typedef float result_type;
	
	noise_base_t();
	
	BOOST_STATIC_CONSTANT( int, tabsize = 256);
	BOOST_STATIC_CONSTANT( int, tabmask = 256 - 1);
	
protected:

	int floor( float x) const { return math::fast_float_floor( x);}
	float smooth_step( float x) const { return ( x * x * (3 - 2 * x));}
	float lerp( float t, float x0, float x1) const { return x0 + t * ( x1 - x0);}
	
	permutation_array_t perm_;
};

} // noise
} // ramen

#endif
