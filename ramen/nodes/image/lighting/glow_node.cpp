// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/lighting/glow_node.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/float3_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>
#include<ramen/gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/color.hpp>
#include<ramen/image/box_blur.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct thereshold_fun
{
	thereshold_fun( float th) : th_( th) {}
	
	image::pixel_t operator()( const image::pixel_t& p) const
	{
		float l = image::luminance( p);
		
		if( l >= th_)
			return p;

		return image::pixel_t( 0, 0, 0, 0);
	}

private:
	
	float th_;
};

struct thereshold_and_mask_fun
{
	thereshold_and_mask_fun( float th) : th_( th) {}
	
	image::pixel_t operator()( const image::pixel_t& p, const image::pixel_t& m) const
	{
		float r = boost::gil::get_color( p, boost::gil::red_t());
		float g = boost::gil::get_color( p, boost::gil::green_t());
		float b = boost::gil::get_color( p, boost::gil::blue_t());		
		float a = boost::gil::get_color( m, boost::gil::alpha_t());
		
		image::pixel_t q( r * a, g * a, b * a, 0);
		float l = image::luminance( q);
		
		if( l >= th_)
			return q;

		return image::pixel_t( 0, 0, 0, 0);
	}
	
private:
	
	float th_;
};

struct set_intensity_fun
{
	set_intensity_fun( const Imath::Color4f& intensity) : intensity_( intensity)
	{
	}
	
	image::pixel_t operator()( const image::pixel_t& p) const
	{
		float r = boost::gil::get_color( p, boost::gil::red_t());
		float g = boost::gil::get_color( p, boost::gil::green_t());
		float b = boost::gil::get_color( p, boost::gil::blue_t());
		r *= intensity_.r;
		g *= intensity_.g;
		b *= intensity_.b;
		float a = std::min( std::max( r, std::max( g, b)), 1.0f);
		return image::pixel_t( r, g, b, a);
	}
	
private:
	
	Imath::Color4f intensity_;
};

} // unnamed

glow_node_t::glow_node_t() : image_node_t()
{ 
	set_name( "glow");
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_input_plug( "mask", true, ui::palette_t::instance().color( "matte plug"), "Mask");
    add_output_plug();
}

void glow_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Intensity"));
    p->set_id( "intensity");
    p->set_range( 0, 100);
    p->set_default_value( 1);
    add_param( p);

    p.reset( new float_param_t( "Thereshold"));
    p->set_id( "theresh");
    p->set_min( 0);
    p->set_default_value( 0.5f);
	p->set_step( 0.05f);
    add_param( p);
	
    std::auto_ptr<color_param_t> c( new color_param_t( "Color"));
    c->set_id( "color");
    c->set_is_rgba( false);
    c->set_default_value( Imath::Color4f( 1, 1, 1, 0));
    add_param( c);
	
    std::auto_ptr<float2_param_t> p2( new float2_param_t( "Radius"));
    p2->set_id( "radius");
    p2->set_range( 0, 200);
    p2->set_step( 0.25);
    p2->set_default_value( Imath::V2f( 5, 5));
    p2->set_proportional( true);
    add_param( p2);
	
    std::auto_ptr<float3_param_t> p3( new float3_param_t( "Channels"));
    p3->set_id( "channels");
    p3->set_range( 0, 100);
    p3->set_step( 0.25);
    p3->set_default_value( Imath::V3f( 1, 1, 1));
    p3->set_proportional( true);
    add_param( p3);

    p.reset( new float_param_t( "Blur Iters"));
    p->set_id( "iters");
    p->set_range( 1, 10);
    p->set_default_value( 2);
    p->set_static( true);
    p->set_round_to_int( true);
    add_param( p);

    std::auto_ptr<popup_param_t> pop( new popup_param_t( "Layer Mode"));
    pop->set_id( "layer_mode");
    pop->menu_items() = boost::assign::list_of( "Add")( "Screen");
    add_param( pop);
	
    p.reset( new float_param_t( "Background"));
    p->set_id( "bg");
    p->set_range( 0, 1);
    p->set_default_value( 1);
	p->set_step( 0.05f);
    add_param( p);
}

bool glow_node_t::do_is_identity() const
{
	if( get_value<float>( param( "intensity")) == 0)
		return true;
	
	return false;
}

void glow_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i box( input_as<image_node_t>()->bounds());

	int hradius, vradius;
	get_expand_radius( hradius, vradius);
	
	if( input( 1))
	{
		Imath::Box2i mask_bounds( input_as<image_node_t>( 1)->bounds());
		mask_bounds = Imath::intersect( box, mask_bounds);
		
		if( !mask_bounds.isEmpty())
		{
			mask_bounds.min.x -= hradius;
			mask_bounds.min.y -= vradius;
			mask_bounds.max.x += hradius;
			mask_bounds.max.y += vradius;
			box.extendBy( mask_bounds);
		}
	}
	else
	{
		box.min.x -= hradius;
		box.min.y -= vradius;
		box.max.x += hradius;
		box.max.y += vradius;
	}
	
    set_bounds( box);
}

