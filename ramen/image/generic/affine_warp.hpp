// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_AFFINE_WARP_HPP
#define RAMEN_IMAGE_GENERIC_AFFINE_WARP_HPP

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ImathExt/ImathBoxAlgo.h>

#include<ramen/image/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

template<class Sampler>
class affine_warp_fn
{
public:

    affine_warp_fn( const Sampler& s, const Imath::Box2i& dst_area, const image_view_t& dst,
		      const Imath::Box2i& active_area, const matrix3_t& inv_xform) : dst_area_(dst_area), dst_( dst),
																			  active_area_(active_area),
																			  inv_xform_( inv_xform),
																			  s_( s)
    {
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
		vector2_t du, dv;
		inv_xform_.multDirMatrix( vector2_t( 0.5, 0), du);
		inv_xform_.multDirMatrix( vector2_t( 0, 0.5), dv);

		for( int j = r.begin(); j < r.end(); ++j)
		{
				vector2_t p ( vector2_t( active_area_.min.x, j) * inv_xform_);
				vector2_t p1( vector2_t( active_area_.max.x, j) * inv_xform_);
				vector2_t inc( ( p1 - p) / ( active_area_.max.x - active_area_.min.x));
	
			for( int i = active_area_.min.x; i <= active_area_.max.x; ++i)
			{
				dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( p, du, dv);
				p += inc;
			}
		}
    }

private:

    Sampler s_;
    const Imath::Box2i& dst_area_, active_area_;
    const image_view_t& dst_;
    const matrix3_t& inv_xform_;
};

} // detail

// for debugging, run in only one thread
template<class Sampler>
void affine_warp_seq( const Sampler& s, const Imath::Box2i& src_area,
						const Imath::Box2i& dst_area, const image_view_t& dst,
						 const matrix3_t& xform, const matrix3_t& inv_xform)
{
    Imath::Box2i active_area( ImathExt::transform( src_area, xform));
    active_area = ImathExt::intersect( active_area, dst_area);
	detail::affine_warp_fn<Sampler> f( s, dst_area, dst, active_area, inv_xform);
    f( tbb::blocked_range<int>( active_area.min.y, active_area.max.y+1));
}

template<class Sampler>
void affine_warp( const Sampler& s, const Imath::Box2i& src_area,
				 const Imath::Box2i& dst_area, const image_view_t& dst,
				 const matrix3_t& xform, const matrix3_t& inv_xform)
{
    Imath::Box2i active_area( ImathExt::transform( src_area, xform));
    active_area = ImathExt::intersect( active_area, dst_area);

    tbb::parallel_for( tbb::blocked_range<int>( active_area.min.y, active_area.max.y+1),
												detail::affine_warp_fn<Sampler>( s, dst_area, dst, active_area, inv_xform),
												tbb::auto_partitioner());
}

template<class Sampler>
void affine_warp_non_black_border_mode( const Sampler& s, const Imath::Box2i& dst_area, const image_view_t& dst,
                                        const matrix3_t& xform, const matrix3_t& inv_xform)
{
    tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y+1),
												  detail::affine_warp_fn<Sampler>( s, dst_area, dst, dst_area, inv_xform),
												  tbb::auto_partitioner());
}

void affine_warp_nearest( const Imath::Box2i& src_area, const const_image_view_t& src,
						 const Imath::Box2i& dst_area, const image_view_t& dst,
						 const matrix3_t& xform, const matrix3_t& inv_xform);

} // namespace
} // namespace
} // namespace

#endif
