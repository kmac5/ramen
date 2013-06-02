// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/warp.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/assert.hpp>

#include<ramen/image/sse2/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{
namespace
{

template<class Sampler>
class warp_fun
{
public:
	
	warp_fun( const Sampler& s, const Imath::Box2i& dst_area,
			  const image_view_t& dst, const warp_function_t& fun) : dst_( dst), fun_( fun), s_( s)
	{
		dst_area_ = dst_area;
	}
	
	void operator()( const tbb::blocked_range<int>& r) const
	{
		for( int j = r.begin(); j < r.end(); ++j)
		{
			for( int i = dst_area_.min.x; i <= dst_area_.max.x; ++i)
			{
				Imath::V2f p( fun_( Imath::V2f( i, j)));
				dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( p);
			}
		}
	}
	
private:

	Sampler s_;
	Imath::Box2i dst_area_;
	const image_view_t& dst_;
	warp_function_t fun_;
};

template<class Sampler>
class warp_and_derivs_fun
{
public:

	warp_and_derivs_fun( const Sampler& s, const Imath::Box2i& dst_area,
						const image_view_t& dst, const warp_function_t& fun) : dst_( dst), fun_( fun), s_( s)
	{
		dst_area_ = dst_area;
	}

	void operator()( const tbb::blocked_range<int>& r) const
	{
		for( int j = r.begin(); j < r.end(); ++j)
		{
			for( int i = dst_area_.min.x; i <= dst_area_.max.x; ++i)
			{
				Imath::V2f p( fun_( Imath::V2f( i, j)));
				Imath::V2f pdu( fun_( Imath::V2f( i + 0.5, j)));
				Imath::V2f pdv( fun_( Imath::V2f( i, j + 0.5)));

				dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( p, pdu - p, pdv - p);
			}
		}
	}

private:

	Sampler s_;
	Imath::Box2i dst_area_;
	const image_view_t& dst_;
	warp_function_t fun_;
};

template<class Sampler>
void warp( const Sampler& s, const Imath::Box2i& dst_area,
		   const image_view_t& dst, const warp_function_t& wfun, bool derivs = false)
{
	if( derivs)
	{
		tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y+1),
							warp_and_derivs_fun<Sampler>( s, dst_area, dst, wfun), tbb::auto_partitioner());
	}
	else
	{
		tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y+1),
							warp_fun<Sampler>( s, dst_area, dst, wfun), tbb::auto_partitioner());
	}
}

template<class Sampler>
void warp_sequential( const Sampler& s, const Imath::Box2i& dst_area,
					   const image_view_t& dst, const warp_function_t& wfun, bool derivs = false)
{
	if( derivs)
	{
		warp_and_derivs_fun<Sampler> fun( s, dst_area, dst, wfun);
		fun( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y+1));
	}
	else
	{
		warp_fun<Sampler> fun( s, dst_area, dst, wfun);
		fun( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y+1));
	}
}

} // unnamed

void warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
					const Imath::Box2i& dst_area, const image_view_t& dst,
					const warp_function_t& wfun, bool uv_derivs, bool sequential)
{
	sse2::bilinear_sampler_t s( src_area, src);

	if( sequential)
		warp_sequential<sse2::bilinear_sampler_t>( s, dst_area, dst, wfun, uv_derivs);
	else
		warp<sse2::bilinear_sampler_t>( s, dst_area, dst, wfun, uv_derivs);
}

void warp_bilinear_tile( const Imath::Box2i& src_area, const const_image_view_t& src,
						const Imath::Box2i& dst_area, const image_view_t& dst, const warp_function_t& wfun, bool uv_derivs, bool sequential)
{
	sse2::bilinear_sampler_t s( src_area, src);
	generic::tile_sampler_t<sse2::bilinear_sampler_t> ts( s);

	if( sequential)
		warp_sequential<generic::tile_sampler_t<sse2::bilinear_sampler_t> >( ts, dst_area, dst, wfun, uv_derivs);
	else
		warp<generic::tile_sampler_t<sse2::bilinear_sampler_t> >( ts, dst_area, dst, wfun, uv_derivs);
}

void warp_bilinear_mirror( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst, const warp_function_t& wfun, bool uv_derivs, bool sequential)
{
	sse2::bilinear_sampler_t s( src_area, src);
	generic::mirror_sampler_t<sse2::bilinear_sampler_t> tm( s);

	if( sequential)
		warp_sequential<generic::mirror_sampler_t<sse2::bilinear_sampler_t> >( tm, dst_area, dst, wfun, uv_derivs);
	else
		warp<generic::mirror_sampler_t<sse2::bilinear_sampler_t> >( tm, dst_area, dst, wfun, uv_derivs);
}

void warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
					const Imath::Box2i& dst_area, const image_view_t& dst, const warp_function_t& wfun, bool uv_derivs, bool sequential)
{
	sse2::catrom_sampler_t s( src_area, src);

	if( sequential)
		warp_sequential<sse2::catrom_sampler_t>( s, dst_area, dst, wfun, uv_derivs);
	else
		warp<sse2::catrom_sampler_t>( s, dst_area, dst, wfun, uv_derivs);
}

} // namespace
} // namespace
} // namespace
