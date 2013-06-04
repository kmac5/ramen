// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_PROJECTIVE_WARP_HPP
#define RAMEN_IMAGE_GENERIC_PROJECTIVE_WARP_HPP

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

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
class projective_warp_fn
{
public:

    projective_warp_fn( const Sampler& s, const Imath::Box2i& dst_area,
						const image_view_t& dst, const matrix3_t& inv_xform) : dst_area_(dst_area), dst_( dst),
                                                                               inv_xform_( inv_xform), s_( s)
    {
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
		for( int j = r.begin(); j < r.end(); ++j)
		{
			typedef vector2_t::BaseType real_type;
			typedef Imath::Vec3<real_type> vector3_type;
			vector3_type p ( vector3_type( dst_area_.min.x, j, 1) * inv_xform_);
			vector3_type p_end( vector3_type( dst_area_.max.x, j, 1) * inv_xform_);
			vector3_type inc( ( p_end - p) / ( dst_area_.max.x - dst_area_.min.x));

			vector2_t du( 0, 0), dv( 0, 0);

			for( int i = dst_area_.min.x; i <= dst_area_.max.x; ++i)
			{
				if( p.z != 0)
				{
					real_type iz = real_type( 1) / p.z;
					vector2_t q( p.x * iz, p.y * iz);
					dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( q);
				}
	
				p += inc;
			}
		}
    }

private:

    Sampler s_;
    const Imath::Box2i& dst_area_;
    const image_view_t& dst_;
    const matrix3_t& inv_xform_;
};


template<class Sampler>
class projective_warp_and_derivs_fn
{
public:

    projective_warp_and_derivs_fn( const Sampler& s, const Imath::Box2i& dst_area,
									const image_view_t& dst, const matrix3_t& inv_xform) : dst_area_(dst_area), dst_( dst),
						                                                                   inv_xform_( inv_xform), s_( s)
    {
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
		for( int j = r.begin(); j < r.end(); ++j)
		{
			typedef vector2_t::BaseType real_type;
			typedef Imath::Vec3<real_type> vector3_type;
			vector3_type p ( vector3_type( dst_area_.min.x, j, 1) * inv_xform_);
			vector3_type p_end( vector3_type( dst_area_.max.x, j, 1) * inv_xform_);
			vector3_type inc( ( p_end - p) / ( dst_area_.max.x - dst_area_.min.x));

			vector3_type pdu ( vector3_type( dst_area_.min.x + 0.5, j, 1) * inv_xform_);
			vector3_type pdu_end( vector3_type( dst_area_.max.x + 0.5, j, 1) * inv_xform_);
			vector3_type pdu_inc( ( pdu_end - pdu) / ( dst_area_.max.x - dst_area_.min.x));

			vector3_type pdv ( vector3_type( dst_area_.min.x, j + 0.5, 1) * inv_xform_);
			vector3_type pdv_end( vector3_type( dst_area_.max.x, j + 0.5, 1) * inv_xform_);
			vector3_type pdv_inc( ( pdv_end - pdv) / ( dst_area_.max.x - dst_area_.min.x));

			for( int i = dst_area_.min.x; i <= dst_area_.max.x; ++i)
			{
				if( p.z != 0)
				{
					real_type iz = real_type( 1) / p.z;
					vector2_t q( p.x * iz, p.y * iz);

					vector2_t du( 0, 0), dv( 0, 0);

					if( pdu.z != 0)
					{
						iz = real_type( 1) / pdu.z;
						du = vector2_t( pdu.x * iz, pdu.y * iz) - q;
					}

					if( pdv.z != 0)
					{
						iz = real_type( 1) / pdv.z;
						dv = vector2_t( pdv.x * iz, pdv.y * iz) - q;
					}

					dst_( i - dst_area_.min.x, j - dst_area_.min.y) = s_( q, du, dv);
				}

				p += inc;
				pdu += pdu_inc;
				pdv += pdv_inc;
			}
		}
    }

private:

    Sampler s_;
    const Imath::Box2i& dst_area_;
    const image_view_t& dst_;
    const matrix3_t& inv_xform_;
};

} // detail

// for debugging, run in only one thread
template<class Sampler>
void projective_warp_seq( const Sampler& s, const Imath::Box2i& dst_area, const image_view_t& dst,
							const matrix3_t& xform, const matrix3_t& inv_xform, bool derivs = false)
{
	if( derivs)
	{
		detail::projective_warp_and_derivs_fn<Sampler> f( s, dst_area, dst, inv_xform);
	    f( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y + 1));
	}
	else
	{
		detail::projective_warp_fn<Sampler> f( s, dst_area, dst, inv_xform);
	    f( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y + 1));
	}
}

template<class Sampler>
void projective_warp( const Sampler& s, const Imath::Box2i& dst_area, const image_view_t& dst,
					 const matrix3_t& xform, const matrix3_t& inv_xform, bool derivs = false)
{
	if( derivs)
	{
	    tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y + 1),
													  detail::projective_warp_and_derivs_fn<Sampler>( s, dst_area, dst, inv_xform),
													  tbb::auto_partitioner());
	}
	else
	{
	    tbb::parallel_for( tbb::blocked_range<int>( dst_area.min.y, dst_area.max.y + 1),
													  detail::projective_warp_fn<Sampler>( s, dst_area, dst, inv_xform),
													  tbb::auto_partitioner());
	}
}

void projective_warp_nearest( const Imath::Box2i& src_area, const const_image_view_t& src,
							 const Imath::Box2i& dst_area, const image_view_t& dst,
							 const matrix3_t& xform, const matrix3_t& inv_xform);
} // namespace
} // namespace
} // namespace

#endif
