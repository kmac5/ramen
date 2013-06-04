// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#ifndef RAMEN_NOISE_WEIGHTED_NOISE_HPP
#define RAMEN_NOISE_WEIGHTED_NOISE_HPP

namespace ramen
{
namespace noise
{

template<class N0, class N1>
class weighted_noise_t
{
public:
	
	typedef typename N0::result_type result_type;
	
	weighted_noise_t( const N0& g0, float w0, const N1& g1) : gen0_( g0), gen1_( g1)
	{
		t_ = w0;
		inv_t_ = 1.0f - t_;
	}

	result_type operator()( float x, float y, float z) const
	{
		return ( t * gen0_( x, y, z)) + ( inv_t_ * gen1_( x, y, z));
	}
	
private:
	
	const N0& gen0_;
	const N1& gen1_;
	float t_, inv_t_;
};

} // noise
} // ramen

#endif