void glow_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi( interest());
	image_node_t *in = input_as<image_node_t>();

	int hradius, vradius;
	get_expand_radius( hradius, vradius);
	
	if( input( 1))
	{
		image_node_t *msk = input_as<image_node_t>( 1);
		msk->add_interest( roi);
		
		Imath::Box2i mask_area( Imath::intersect( msk->bounds(), roi));
		mask_area.min.x -= hradius;
		mask_area.min.y -= vradius;
		mask_area.max.x += hradius;
		mask_area.max.y += vradius;
		roi.extendBy( mask_area);
		in->add_interest( roi);
	}
	else
	{
		roi.min.x -= hradius; roi.min.y -= vradius;
		roi.max.x += hradius; roi.max.y += vradius;
		in->add_interest( roi);
	}
}

void glow_node_t::do_process( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
	
    Imath::Box2i area( Imath::intersect( in->defined(), defined()));

    if( area.isEmpty())
		return;
	
	Imath::V3f channel_fact = get_value<Imath::V3f>( param( "channels"));
	
	Imath::V2f r_radius = get_value<Imath::V2f>( param( "radius")) / context.subsample;
	r_radius.x = r_radius.x / aspect_ratio() * proxy_scale().x;
	r_radius.y = r_radius.y / aspect_ratio() * proxy_scale().y;

	Imath::V2f g_radius = r_radius * channel_fact.y;
	Imath::V2f b_radius = r_radius * channel_fact.z;
	r_radius *= channel_fact.x;
	
    int iters = get_value<float>( param( "iters"));

	float bg = get_value<float>( param( "bg"));

	if( bg != 0)
		image::mul_image_scalar( in->const_subimage_view( area), bg, subimage_view( area));

	if( get_value<float>( param( "intensity")) == 0)
		return;
	
	image::buffer_t src( defined(), 4);
	boost::gil::fill_pixels( src.rgba_view(), image::pixel_t( 0, 0, 0, 0));

	if( input( 1))
	{
		image_node_t *msk = input_as<image_node_t>( 1);

		Imath::Box2i msk_area( Imath::intersect( msk->defined(), defined()));
		
		if( !msk_area.isEmpty())
		{
			boost::gil::tbb_transform2_pixels( in->const_subimage_view( msk_area),
											   msk->const_subimage_view( msk_area),
											   src.rgba_subimage_view( msk_area),
											   thereshold_and_mask_fun( get_value<float>( param( "theresh"))));
		}
	}
	else
	{
		boost::gil::tbb_transform_pixels( in->const_subimage_view( area), src.rgba_subimage_view( area), 
										  thereshold_fun( get_value<float>( param( "theresh"))));
	}
	
	image::buffer_t blurred( defined(), 4);
	boost::gil::fill_pixels( blurred.rgba_view(), image::pixel_t( 0, 0, 0, 0));
	
	image::buffer_t buffer( image_view().height(), image_view().width(), 1);
	image::gray_image_view_t tmp = buffer.gray_view();
	
	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( src.rgba_view(), 0, tmp, r_radius, iters, blurred.rgba_view());

	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( src.rgba_view(), 1, tmp, g_radius, iters, blurred.rgba_view());

	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( src.rgba_view(), 2, tmp, b_radius, iters, blurred.rgba_view());
	
	Imath::Color4f intensity = get_value<Imath::Color4f>( param( "color"));
	intensity *= get_value<float>( param( "intensity"));
	boost::gil::tbb_transform_pixels( blurred.rgba_view(), blurred.rgba_view(), set_intensity_fun( intensity));

	if( get_value<int>( param( "layer_mode")))
		image::composite_screen( const_image_view(), blurred.rgba_view(), image_view(), 1.0f);
	else
		image::composite_add( const_image_view(), blurred.rgba_view(), image_view(), 1.0f);
}

void glow_node_t::blur_channel( const image::const_image_view_t& src, int ch, const boost::gil::gray32f_view_t& tmp, 
								const Imath::V2f& radius, int iters, const image::image_view_t& dst)
{
    image::box_blur_channel( boost::gil::nth_channel_view( src, ch), tmp,
							 boost::gil::nth_channel_view( dst, ch),
							 radius.x / ( float) iters, radius.y / ( float) iters, iters);
}

void glow_node_t::get_expand_radius( int& hradius, int& vradius) const
{
	Imath::V3f channel_fact = get_value<Imath::V3f>( param( "channels"));
	
	Imath::V2f r_radius = get_value<Imath::V2f>( param( "radius"));
	r_radius.x = r_radius.x / aspect_ratio() * proxy_scale().x;
	r_radius.y = r_radius.y / aspect_ratio() * proxy_scale().y;

	Imath::V2f g_radius = r_radius * channel_fact.y;
	Imath::V2f b_radius = r_radius * channel_fact.z;
	r_radius *= channel_fact.x;
	
	hradius = std::ceil( std::max( r_radius.x, std::max( g_radius.x, b_radius.x)));
	vradius = std::ceil( std::max( r_radius.y, std::max( g_radius.y, b_radius.y)));	
}

// factory
node_t *create_glow_node() { return new glow_node_t();}

const node_metaclass_t *glow_node_t::metaclass() const { return &glow_node_metaclass();}

const node_metaclass_t& glow_node_t::glow_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.glow";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Lighting";
        info.menu_item = "Glow";
        info.create = &create_glow_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( glow_node_t::glow_node_metaclass());

} // namespace
} // namespace
