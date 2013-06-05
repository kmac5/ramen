// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/filter/blur_mask_node.hpp>

#include<ramen/gil/extension/border_algorithm.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur_mask.hpp>

namespace ramen
{
namespace image
{

blur_mask_node_t::blur_mask_node_t() : base_blur_node_t()
{
	set_name( "blur_msk");
    add_input_plug( "mask", false, ui::palette_t::instance().color( "matte plug"), "Mask");
}

void blur_mask_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Channels"));
    r->set_id( "channels");
    r->menu_items() = boost::assign::list_of( "RGBA")( "RGB")( "Alpha");
    add_param( r);

    std::auto_ptr<float2_param_t> q( new float2_param_t( "Min Radius"));
    q->set_id( "min_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "Max Radius"));
    q->set_id( "max_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);
	
    std::auto_ptr<float_param_t> p( new float_param_t( "Iters"));
    p->set_id( "iters");
    p->set_range( 1, 5);
    p->set_default_value( 1);
    p->set_static( true);
    p->set_round_to_int( true);
    add_param( p);

    std::auto_ptr<popup_param_t> b( new popup_param_t( "Border Mode"));
    b->set_id( "border");
    b->menu_items() = boost::assign::list_of( "Black")( "Repeat")( "Reflect");
    add_param( b);
}

bool blur_mask_node_t::do_is_identity() const
{
	Imath::V2f radius = get_max_blur_radius();
	return radius == Imath::V2f( 0, 0);
}

void blur_mask_node_t::do_calc_bounds( const render::context_t& context)
{
	Imath::V2i min_radius = round_blur_size( adjust_blur_size( get_value<Imath::V2f>( param( "min_radius")), 1));	
    Imath::Box2i bounds( input_as<image_node_t>()->bounds());		

	Imath::V2i max_radius = round_blur_size( adjust_blur_size( get_value<Imath::V2f>( param( "max_radius")), 1));
	Imath::Box2i max_bounds( input_as<image_node_t>( 1)->bounds());
	max_bounds = ImathExt::intersect( max_bounds, bounds);

	bounds.min.x -= min_radius.x;
	bounds.max.x += min_radius.x;
	bounds.min.y -= min_radius.y;
	bounds.max.y += min_radius.y;

	if( !max_bounds.isEmpty())
	{
		max_bounds.min.x -= max_radius.x;
		max_bounds.max.x += max_radius.x;
		max_bounds.min.y -= max_radius.y;
		max_bounds.max.y += max_radius.y;
		bounds.extendBy( max_bounds);
	}

    set_bounds( bounds);
}

void blur_mask_node_t::do_calc_inputs_interest( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>( 0);
	image_node_t *msk = input_as<image_node_t>( 1);
	
    Imath::Box2i roi( interest());
	
	Imath::V2i min_radius = round_blur_size( adjust_blur_size( get_value<Imath::V2f>( param( "min_radius")), 1));

	Imath::Box2i min_roi( roi);
	min_roi.min.x -= min_radius.x;
	min_roi.max.x += min_radius.x;
	min_roi.min.y -= min_radius.y;
	min_roi.max.y += min_radius.y;
	
	Imath::Box2i max_roi( ImathExt::intersect( roi, msk->bounds()));
	
	if( !max_roi.isEmpty())
	{
		Imath::V2i max_radius = round_blur_size( adjust_blur_size( get_value<Imath::V2f>( param( "max_radius")), 1));	
		
		max_roi.min.x -= max_radius.x;
		max_roi.max.x += max_radius.x;
		max_roi.min.y -= max_radius.y;
		max_roi.max.y += max_radius.y;
		min_roi.extendBy( max_roi);
	}

	in->add_interest( min_roi);
	msk->add_interest( roi);	
}

void blur_mask_node_t::do_process( const render::context_t& context)
{
	image_node_t *in  = input_as<image_node_t>( 0);
	image_node_t *msk = input_as<image_node_t>( 1);
	
    Imath::Box2i area( ImathExt::intersect( in->defined(), defined()));

    if( area.isEmpty())
        return;

	copy_src_image( 0, area, ( blur_border_mode) get_value<int>( param( "border")));

    blur_channels_mode channels = ( blur_channels_mode) get_value<int>( param( "channels"));
    int iters = get_value<float>( param( "iters"));

    Imath::V2f min_radius = adjust_blur_size( get_value<Imath::V2f>( param( "min_radius")) / iters, context.subsample);
    Imath::V2f max_radius = adjust_blur_size( get_value<Imath::V2f>( param( "max_radius")) / iters, context.subsample);
	Imath::V2i mask_pos = msk->defined().min - defined().min;

	image::buffer_t tmp( const_image_view().height(), const_image_view().width(), 1);

	if( channels == channels_rgb || channels == channels_rgba)
	{
		for( int i = 0; i < 3; ++i)
		{
			image::box_blur_mask( boost::gil::nth_channel_view( const_image_view(), i),
								  boost::gil::nth_channel_view( msk->const_image_view(), 3), mask_pos, 
								  min_radius, max_radius, iters, tmp.gray_view(), 
								  boost::gil::nth_channel_view( image_view(), i));
		}
	}
	
	if( channels == channels_rgba || channels == channels_alpha)
	{
		image::box_blur_mask( boost::gil::nth_channel_view( const_image_view(), 3), 
							  boost::gil::nth_channel_view( msk->const_image_view(), 3), mask_pos, 
							  min_radius, max_radius, iters, tmp.gray_view(), 
							  boost::gil::nth_channel_view( image_view(), 3));
	}
}

Imath::V2f blur_mask_node_t::get_max_blur_radius() const
{
    Imath::V2f min_radius = get_value<Imath::V2f>( param( "min_radius"));
    Imath::V2f max_radius = get_value<Imath::V2f>( param( "max_radius"));	
	return Imath::V2f( std::max( min_radius.x, max_radius.x),
					   std::max( min_radius.y, max_radius.y));
}

// factory
node_t *create_blur_mask_node() { return new blur_mask_node_t();}

const node_metaclass_t *blur_mask_node_t::metaclass() const { return &blur_mask_node_metaclass();}

const node_metaclass_t& blur_mask_node_t::blur_mask_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.blur_mask";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Filter";
        info.menu_item = "Blur Mask";
        info.create = &create_blur_mask_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( blur_mask_node_t::blur_mask_node_metaclass());

} // namespace
} // namespace
