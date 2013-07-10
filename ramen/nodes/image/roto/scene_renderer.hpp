// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_SCENE_RENDERER_HPP
#define RAMEN_ROTO_SCENE_RENDERER_HPP

#include<vector>

#include<boost/noncopyable.hpp>

#include<OpenEXR/ImathBox.h>

#include<agg2/agg_renderer_scanline.h>
#include<agg2/agg_rasterizer_scanline_aa.h>
#include<agg2/agg_scanline_u.h>
#include<agg2/agg_path_storage.h>

#include<ramen/image/typedefs.hpp>
#include<ramen/image/agg_renderers.hpp>

#include<ramen/nodes/image/roto/scene_fwd.hpp>
#include<ramen/nodes/image/roto/shape_fwd.hpp>

namespace ramen
{
namespace roto
{

class scene_renderer_t : boost::noncopyable
{
public:

    scene_renderer_t( const scene_t& scene, const Imath::Box2i& area, float aspect, int subsample);
	
	void render();
	
	image::const_gray_image_view_t const_view() const	{ return boost::gil::const_view( pixels_);}
	image::gray_image_view_t view()						{ return boost::gil::view( pixels_);}
	
private:

	void render_shape( const shape_t& s);
	void render_and_filter_shape( const shape_t& s, int bbox_index);
	
	void convert_to_path( const shape_t& s, agg::path_storage& path, const Imath::V2i& offset, int subsample) const;
	Imath::Box2f calc_bbox( const shape_t& s, int subsample);

	Imath::V2f transform_point( const Imath::V2d& p, const Imath::V2f& shape_offset, 
								  const Imath::M33f& m, int subsample, const Imath::V2i& offset) const;
	
	const scene_t& scene_;
	image::gray_image_t pixels_;
    Imath::Box2i area_;
    int subsample_;
	float aspect_;
	
	// agg things
    typedef image::agg_gray32f_renderer_t<boost::gil::gray32f_view_t> ren_base_type;
    typedef ren_base_type::color_type color_type;
    typedef agg::renderer_scanline_aa_solid<ren_base_type> renderer_type;
	
    ren_base_type ren_base_;
    agg::scanline_u8 sl_;
    agg::rasterizer_scanline_aa<> ras_;
    renderer_type ren_;

    agg::path_storage path_;
	
	// for filters
	std::vector<Imath::Box2i> bboxes_, filtered_bboxes_;
	image::gray_image_t buf_, tmp_;
};
	
} // roto
} // ramen

#endif
