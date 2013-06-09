// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_SAMPLERS_HPP
#define	RAMEN_IMAGE_GENERIC_SAMPLERS_HPP

#include<cmath>

#include<algorithm>

#include<boost/scoped_array.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathFun.h>

#include<ramen/math/fast_float.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

class sampler_t
{
public:

	sampler_t() {}
    sampler_t( const const_image_view_t& src);
    sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

	const Imath::Box2i& src_area() const		{ return src_area_;}
    const const_image_view_t& src_view() const	{ return src_;}

protected:

    Imath::Box2i src_area_;
    const_image_view_t src_;
};

class point_sampler_t : public sampler_t
{
public:

	point_sampler_t() {}
    point_sampler_t( const const_image_view_t& src);
    point_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const vector2_t& p) const;
    pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const;
};

template<class BaseSampler>
class tile_sampler_t
{
public:

	tile_sampler_t() {}
	tile_sampler_t( const BaseSampler& s) : s_( s) {}

	pixel_t operator()( const vector2_t& p) const { return s_( tile( p));}

    pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return s_( tile( p), du, dv);}

private:

	vector2_t tile( const vector2_t& p) const
	{
		typedef vector2_t::BaseType base_type;

		Imath::V2i size( s_.src_area().size());

		base_type ix, iy;
		base_type x = Imath::Math<base_type>::modf( ( p.x - s_.src_area().min.x) / size.x, &ix);
		base_type y = Imath::Math<base_type>::modf( ( p.y - s_.src_area().min.y) / size.y, &iy);

		if( x < 0) x = base_type( 1.0) + x;
		if( y < 0) y = base_type( 1.0) + y;

		return vector2_t( x * size.x + s_.src_area().min.x, y * size.y + s_.src_area().min.y);
	}

	BaseSampler s_;
};

template<class BaseSampler>
class repeat_sampler_t
{
public:

	repeat_sampler_t() {}
	repeat_sampler_t( const BaseSampler& s) : s_( s) {}

	pixel_t operator()( const vector2_t& p) const { return s_( repeat( p));}

    pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return s_( repeat( p), du, dv);}

private:

	vector2_t repeat( const vector2_t& p) const
	{
		typedef vector2_t::BaseType base_type;

		return vector2_t( Imath::clamp( p.x, ( base_type) s_.src_area().min.x, ( base_type) s_.src_area().max.x),
						Imath::clamp( p.y, ( base_type) s_.src_area().min.y, ( base_type) s_.src_area().max.y));
	}

	BaseSampler s_;
};

template<class BaseSampler>
class mirror_sampler_t
{
public:

	mirror_sampler_t() {}
	mirror_sampler_t( const BaseSampler& s) : s_( s) {}

	pixel_t operator()( const vector2_t& p) const { return s_( mirror( p));}

    pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return s_( mirror( p), du, dv);}

private:

	vector2_t mirror( const vector2_t& p) const
	{
		typedef vector2_t::BaseType base_type;

		Imath::V2i size( s_.src_area().size());

		base_type ix, iy;
		base_type x = Imath::Math<base_type>::modf( Imath::Math<base_type>::fabs( ( p.x - s_.src_area().min.x) / size.x), &ix);
		base_type y = Imath::Math<base_type>::modf( Imath::Math<base_type>::fabs( ( p.y - s_.src_area().min.y) / size.y), &iy);

		if( (int) ix & 1) x = base_type( 1.0) - x;
		if( (int) iy & 1) y = base_type( 1.0) - y;

		return vector2_t( x * size.x + s_.src_area().min.x, y * size.y + s_.src_area().min.y);
	}

	BaseSampler s_;
};

} // namespace
} // namespace
} // namespace

#endif
