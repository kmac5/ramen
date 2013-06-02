// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/image/processing.hpp>

#include<ramen/image/generic/affine_warp.hpp>
#include<ramen/image/generic/alpha_composite.hpp>
#include<ramen/image/generic/apply_color_matrix.hpp>
#include<ramen/image/generic/apply_gamma_exp_log.hpp>
#include<ramen/image/generic/box_blur_channel.hpp>
#include<ramen/image/generic/composite.hpp>
#include<ramen/image/generic/convolve33.hpp>
#include<ramen/image/generic/gaussian_blur_channel.hpp>
#include<ramen/image/generic/gaussian_blur_rgb.hpp>
#include<ramen/image/generic/invert.hpp>
#include<ramen/image/generic/math.hpp>
#include<ramen/image/generic/premultiply.hpp>
#include<ramen/image/generic/projective_warp.hpp>
#include<ramen/image/generic/resize.hpp>
#include<ramen/image/generic/rgb_to_yuv.hpp>
#include<ramen/image/generic/rgb_to_hsv.hpp>
#include<ramen/image/generic/rgb_to_lab.hpp>
#include<ramen/image/generic/rgb_to_gray.hpp>
#include<ramen/image/generic/stats.hpp>

#include<ramen/image/sse2/composite.hpp>
#include<ramen/image/sse2/affine_warp.hpp>
#include<ramen/image/sse2/box_blur.hpp>
#include<ramen/image/sse2/gaussian_blur.hpp>
#include<ramen/image/sse2/key_mix.hpp>
#include<ramen/image/sse2/math.hpp>
#include<ramen/image/sse2/projective_warp.hpp>
#include<ramen/image/sse2/resize.hpp>
#include<ramen/image/sse2/warp.hpp>

namespace ramen
{
namespace image
{	
	
// basic math
boost::function<void ( const image_view_t&)> clear_image = generic::clear_image;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> add_images = sse2::add_images;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> sub_images = sse2::sub_images;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> mul_images = sse2::mul_images;
boost::function<void ( const const_image_view_t&, float, const image_view_t&)> mul_image_scalar = sse2::mul_image_scalar;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> madd_images = sse2::madd_images;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> lerp_images = sse2::lerp_images;

boost::function<void ( const channel_view_t&)> clear_channel= generic::clear_channel;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> add_channels = generic::add_channels;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> sub_channels = generic::sub_channels;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> mul_channels = generic::mul_channels;
boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, float, const channel_view_t&)> madd_channels = generic::madd_channels;
boost::function<void ( const const_channel_view_t&, float, const channel_view_t&)> mul_channel_scalar = generic::mul_channel_scalar;

boost::function<void ( const gray_image_view_t&)> clear_gray_image;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> add_gray_images = generic::add_gray_images;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> sub_gray_images = generic::sub_gray_images;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> mul_gray_images = generic::mul_gray_images;
boost::function<void ( const const_gray_image_view_t&, float, const gray_image_view_t&)> mul_gray_image_scalar = generic::mul_gray_image_scalar;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> madd_gray_images = generic::madd_gray_images;
boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> lerp_gray_images = generic::lerp_gray_images;

// channels
boost::function<void ( const const_image_view_t&, const image_view_t&)> premultiply = generic::premultiply;
boost::function<void ( const const_image_view_t&, const image_view_t&)> unpremultiply = generic::unpremultiply;

boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgba = generic::invert_rgba;
boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgb = generic::invert_rgb;
boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_alpha = generic::invert_alpha;
boost::function<void ( const const_channel_view_t&, const channel_view_t&)> invert_channel = generic::invert_channel;

// color space
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_yuv = generic::convert_rgb_to_yuv;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_hsv = generic::convert_rgb_to_hsv;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_lab = generic::convert_rgb_to_lab;

boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_yuv_to_rgb = generic::convert_yuv_to_rgb;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_hsv_to_rgb = generic::convert_hsv_to_rgb;
boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_lab_to_rgb = generic::convert_lab_to_rgb;

boost::function<void ( const const_image_view_t&, const gray_image_view_t&)> convert_rgb_to_gray = generic::convert_rgb_to_gray;

// color
boost::function<void ( const const_image_view_t&, const image_view_t&, float)> apply_gamma = generic::apply_gamma;
boost::function<void ( const const_image_view_t&, const image_view_t&)> apply_log = generic::apply_log;
boost::function<void ( const const_image_view_t&, const image_view_t&)> apply_pow10 = generic::apply_pow10;
boost::function<void ( const const_image_view_t&, const image_view_t&, const matrix4_t&)> apply_color_matrix = generic::apply_color_matrix;

// stats
boost::function<image::pixel_t ( const const_image_view_t&)> mean = generic::mean;

// transform
boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> affine_warp_nearest = generic::affine_warp_nearest;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> affine_warp_bilinear = sse2::affine_warp_bilinear;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> affine_warp_catrom = sse2::affine_warp_catrom;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> projective_warp_nearest = generic::projective_warp_nearest;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> projective_warp_bilinear = sse2::projective_warp_bilinear;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> projective_warp_catrom = sse2::projective_warp_catrom;

// transform + non black border modes
boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> affine_warp_bilinear_tile = sse2::affine_warp_bilinear_tile;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&,
		    const matrix3_t&)> affine_warp_bilinear_mirror = sse2::affine_warp_bilinear_mirror;

