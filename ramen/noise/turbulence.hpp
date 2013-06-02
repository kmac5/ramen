// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_NOISE_TURBULENCE_HPP
#define RAMEN_NOISE_TURBULENCE_HPP

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace noise
{

template<class NoiseGen>
class turbulence_t
{
public:

	typedef typename NoiseGen::result_type result_type;
	
    turbulence_t( const NoiseGen& gen, int octaves = 4, float gain = 0.51f, float lacunarity = 1.9977f, bool turbulent = true) : gen_( gen)
	{
		octaves_ = octaves;
		gain_ = gain;
		lacunarity_ = lacunarity;
		turbulent_ = turbulent;
		
		// find the max possible value
		float max_value = 0;
		float scale = 1;
		
		for( int i = 0; i < octaves_; ++i)
		{
			max_value += scale;
			scale *= gain_;
		}
		
		norm_ = 1.0f / max_value;
	}
	
    result_type operator()( float x, float y = 0, float z = 0) const
	{
		float freq = 1.0f;
		float scale = 1.0f;

		result_type result = gen_( x, y, z);
		scale *= gain_;
		freq *= lacunarity_;
		
		for( int i = 1; i < octaves_; ++i)
		{
			result_type v = gen_( x * freq, y * freq, z * freq) * scale;
	
			if( turbulent_)
				v = abs( v);
	
			result += v;
			scale *= gain_;
			freq *= lacunarity_;
		}
	
		return result / norm_;
	}

	result_type operator()( const Imath::V2f& v) const { return (*this)( v.x, v.y);}
	result_type operator()( const Imath::V3f& v) const { return (*this)( v.x, v.y, v.z);}
	
private:

	float abs( float x) const { return ( x < 0) ? -x : x;}
	Imath::V2f abs( const Imath::V2f& v) const { return Imath::V2f( abs( v.x), abs( v.y));}
	Imath::V3f abs( const Imath::V3f& v) const { return Imath::V3f( abs( v.x), abs( v.y), abs( v.z));}

	const NoiseGen& gen_;
	
    int octaves_;
    float gain_;
    float lacunarity_;

    bool turbulent_;
	float norm_;
};

} // noise
} // ramen

#endif
