// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/time/retime_node.hpp>

#include<OpenEXR/ImathMath.h>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/render/context_guard.hpp>
#include<ramen/render/image_node_renderer.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/color.hpp>

#include<ramen/image/generic/samplers.hpp>
#include<ramen/image/sse2/samplers.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
	timing_src_frame = 0,
	timing_speed
};

enum
{
	interp_nearest = 0,
	interp_blend,
	interp_motion
};

// motion vectors warp code...

template<class Sampler>
struct flow_warp_fun
{
	flow_warp_fun( const image::const_image_view_t& src, const Imath::Box2i& src_area,
				   const image::const_image_view_t& flow, const Imath::Box2i& flow_area,
				   int flow_ch_offset, const Imath::V2f& flow_scale, 
				   const image::image_view_t& dst, const Imath::Box2i& dst_area) : src_sampler_( src_area, src), flow_sampler_( flow_area, flow), dst_( dst)
	{
		flow_channel_offset_ = flow_ch_offset;
		flow_scale_ = flow_scale;
		dst_area_ = dst_area;
	}
	
	void operator()( const tbb::blocked_range<int>& r) const
	{
		for( int y = r.begin(); y < r.end(); ++y)
			for( int x = dst_area_.min.x, xe = dst_area_.max.x + 1; x < xe; ++x)
				dst_( x - dst_area_.min.x, y - dst_area_.min.y) = (*this)( Imath::V2d( x, y));
	}
	
	inline image::pixel_t operator()( const Imath::V2d& p) const
	{
		image::pixel_t px( flow_sampler_( p));
		Imath::V2d uv( px[flow_channel_offset_] * flow_scale_.x, px[flow_channel_offset_ + 1] * flow_scale_.y);
		return src_sampler_( p - uv);
	}
	
private:
	
	Sampler src_sampler_;

	image::generic::point_sampler_t flow_sampler_;
	int flow_channel_offset_;
	Imath::V2f flow_scale_;

	const image::image_view_t& dst_;
	Imath::Box2i dst_area_;	
};

template<class Sampler>
struct flow_warp_mix_fun
{

	flow_warp_mix_fun( const image::const_image_view_t& src, const Imath::Box2i& src_area,
					   const image::const_image_view_t& flow, const Imath::Box2i& flow_area,
					   int flow_ch_offset, const Imath::V2f& flow_scale, 
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					   float mix) : flow_warp_( src, src_area, flow, flow_area, flow_ch_offset, flow_scale, dst, dst_area), dst_( dst)
	{
		dst_area_ = dst_area;
		mix_ = mix;
	}
	
	void operator()( const tbb::blocked_range<int>& r) const
	{
		for( int y = r.begin(); y < r.end(); ++y)
		{
			for( int x = dst_area_.min.x, xe = dst_area_.max.x + 1; x < xe; ++x)
			{
				image::pixel_t p( dst_( x - dst_area_.min.x, y - dst_area_.min.y));
				image::pixel_t q( flow_warp_( Imath::V2d( x, y)));
				dst_( x - dst_area_.min.x, y - dst_area_.min.y) = image::lerp_pixels( p, q, mix_);
			}
		}
	}
	
private:

	flow_warp_fun<Sampler> flow_warp_;
	float mix_;

	const image::image_view_t& dst_;
	Imath::Box2i dst_area_;	
};

} // unnamed

retime_node_t::retime_node_t() : image_node_t()
{
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_input_plug( "mvectors", true, ui::palette_t::instance().color( "back plug"), "Motion Vectors");
    add_output_plug();
	set_name( "retime");
}

retime_node_t::retime_node_t( const retime_node_t& other) : image_node_t( other) {}

