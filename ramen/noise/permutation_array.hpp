// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#ifndef RAMEN_NOISE_PERM_ARRAY_HPP
#define RAMEN_NOISE_PERM_ARRAY_HPP

#include<boost/cstdint.hpp>

namespace ramen
{
namespace noise
{

class permutation_array_t
{
public:

    permutation_array_t() {}

	int operator()( int x) const
	{
		return perm_[ x & mask()];
	}

	int operator()( int x, int y) const
	{
		return perm_[ x & mask()] + perm_[ y & mask()];
	}
	
	int operator()( int x, int y, int z) const
	{
		return perm_[ x & mask()] + perm_[ y & mask()] + perm_[ z & mask()];
	}
	
private:

	int size() const { return 256;}
	int mask() const { return size() - 1;}
	
	static boost::uint8_t perm_[256];
};

} // noise
} // ramen

#endif
