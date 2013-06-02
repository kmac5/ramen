// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#ifndef RAMEN_NOISE_VECTOR_NOISE_ADAPTOR_HPP
#define RAMEN_NOISE_VECTOR_NOISE_ADAPTOR_HPP

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace noise
{

template<class N0, class N1>
class vector_noise_adaptor2_t
{
public:
	
	typedef Imath::V2f result_type;
	
	vector_noise_adaptor2_t( const N0& g0, const N1& g1) : gen0_( g0), gen1_( g1) {}

	Imath::V2f operator()( float x, float y = 0, float z = 0) const
	{
		return Imath::V2f( gen0_( x, y, z), gen1_( x, y, z));
	}

	Imath::V2f operator()( const Imath::V2f& v) const { return (*this)( v.x, v.y);}
	Imath::V2f operator()( const Imath::V3f& v) const { return (*this)( v.x, v.y, v.z);}
	
private:
	
	const N0& gen0_;
	const N1& gen1_;
};

template<class N0, class N1, class N2>
class vector_noise_adaptor3_t
{
public:

	typedef Imath::V3f result_type;
	
	vector_noise_adaptor3_t( const N0& g0, const N1& g1, const N2& g2) : gen0_( g0), gen1_( g1), gen2_( g2) {}

	Imath::V3f operator()( float x, float y = 0, float z = 0) const
	{
		return Imath::V3f( gen0_( x, y, z), gen1_( x, y, z), gen2_( x, y, z));
	}

	Imath::V3f operator()( const Imath::V2f& v) const { return (*this)( v.x, v.y);}
	Imath::V3f operator()( const Imath::V3f& v) const { return (*this)( v.x, v.y, v.z);}
	
private:
	
	const N0& gen0_;
	const N1& gen1_;
	const N2& gen2_;
};

} // noise
} // ramen

#endif
