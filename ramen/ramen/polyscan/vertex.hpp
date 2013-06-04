// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_POLYSCAN_VERTEX_HPP
#define RAMEN_POLYSCAN_VERTEX_HPP

#include<emmintrin.h>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathColor.h>

#include<ramen/assert.hpp>
#include<ramen/align.hpp>

namespace ramen
{
namespace polyscan
{

class RAMEN_ALIGN( 16) vertex_t
{
public:

	vertex_t();

	Imath::V4f screen;
	float inv_w;

	Imath::V3f world;

	// texture and derivs
	Imath::V2f uv;
	Imath::V3f nyuv;
	Imath::V3f nxuv;

	void increment( const vertex_t& other);
	void substract( const vertex_t& other, vertex_t& result);

	void mult( float x);

	void madd( const vertex_t& other, float x);

	void interpolate( const vertex_t& u, const vertex_t& v, float t);
	void sub_scale( const vertex_t& u, const vertex_t& v, float s);

	void perspective_divide();

	void texture_coords_and_derivs( Imath::V2f& uv, Imath::V2f& duv_dx, Imath::V2f& duv_dy) const;

private:

	const float *sse2_ptr() const	{ return reinterpret_cast<const float*>( this);}
	float *sse2_ptr()				{ return reinterpret_cast<float*>( this);}

	const int sse2_size() const { return sizeof( vertex_t) / 4;}

	// Update this if you add more things to the struct
	//float pad_[3];
};

} // namespace
} // namespace

#endif
