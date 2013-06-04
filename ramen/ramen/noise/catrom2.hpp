// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#ifndef RAMEN_NOISE_CATROM2_HPP
#define RAMEN_NOISE_CATROM2_HPP

#include<boost/cstdint.hpp>

#include<ramen/math/fast_float.hpp>

namespace ramen
{
namespace noise
{

class catrom2_t
{
public:

    catrom2_t();

	float operator()( float d) const
	{
		if( d >= 4.0f)
			return 0.0f;
		
		d = d * sample_rate() + 0.5f;
		int i = math::fast_float_floor( d);
		
		if( i >= num_entries())
			return 0;
		
		return table_[i];
	}
	
private:

	static int sample_rate() { return 100;}
	static int num_entries() { return 4 * sample_rate() + 1;}

	static void init_table();
	static float table_[401];
};

} // noise
} // ramen

#endif