void retime_node_t::do_create_params()
{
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Retime"));
    top->set_id( "retime");
	// frame
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Src Frame"));
        std::auto_ptr<float_param_t> p( new float_param_t( "Src Frame"));
        p->set_id( "src_frame");
        p->set_default_value( 1);
		group->add_param( p);

        top->add_param( group);
    }
	// speed
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Speed"));
		std::auto_ptr<float_param_t> p( new float_param_t( "Speed"));
		p->set_id( "speed");
		p->set_min( 0);
		p->set_default_value( 1);
		group->add_param( p);

		top->add_param( group);
    }
    add_param( top);	

	//----------------------------------------------------//
	
    top.reset( new combo_group_param_t( "Interpolation"));
    top->set_id( "interp");
	// nearest
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Nearest"));
		top->add_param( group);
    }
	// blend
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Blend"));
		top->add_param( group);
    }
	// motion
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Motion"));
		
		std::auto_ptr<bool_param_t> bidir( new bool_param_t( "Bidirectional Vectors"));
		bidir->set_id( "bidirect");
		bidir->set_default_value( false);
		group->add_param( bidir);
		
		/*
		std::auto_ptr<popup_param_t> filter( new popup_param_t( "Filter"));
		filter->set_id( "filter");
		filter->menu_items() = boost::assign::list_of( "Bilinear")( "Catrom");
		filter->set_default_value( 0);
		group->add_param( filter);
		*/
		
		top->add_param( group);
    }
    add_param( top);	
}

bool retime_node_t::do_is_valid() const
{
	if( get_value<int>( param( "interp")) == interp_motion)
		return input( 1) != 0;
	
	return true;
}

bool retime_node_t::do_is_identity() const
{ 
	return get_src_frame() == composition()->frame();
}

bool retime_node_t::include_input_in_hash( int num) const
{
	if( num == 1)
		return get_value<int>( param( "interp")) == interp_motion;
	
	return true;	
}

void retime_node_t::do_calc_frames_needed( const render::context_t& context)
{
	float f = get_src_frame();
	int frame = Imath::Math<float>::floor( f);		

	switch( get_value<int>( param( "interp")))
	{
		case interp_nearest:
			frames_needed().push_back( std::pair<int,int>( f, 0));
		break;
	
		case interp_blend:
			frames_needed().push_back( std::pair<int,int>( frame, 0));
		
			if( f - frame != 0)
				frames_needed().push_back( std::pair<int,int>( frame + 1, 0));
		break;
		
		case interp_motion:
			RAMEN_ASSERT( input( 1));
			frames_needed().push_back( std::pair<int,int>( frame, 1));
			frames_needed().push_back( std::pair<int,int>( frame + 1, 0));
			
			if( get_value<bool>( param( "bidirect")))
				frames_needed().push_back( std::pair<int,int>( frame, 0));
		break;
	}
}

void retime_node_t::do_calc_bounds( const render::context_t& context)
{
	render::context_t c( context);
	float f = get_src_frame();
	int frame = Imath::Math<float>::floor( f);
	c.result_node = input( 0);
	
	switch( get_value<int>( param( "interp")))
	{
		case interp_nearest:
		{
			c.frame = ( int) f;
			set_bounds( get_input_bounds( c));
		}
		break;
		
		case interp_blend:
		{
			c.frame = frame;
			Imath::Box2i box( get_input_bounds( c));
			
			if( f - frame != 0)
			{
				c.frame = frame + 1;
				box.extendBy( get_input_bounds( c));
			}

			set_bounds( box);
		}
		break;
		
		case interp_motion:
		{
			c.frame = frame + 1;
			Imath::Box2i box( get_input_bounds( c));

			if( get_value<bool>( param( "bidirect")))
			{
				c.frame = frame;
				box.extendBy( get_input_bounds( c));
			}

			set_bounds( box);
		}
		break;
	}
}

void retime_node_t::do_calc_inputs_interest( const render::context_t& context)
{
	// Here, we only have to save the interest
	full_interest_ = interest();
}

void retime_node_t::do_recursive_process( const render::context_t& context)
{	
	if( !context.render_cancelled())
	{
		alloc_image();
		do_retime( context);
	}
}