// resize

boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_bilinear = sse2::resize_bilinear;

boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&,
					   const image::image_view_t&, const Imath::Box2i&)> resize_half = sse2::resize_half;

boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&, const Imath::Box2i&,
					   const image::image_view_t&, const Imath::Box2i&, const image::image_view_t&,
					  const Imath::Box2i&, const Imath::V2i&, const Imath::V2f&)> resize_lanczos3_ = sse2::resize_lanczos3;

boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&, const Imath::Box2i&,
					   const image::image_view_t&, const Imath::Box2i&, const image::image_view_t&,
					  const Imath::Box2i&, const Imath::V2i&, const Imath::V2f&)> resize_mitchell_ = sse2::resize_mitchell;

boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&, const Imath::Box2i&,
					   const image::image_view_t&, const Imath::Box2i&, const image::image_view_t&,
					  const Imath::Box2i&, const Imath::V2i&, const Imath::V2f&)> resize_catrom_ = sse2::resize_catrom;

// composite
boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_over = sse2::composite_over;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_add = sse2::composite_add;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mul = sse2::composite_mul;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_sub = sse2::composite_sub;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mix = sse2::composite_mix;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_screen = generic::composite_screen;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_overlay = generic::composite_overlay;

boost::function<void ( const const_image_view_t&, const image_view_t&,
                                                float)> composite_zero_overlay = generic::composite_zero_overlay;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_diff = generic::composite_diff;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_max = sse2::composite_max;

boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_min = sse2::composite_min;

boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_add = generic::alpha_composite_add;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_mul = generic::alpha_composite_mul;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_sub = generic::alpha_composite_sub;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_mix = generic::alpha_composite_mix;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_max = generic::alpha_composite_max;
boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_min = generic::alpha_composite_min;

// warps
boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bilinear = sse2::warp_bilinear;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bilinear_tile = sse2::warp_bilinear_tile;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bilinear_mirror = sse2::warp_bilinear_mirror;

boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bicubic = sse2::warp_bicubic;

// key mix
boost::function<void ( const const_image_view_t&, const const_image_view_t&,
                       const const_channel_view_t&, const image_view_t&)> key_mix = sse2::key_mix;

// convolve
boost::function<void ( const const_image_view_t&, const image_view_t&, const Imath::M33f&)> convolve33 = generic::convolve33;

// box blur
boost::function<void ( const const_image_view_t&, const image_view_t&, 
					   const image_view_t&, float, float, int iters)> box_blur_rgba_ = sse2::box_blur_rgba;

boost::function<void ( const const_channel_view_t&, const boost::gil::gray32f_view_t&,
					   const channel_view_t&, float, float, int)>  box_blur_channel_ = generic::box_blur_channel;

boost::function<void ( const boost::gil::gray32fc_view_t&, const boost::gil::gray32f_view_t&,
					   const boost::gil::gray32f_view_t&, float, float, int)> box_blur_gray_ = generic::box_blur_gray;

// gaussian blur
boost::function<void ( const const_image_view_t&, const image_view_t&, 
					   const image_view_t&, float, float)> gaussian_blur_rgba_ = sse2::gaussian_blur_rgba;

boost::function<void( const const_rgb_image_view_t&, const rgb_image_view_t&,
					  const rgb_image_view_t&, float, float)> gaussian_blur_rgb_ = generic::gaussian_blur_rgb;

boost::function<void ( const const_channel_view_t&, const boost::gil::gray32f_view_t&,
						  const channel_view_t&, float, float)>  gaussian_blur_channel_ = generic::gaussian_blur_channel;

boost::function<void ( const boost::gil::gray32fc_view_t&, const boost::gil::gray32f_view_t&,
						  const boost::gil::gray32f_view_t&, float, float)> gaussian_blur_gray_ = generic::gaussian_blur_gray;

} // namespace
} // namespace
