// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/camera/syntheyes_lens_distortion.hpp>

#include<algorithm>

#include<boost/tr1/tuple.hpp>
#include<boost/math/tools/roots.hpp>

#include<OpenEXR/ImathMath.h>

namespace ramen
{
namespace camera
{
namespace detail
{

inline float syntheyes_undistort( float k, float k3, float r)
{
	float r2 = r * r;
	float f = 1.0f + r2 * ( k + k3 * r);
	return f * r;
}
	
// quartic equation and derivatives for root finding
struct distort_fun
{
public:

	distort_fun( float k, float k3, float c) : k_(k), k3_(k3), c_(c) {}
	
	std::tr1::tuple<float, float, float> operator()( float r) const
	{
		float r2  = r*r;
		float y	  = ( k3_ * r2 * r2) + ( k_ * r2 * r) + r - c_;
		float dy  = ( 4.0f * k3_ * r2 * r) + ( 3.0f * k_ * r2) + 1.0f;
		float ddy = ( 12.0f * k3_ * r2) + ( 6.0f * k_ * r);
		return std::tr1::tuple<float,float,float>( y, dy, ddy);
	}

	float k_, k3_, c_;
};

float redistort_scale( float k, float k3, float dist)
{
	const int digits = std::numeric_limits<float>::digits / 2;
	
	if( dist == 0.0f)
		return 1.0f;

	float best_guess = dist;
	float min_diff = Imath::Math<float>::fabs( syntheyes_undistort( k, k3, dist) - dist);
	
	// do some iterations to improve the initial guess
	for( int i = 1; i < 10; ++i)
	{
		float r = i * 0.141421f;
		float d = Imath::Math<float>::fabs( syntheyes_undistort( k, k3, r) - dist);

		if( d < min_diff)
		{
			best_guess = r;
			min_diff = d;
		}
	}
	
	float newdist = boost::math::tools::halley_iterate( distort_fun( k, k3, dist), best_guess, 0.0f, 5.0f, digits);
	return newdist / dist;
}

syntheyes_distortion_base::syntheyes_distortion_base( float k, float k3, const Imath::Box2i& format, float pixel_aspect)
{
	k_ = k;
	k3_ = k3;
	pixel_aspect_ = pixel_aspect;
		
	format_.min.x = format.min.x * pixel_aspect;
	format_.max.x = format.max.x * pixel_aspect;
	format_.min.y = format.min.y;
	format_.max.y = format.max.y;
	
	if( format_.size().x > format_.size().y)
	{
		format_aspect_.x = 1;
		format_aspect_.y = (float) format_.size().y / format_.size().x;
	}
	else
	{
		format_aspect_.x = (float) format_.size().x / format_.size().y;
		format_aspect_.y = 1;
	}
}

Imath::V2f syntheyes_distortion_base::transform_to_uv( const Imath::V2f& p) const
{
	Imath::V2f uv;
	
    uv.x = (p.x * pixel_aspect_ - format_.min.x) / (float) format_.size().x;
    uv.y = (p.y - format_.min.y) / (float) format_.size().y;

    // transform to -1..1 taking into acount the aspect ratio
    uv.x = ( uv.x - 0.5f) * 2.0f * format_aspect_.x;
    uv.y = ( uv.y - 0.5f) * 2.0f * format_aspect_.y;
	return uv;
}

Imath::V2f syntheyes_distortion_base::transform_from_uv( const Imath::V2f& uv) const
{	
    // transform back to 0..1
    float u = (uv.x / format_aspect_.x / 2.0f) + 0.5f;
    float v = (uv.y / format_aspect_.y / 2.0f) + 0.5f;

    return Imath::V2f( u * format_.size().x / pixel_aspect_ + format_.min.x,
                       v * format_.size().y + format_.min.y);
}

} // detail

/************************************************************************************************/

syntheyes_quadratic_undistort::syntheyes_quadratic_undistort( float k, const Imath::Box2i& format, 
													float pixel_aspect) : syntheyes_distortion_base( k, 0, format, pixel_aspect)
{
}
		
Imath::V2f syntheyes_quadratic_undistort::operator()( const Imath::V2f& p) const
{
	Imath::V2f uv = transform_to_uv( p);
    float r2 = uv.length2();
    uv *= 1.0f + r2 * k_;
	return transform_from_uv( uv);
}

/************************************************************************************************/

syntheyes_undistort::syntheyes_undistort( float k, float k3, const Imath::Box2i& format, 
													float pixel_aspect) : syntheyes_distortion_base( k, k3, format, pixel_aspect)
{
}
		
Imath::V2f syntheyes_undistort::operator()( const Imath::V2f& p) const
{
	Imath::V2f uv = transform_to_uv( p);
    float r2 = uv.length2();
	float r = Imath::Math<float>::sqrt( r2);	
	uv *= 1.0f + r2 * ( k_ + k3_ * r);
	return transform_from_uv( uv);
}

/************************************************************************************************/

syntheyes_quadratic_redistort::syntheyes_quadratic_redistort( float k, const Imath::Box2i& format, 
															float pixel_aspect) : syntheyes_distortion_base( k, 0, format, pixel_aspect)
{
}

Imath::V2f syntheyes_quadratic_redistort::operator()( const Imath::V2f& p) const
{
	Imath::V2f uv = transform_to_uv( p);
    double rp = uv.length();

	if( k_ < 0)
	{
		double rlim = Imath::Math<float>::sqrt(( -1.0 / 3.0) / k_);
		double rplim = rlim * ( 1 + k_ * rlim * rlim);

		if( rp >= 0.99 * rplim)
		{
			uv *= rlim / rp;
			return transform_from_uv( uv);
		}
	}

	double r = rp;
	
	for( int i = 0; i < 20; ++i)
	{
		double raw = k_ * r * r;
		double slope = 1.0 + 3.0 * raw;
		double err = rp - r * ( 1 + raw);

		if( Imath::Math<float>::fabs( err) < 1.0e-10)
			break;

		r += err / slope;
	}
	
    uv *= r / rp;
	return transform_from_uv( uv);
}

/************************************************************************************************/

syntheyes_redistort::syntheyes_redistort( float k, float k3, const Imath::Box2i& format, 
													float pixel_aspect) : syntheyes_distortion_base( k, k3, format, pixel_aspect)
{
}
		
Imath::V2f syntheyes_redistort::operator()( const Imath::V2f& p) const
{
	Imath::V2f uv = transform_to_uv( p);
    float r = uv.length();
    uv *= detail::redistort_scale( k_, k3_, r);
	return transform_from_uv( uv);
}

} // lens_distortion
} // ramen
