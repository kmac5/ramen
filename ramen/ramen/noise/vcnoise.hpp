// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#ifndef RAMEN_NOISE_VCNOISE_HPP
#define RAMEN_NOISE_VCNOISE_HPP

#include<boost/noncopyable.hpp>

#include<ramen/noise/noise_base.hpp>
#include<ramen/noise/catrom2.hpp>

namespace ramen
{
namespace noise
{

class vcnoise_t : public noise_base_t, boost::noncopyable
{
public:

	vcnoise_t( int seed = 0);
	
	float operator()( float x, float y = 0, float z = 0) const;
	float operator()( const Imath::V2f& v) const { return (*this)( v.x, v.y);}
	float operator()( const Imath::V3f& v) const { return (*this)( v.x, v.y, v.z);}
	
private:

	void init( int seed);

	float vlattice( int ix, int iy, int iz) const;
	
	float value_table_[ tabsize];
	catrom2_t catrom_;
};
	
} // noise
} // ramen

#endif