void retime_node_t::do_retime( const render::context_t& context)
{
	render::context_t ctx( context);
	int interp = get_value<int>( param( "interp"));
	
	float f = get_src_frame();
	int frame = Imath::Math<float>::floor( f);
	float fract = f - frame;

	if( interp == interp_motion)
	{
		// get motion vectors
		ctx.frame = frame;
		ctx.result_node = input( 1);
		image::buffer_t flow = get_input_image( ctx, full_interest_);

		// get images
		ctx.result_node = input( 0);
		ctx.frame = frame + 1;
		image::buffer_t pixels = get_input_image( ctx);
		
		Imath::V2f flow_scale( ( 1.0f - fract) / context.subsample / proxy_scale().x,
							   ( 1.0f - fract) / context.subsample / proxy_scale().y);
		
		tbb::parallel_for( tbb::blocked_range<int>( defined().min.y, defined().max.y + 1),
							flow_warp_fun<sse2::bilinear_sampler_t>( pixels.const_rgba_view(), pixels.bounds(),
																	flow.const_rgba_view(), flow.bounds(), 
																	0, flow_scale, image_view(), defined()));

		 if( get_value<bool>( param( "bidirect")))
		 {
			 ctx.frame = frame;
			image::buffer_t pixels = get_input_image( ctx);

			flow_scale = Imath::V2f( fract / context.subsample / proxy_scale().x,
									 fract / context.subsample / proxy_scale().y);
			
			tbb::parallel_for( tbb::blocked_range<int>( defined().min.y, defined().max.y + 1),
								flow_warp_mix_fun<sse2::bilinear_sampler_t>( pixels.const_rgba_view(), pixels.bounds(),
																			flow.const_rgba_view(), flow.bounds(), 
																			2, flow_scale, image_view(), defined(), fract));
		 }

		 return;
	}
	
	if( interp == interp_nearest || fract == 0)
	{
		ctx.frame = ( int) f;
		ctx.result_node = input( 0);
		image::buffer_t pixels = get_input_image( ctx, full_interest_);
		
		Imath::Box2i area = ImathExt::intersect( pixels.bounds(), defined());
		
		if( !area.isEmpty())
			boost::gil::copy_pixels( pixels.const_rgba_subimage_view( area), subimage_view( area));
		
		return;
	}
	
	// blend interpolation case.

	// first frame
	ctx.frame = frame;
	ctx.result_node = input( 0);
	image::buffer_t pixels = get_input_image( ctx, full_interest_);
	Imath::Box2i area = ImathExt::intersect( pixels.bounds(), defined());
		
	if( !area.isEmpty())
		boost::gil::copy_pixels( pixels.const_rgba_subimage_view( area), subimage_view( area));
		
	// second frame
	ctx.frame = frame + 1;
	pixels = get_input_image( ctx, full_interest_);
	area = ImathExt::intersect( pixels.bounds(), defined());
	
	if( !area.isEmpty())
		image::lerp_images( const_subimage_view( area), pixels.const_rgba_subimage_view( area), 1.0f - fract, subimage_view( area));
}

float retime_node_t::get_src_frame() const
{
	if( get_value<int>( param( "retime")) == timing_speed)
	{
		const float_param_t *p = dynamic_cast<const float_param_t*>( &param( "speed"));
		RAMEN_ASSERT( p);

		return p->integrate( 0, composition()->frame());
	}
	else
		return get_value<float>( param( "src_frame"));
}

void retime_node_t::release_image() { image_node_t::release_image();}

Imath::Box2i retime_node_t::get_input_bounds( const render::context_t& context)
{
	render::context_guard_t guard( composition()->current_context(), context.result_node);
	render::image_node_renderer_t r( context);
	return r.bounds();
}

image::buffer_t retime_node_t::get_input_image( const render::context_t& context)
{
	render::context_guard_t guard( composition()->current_context(), context.result_node);	
	render::image_node_renderer_t r( context);
	r.render( r.bounds());
	return r.image();
}

image::buffer_t retime_node_t::get_input_image( const render::context_t& context, const Imath::Box2i& roi)
{
	RAMEN_ASSERT( !roi.isEmpty());

	render::context_guard_t guard( composition()->current_context(), context.result_node);	
	render::image_node_renderer_t r( context);
	r.render( roi);
	return r.image();	
}

// factory
node_t *create_retime_node() { return new retime_node_t();}

const node_metaclass_t *retime_node_t::metaclass() const { return &retime_node_metaclass();}

const node_metaclass_t& retime_node_t::retime_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.retime";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Time";
		info.menu_item = "Retime";
		info.create = &create_retime_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( retime_node_t::retime_node_metaclass());
	
} // namespace
} // namespace

