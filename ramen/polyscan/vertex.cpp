// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/polyscan/vertex.hpp>

#include<string.h>

namespace ramen
{
namespace polyscan
{

vertex_t::vertex_t() { memset( sse2_ptr(), 0, sizeof( vertex_t));}

void vertex_t::increment( const vertex_t& other)
{
	const float *src = other.sse2_ptr();
	float *dst = sse2_ptr();

	for( int i = 0, ie = sse2_size(); i < ie; ++i)
	{
		__m128 v0 = _mm_load_ps( src);
		__m128 v1 = _mm_load_ps( dst);
		__m128 v2 = _mm_add_ps( v0, v1);
		_mm_store_ps( dst, v2);
		src += 4;
		dst += 4;
	}
}

void vertex_t::substract( const vertex_t& other, vertex_t& result)
{
	const float *src0 = sse2_ptr();
	const float *src1 = other.sse2_ptr();
	float *dst = result.sse2_ptr();

	for( int i = 0, ie = sse2_size(); i < ie; ++i)
	{
		__m128 v0 = _mm_load_ps( src0);
		__m128 v1 = _mm_load_ps( src1);
		__m128 v2 = _mm_sub_ps( v0, v1);
		_mm_store_ps( dst, v2);
		src0 += 4;
		src1 += 4;
		dst += 4;
	}
}

void vertex_t::mult( float x)
{
	float *src = sse2_ptr();
	__m128 fact = _mm_set_ps1( x);

	for( int i = 0, ie = sse2_size(); i < ie; ++i)
	{
		__m128 v0 = _mm_load_ps( src);
		__m128 v1 = _mm_mul_ps( v0, fact);
		_mm_store_ps( src, v1);
		src += 4;
	}
}

void vertex_t::madd( const vertex_t& other, float x)
{
	const float *src = other.sse2_ptr();
	float *dst = sse2_ptr();
	__m128 fact = _mm_set_ps1( x);

	for( int i = 0, ie = sse2_size(); i < ie; ++i)
	{
		__m128 v0 = _mm_load_ps( src);
		__m128 v1 = _mm_load_ps( dst);
		__m128 v2 = _mm_add_ps( _mm_mul_ps( v0, fact), v1);
		_mm_store_ps( dst, v2);
		src += 4;
		dst += 4;
	}
}

void vertex_t::interpolate( const vertex_t& u, const vertex_t& v, float t)
{
	const float *usrc = u.sse2_ptr();
	const float *vsrc = v.sse2_ptr();
	float *dst = sse2_ptr();

	__m128 tt = _mm_set_ps1( t);

	for( int i = 0, ie = sse2_size(); i < ie; ++i)
	{
		__m128 uu = _mm_load_ps( usrc);
		__m128 vv = _mm_load_ps( vsrc);
		__m128 vmu = _mm_sub_ps( vv, uu);
		__m128 r = _mm_add_ps( uu, _mm_mul_ps( tt, vmu));
		_mm_store_ps( dst, r);
		usrc += 4;
		vsrc += 4;
		dst += 4;
	}
}

void vertex_t::sub_scale( const vertex_t& u, const vertex_t& v, float s)
{
	const float *usrc = u.sse2_ptr();
	const float *vsrc = v.sse2_ptr();
	float *dst = sse2_ptr();

	__m128 ss = _mm_set_ps1( s);

	for( int i = 0, ie = sse2_size(); i < ie; ++i)
	{
		__m128 uu = _mm_load_ps( usrc);
		__m128 vv = _mm_load_ps( vsrc);
		__m128 vmu = _mm_sub_ps( uu, vv);
		__m128 r = _mm_mul_ps( ss, vmu);
		_mm_store_ps( dst, r);
		usrc += 4;
		vsrc += 4;
		dst += 4;
	}
}

void vertex_t::perspective_divide()
{
	RAMEN_ASSERT( screen.w != 0);

	inv_w = 1.0f / screen.w;
	screen.x *= inv_w;
	screen.y *= inv_w;

	uv.x *= inv_w;
	uv.y *= inv_w;
}

void vertex_t::texture_coords_and_derivs( Imath::V2f& uv, Imath::V2f& duv_dx, Imath::V2f& duv_dy) const
{
	float w = 1.0f / inv_w;
	uv.x = this->uv.x * w;
	uv.y = this->uv.y * w;

	w = 1.0f / nyuv.z;
	Imath::V2f ny( nyuv.x * w, nyuv.y * w);

	w = 1.0f / nxuv.z;
	Imath::V2f nx( nxuv.x * w, nxuv.y * w);

	duv_dx = nx - uv;
	duv_dx *= 0.5f;

	duv_dy = ny - uv;
	duv_dy *= 0.5f;
}

} // namespace
} // namespace
