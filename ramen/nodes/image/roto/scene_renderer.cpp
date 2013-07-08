// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/scene_renderer.hpp>

#include<cmath>
#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<agg2/agg_conv_curve.h>
#include<agg2/agg_bounding_rect.h>

#include<ramen/assert.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/nodes/image/roto/scene.hpp>

#include<ramen/image/box_blur.hpp>
#include<ramen/image/dilate.hpp>

namespace ramen
{
namespace roto
{
namespace
{
	
struct composite_layer
{
public:
	
	composite_layer( float c, float o) : color_( c), opacity_( o) {}
	
	boost::gil::gray32f_pixel_t operator()( const boost::gil::gray32f_pixel_t& fg, const boost::gil::gray32f_pixel_t& bg) const
	{
		float a = fg[0] * opacity_;
		float a_inv = 1.0f - a;
        return boost::gil::gray32f_pixel_t( clamp( ( color_ * a) + ( bg[0] * a_inv), 0.0f, 1.0f));
	}
	
private:
	
	float color_;
	float opacity_;
};
	
} // unnamed

scene_renderer_t::scene_renderer_t( const scene_t& scene, const Imath::Box2i& area, float aspect, int subsample) : scene_( scene)
{
	area_ = area;
	aspect_ = aspect;
	subsample_ = subsample;
    pixels_.recreate( area_.size().x + 1, area_.size().y + 1);
	boost::gil::fill_pixels( boost::gil::view( pixels_), image::gray_pixel_t( 0));
	
    ren_base_.set_view( boost::gil::view( pixels_));
    ren_.attach( ren_base_);
    ras_.gamma( agg::gamma_none());

	// find max temp buffer area needed.
	int max_width = 0;
	int max_height = 0;
	
	BOOST_FOREACH( const shape_t& s, scene_)
	{
		if( s.is_null() || !s.active() || s.opacity() == 0.0f)
			continue;

		Imath::V2f blur = s.blur();
		float grow = s.grow();
		
		if( blur.x == 0 && blur.y == 0 && grow == 0)
			continue;
		
		float g = std::max( grow, 0.0f);
		
		// calc an accurate bounding box and save it for later reuse.
		Imath::Box2f box( calc_bbox( s, subsample_));
		box = ImathExt::intersect( Imath::Box2f( area_.min, area_.max), box);
        bboxes_.push_back( ImathExt::roundBox( box, true));

		// expand the box to take into account the filters.
		box.min.x -= std::ceil( (double) g / aspect_ / subsample_) +
					 std::ceil( (double) blur.x / aspect_ / subsample_) + 1;

		box.min.y -= std::ceil( (double) g / subsample_) +
					 std::ceil( (double) blur.y / subsample_) + 1;

		box.max.x += std::ceil( (double) g / aspect_ / subsample_) +
					 std::ceil( (double) blur.x / aspect_ / subsample_) + 1;
		
		box.max.y += std::ceil( (double) g / subsample_) +
					 std::ceil( (double) blur.y / subsample_) + 1;

        filtered_bboxes_.push_back( ImathExt::roundBox( box, true));
		max_width = std::max( max_width  , filtered_bboxes_.back().size().x);
		max_height = std::max( max_height, filtered_bboxes_.back().size().y);
	}
	
	if( max_width != 0 && max_height != 0)
	{
		buf_.recreate( max_width, max_height);
		tmp_.recreate( max_height, max_width);
	}
}

void scene_renderer_t::render()
{
	int bbox_index = 0;
	
	BOOST_FOREACH( const shape_t& s, scene_)
	{
		if( !s.is_null() && s.active() && s.opacity() != 0.0f)
		{
			Imath::V2f blur = s.blur();
			float grow = s.grow();
		
			if( blur.x == 0 && blur.y == 0 && grow == 0)
				render_shape( s);
			else
			{
				render_and_filter_shape( s, bbox_index);
				++bbox_index;
			}
		}
	}
}

void scene_renderer_t::render_shape( const shape_t& s)
{
    ras_.reset();
	convert_to_path( s, path_, area_.min, subsample_);
    agg::conv_curve<agg::path_storage> cpath( path_);
    ras_.add_path( cpath);	
    ras_.filling_rule( agg::fill_non_zero);
    ren_.color( color_type( s.color(), s.opacity()));
    agg::render_scanlines( ras_, sl_, ren_);
}

void scene_renderer_t::render_and_filter_shape( const shape_t& s, int bbox_index)
{
	RAMEN_ASSERT( bbox_index >= 0 && bbox_index <= filtered_bboxes_.size());
	
	boost::gil::fill_pixels( boost::gil::view( buf_), image::gray_pixel_t( 0));
	boost::gil::fill_pixels( boost::gil::view( tmp_), image::gray_pixel_t( 0));
	
    ren_base_type ren_base;
    agg::scanline_u8 sl;
    agg::rasterizer_scanline_aa<> ras;
    renderer_type ren;

    ren_base.set_view( boost::gil::view( buf_));
    ren.attach( ren_base);
    ras.gamma( agg::gamma_none());
	
	convert_to_path( s, path_, filtered_bboxes_[bbox_index].min, subsample_);
    agg::conv_curve<agg::path_storage> cpath( path_);
    ras.add_path( cpath);
    ras.filling_rule( agg::fill_non_zero);
    ren.color( color_type( 1.0f, 1.0f));
    agg::render_scanlines( ras, sl, ren);

	image::gray_image_view_t buf_view( boost::gil::view( buf_));
	
	float g = s.grow();
	if( g != 0.0f)
	{
		/*
		image::gray_image_view_t subbuf_view( boost::gil::subimage_view( buf_view, 
																		 bboxes_[bbox_index].min.x - filtered_bboxes_[bbox_index].min.x,
																		 bboxes_[bbox_index].min.y - filtered_bboxes_[bbox_index].min.y,
																		 bboxes_[bbox_index].size().x + 1, bboxes_[bbox_index].size().y + 1));
		
		image::dilate( subbuf_view, boost::gil::view( tmp_), subbuf_view, g / aspect_ / subsample_, g / subsample_);
		*/
		boost::gil::fill_pixels( boost::gil::view( tmp_), image::gray_pixel_t( 0));
		image::dilate( buf_view, boost::gil::view( tmp_), buf_view, g / aspect_ / subsample_, g / subsample_);
	}

	Imath::V2f blur = s.blur();
	if( blur.x != 0.0f || blur.y != 0.0f)
	{
		boost::gil::fill_pixels( boost::gil::view( tmp_), image::gray_pixel_t( 0));
		image::box_blur_gray( buf_view, boost::gil::view( tmp_), buf_view, blur.x / aspect_ / subsample_, blur.y / subsample_, 1);
	}
	
	Imath::Box2i common_area = ImathExt::intersect( area_, filtered_bboxes_[bbox_index]);
	
	if( !common_area.isEmpty())
	{
		
		image::gray_image_view_t bg_view( boost::gil::subimage_view( boost::gil::view( pixels_), 
																	 common_area.min.x - area_.min.x, 
																	 common_area.min.y - area_.min.y, 
																	 common_area.size().x, 
																	 common_area.size().y));
		
		boost::gil::tbb_transform2_pixels( boost::gil::subimage_view( buf_view, 
																	  common_area.min.x - filtered_bboxes_[bbox_index].min.x, 
																	  common_area.min.y - filtered_bboxes_[bbox_index].min.y, 
																	  common_area.size().x, 
																	  common_area.size().y),
										   bg_view, bg_view, composite_layer( s.color(), s.opacity()));
	}
}

void scene_renderer_t::convert_to_path( const shape_t& s, agg::path_storage& path, const Imath::V2i& offset, int subsample) const
{
	path.remove_all();
	
	Imath::V2f p0, p1, p2;
	Imath::V2f shape_offset = s.offset();

	Imath::M33f m( s.global_xform());
	
	p0 = transform_point( s.triples()[0].p1(), shape_offset, m, subsample, offset);
	path.move_to( p0.x, p0.y);
	
	for( int i = 0; i < s.triples().size() - 1; ++i)
	{
		p2 = transform_point( s.triples()[i].p2(), shape_offset, m, subsample, offset);
		p0 = transform_point( s.triples()[i+1].p0(), shape_offset, m, subsample, offset);
		p1 = transform_point( s.triples()[i+1].p1(), shape_offset, m, subsample, offset);
		path.curve4( p2.x, p2.y, p0.x, p0.y, p1.x, p1.y);
	}

	// last segment
	p2 = transform_point( s.triples()[s.triples().size()-1].p2(), shape_offset, m, subsample, offset);
	p0 = transform_point( s.triples()[0].p0(), shape_offset, m, subsample, offset);
	p1 = transform_point( s.triples()[0].p1(), shape_offset, m, subsample, offset);
	path.curve4( p2.x, p2.y, p0.x, p0.y, p1.x, p1.y);
	path.close_polygon();
}

Imath::Box2f scene_renderer_t::calc_bbox( const shape_t& s, int subsample)
{
	Imath::Box2f box;
	agg::path_storage path;

	convert_to_path( s, path, Imath::V2i( 0, 0), subsample);
    agg::conv_curve<agg::path_storage> cpath( path);
	agg::bounding_rect_single( cpath, 0, &(box.min.x), &(box.min.y), &(box.max.x), &(box.max.y));	
	return box;
}

Imath::V2f scene_renderer_t::transform_point( const Imath::V2d& p, const Imath::V2f& shape_offset, 
											  const Imath::M33f& m, int subsample, const Imath::V2i& offset) const
{
	Imath::V2f q = ( p + shape_offset) * m;
	return ( q / subsample) - offset;
}

} // roto
} // ramen
