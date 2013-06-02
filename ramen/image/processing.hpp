// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_PROCESSING_HPP
#define	RAMEN_IMAGE_PROCESSING_HPP

#include<ramen/config.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/image/typedefs.hpp>

#include<ramen/image/warp.hpp>

namespace ramen
{
namespace image
{

// basic math ops
extern RAMEN_API boost::function<void ( const image_view_t&)> clear_image;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> add_images;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> sub_images;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&)> mul_images;
extern RAMEN_API boost::function<void ( const const_image_view_t&, float, const image_view_t&)> mul_image_scalar;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> madd_images;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, float, const image_view_t&)> lerp_images;

extern RAMEN_API boost::function<void ( const channel_view_t&)> clear_channel;
extern RAMEN_API boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> add_channels;
extern RAMEN_API boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> sub_channels;
extern RAMEN_API boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, const channel_view_t&)> mul_channels;
extern RAMEN_API boost::function<void ( const const_channel_view_t&, const const_channel_view_t&, float, const channel_view_t&)> madd_channels;
extern RAMEN_API boost::function<void ( const const_channel_view_t&, float, const channel_view_t&)> mul_channel_scalar;

extern RAMEN_API boost::function<void ( const gray_image_view_t&)> clear_gray_image;
extern RAMEN_API boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> add_gray_images;
extern RAMEN_API boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> sub_gray_images;
extern RAMEN_API boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, const gray_image_view_t&)> mul_gray_images;
extern RAMEN_API boost::function<void ( const const_gray_image_view_t&, float, const gray_image_view_t&)> mul_gray_image_scalar;
extern RAMEN_API boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> madd_gray_images;
extern RAMEN_API boost::function<void ( const const_gray_image_view_t&, const const_gray_image_view_t&, float, const gray_image_view_t&)> lerp_gray_images;

// alpha
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> premultiply;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> unpremultiply;

// invert
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgba;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_rgb;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> invert_alpha;
extern RAMEN_API boost::function<void ( const const_channel_view_t&, const channel_view_t&)> invert_channel;

// color space conversions
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_yuv;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_hsv;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_rgb_to_lab;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_yuv_to_rgb;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_hsv_to_rgb;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> convert_lab_to_rgb;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const gray_image_view_t&)> convert_rgb_to_gray;

// color
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&, float)> apply_gamma;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> apply_log;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> apply_pow10;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&, const matrix4_t&)> apply_color_matrix;

// statistics
extern RAMEN_API boost::function<image::pixel_t ( const const_image_view_t&)> mean;

// transform
extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
									     const Imath::Box2i&, const image_view_t&,
									     const matrix3_t&, const matrix3_t&)> affine_warp_nearest;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
										const Imath::Box2i&, const image_view_t&,
										const matrix3_t&, const matrix3_t&)> affine_warp_bilinear;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
		     const Imath::Box2i&, const image_view_t&,
		     const matrix3_t&, const matrix3_t&)> affine_warp_catrom;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
										 const Imath::Box2i&, const image_view_t&,
										 const matrix3_t&, const matrix3_t&)> projective_warp_nearest;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
										 const Imath::Box2i&, const image_view_t&,
										 const matrix3_t&, const matrix3_t&)> projective_warp_bilinear;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
										 const Imath::Box2i&, const image_view_t&,
										 const matrix3_t&, const matrix3_t&)> projective_warp_catrom;

// transform + non-black border modes
extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
										 const Imath::Box2i&, const image_view_t&,
										 const matrix3_t&, const matrix3_t&)> affine_warp_bilinear_tile;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
										 const Imath::Box2i&, const image_view_t&,
										 const matrix3_t&, const matrix3_t&)> affine_warp_bilinear_mirror;

// resize
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&)> resize_bilinear;

extern RAMEN_API boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&,
								   const image::image_view_t&, const Imath::Box2i&)> resize_half;

extern boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&, const Imath::Box2i&,
							   const image::image_view_t&, const Imath::Box2i&, const image::image_view_t&,
							  const Imath::Box2i&, const Imath::V2i&, const Imath::V2f&)> resize_lanczos3_;

extern boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&, const Imath::Box2i&,
							   const image::image_view_t&, const Imath::Box2i&, const image::image_view_t&,
							  const Imath::Box2i&, const Imath::V2i&, const Imath::V2f&)> resize_mitchell_;

extern boost::function<void ( const image::const_image_view_t&, const Imath::Box2i&, const Imath::Box2i&,
							   const image::image_view_t&, const Imath::Box2i&, const image::image_view_t&,
							  const Imath::Box2i&, const Imath::V2i&, const Imath::V2f&)> resize_catrom_;

// layer
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, 
										const image_view_t&, float)> composite_over;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
										const image_view_t&, float)> composite_add;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mul;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_sub;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_mix;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_screen;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_overlay;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&,
                                                float)> composite_zero_overlay;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_diff;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_max;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
				const image_view_t&, float)> composite_min;

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_add;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_mul;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_sub;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_mix;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_max;
extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&, const image_view_t&, float)> alpha_composite_min;

// warps

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bilinear;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bilinear_tile;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bilinear_mirror;

extern RAMEN_API boost::function<void ( const Imath::Box2i&, const const_image_view_t&,
				const Imath::Box2i&, const image_view_t&,
				const warp_function_t&, bool, bool)> warp_bicubic;

// key mix

extern RAMEN_API boost::function<void ( const const_image_view_t&, const const_image_view_t&,
                                const const_channel_view_t&, const image_view_t&)> key_mix;

// convolve
extern RAMEN_API boost::function<void ( const const_image_view_t&, const image_view_t&, const Imath::M33f&)> convolve33;

// box blur
extern boost::function<void ( const const_image_view_t&, const image_view_t&,
							  const image_view_t&, float, float, int iters)> box_blur_rgba_;

extern boost::function<void ( const const_channel_view_t&, const boost::gil::gray32f_view_t&,
							  const channel_view_t&, float, float, int)>  box_blur_channel_;

extern boost::function<void ( const boost::gil::gray32fc_view_t&, const boost::gil::gray32f_view_t&,
							  const boost::gil::gray32f_view_t&, float, float, int)> box_blur_gray_;

// gaussian blur
extern boost::function<void ( const const_image_view_t&, const image_view_t&, 
							  const image_view_t&, float, float)> gaussian_blur_rgba_;

extern boost::function<void( const const_rgb_image_view_t&, const rgb_image_view_t&,
							 const rgb_image_view_t&, float, float)> gaussian_blur_rgb_;

extern boost::function<void ( const const_channel_view_t&, const boost::gil::gray32f_view_t&,
							  const channel_view_t&, float, float)>  gaussian_blur_channel_;

extern boost::function<void ( const boost::gil::gray32fc_view_t&, const boost::gil::gray32f_view_t&,
							  const boost::gil::gray32f_view_t&, float, float)> gaussian_blur_gray_;

} // namespace
} // namespace

#endif
