// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/filter/blur_node.hpp>

#include<ramen/gil/extension/border_algorithm.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur.hpp>

namespace ramen
{
namespace image
{

blur_node_t::blur_node_t() : base_blur_node_t() { set_name( "blur");}

void blur_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Channels"));
    r->set_id( "channels");
    r->menu_items() = boost::assign::list_of( "RGBA")( "RGB")( "Alpha");
    add_param( r);

    std::auto_ptr<float2_param_t> q( new float2_param_t( "Radius"));
    q->set_id( "radius");
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

bool blur_node_t::do_is_identity() const
{
    Imath::V2f radius = get_value<Imath::V2f>( param( "radius"));
	return radius == Imath::V2f( 0, 0);
}

void blur_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    Imath::V2i radius = round_blur_size( adjust_blur_size( get_value<Imath::V2f>( param( "radius")), 1));
    hradius = radius.x;
    vradius = radius.y;
}

void blur_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( ImathExt::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
        return;

	copy_src_image( 0, area, ( blur_border_mode) get_value<int>( param( "border")));

    blur_channels_mode channels = ( blur_channels_mode) get_value<int>( param( "channels"));
    int iters = get_value<float>( param( "iters"));
    Imath::V2f radius = get_value<Imath::V2f>( param( "radius")) / iters;
	radius = adjust_blur_size( radius, context.subsample);
	
    switch( channels)
    {
		case channels_rgba:
		{
			image::buffer_t tmp( const_image_view().height(), const_image_view().width(), 4);
			image::box_blur_rgba( const_image_view(), tmp.rgba_view(), image_view(), radius.x, radius.y, iters);
		}
		break;
	
		case channels_rgb:
		{
			image::buffer_t tmp( const_image_view().height(), const_image_view().width(), 4);
			image::box_blur_rgba( const_image_view(), tmp.rgba_view(), image_view(), radius.x, radius.y, iters);
			boost::gil::fill_pixels( boost::gil::nth_channel_view( image_view(), 3), boost::gil::gray32f_pixel_t( 0));
			boost::gil::copy_pixels( boost::gil::nth_channel_view( input_as<image_node_t>()->const_subimage_view( area), 3),
										boost::gil::nth_channel_view( subimage_view( area), 3));
		}
		break;
	
		case channels_alpha:
		{
			image::buffer_t tmp( const_image_view().height(), const_image_view().width(), 1);
			image::box_blur_channel( boost::gil::nth_channel_view( const_image_view(), 3),
									 tmp.gray_view(),
									boost::gil::nth_channel_view( image_view(), 3),
									radius.x, radius.y, iters);
		}
		break;
    }
}

// factory
node_t *create_blur_node() { return new blur_node_t();}

const node_metaclass_t *blur_node_t::metaclass() const { return &blur_node_metaclass();}

const node_metaclass_t& blur_node_t::blur_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.blur";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Filter";
        info.menu_item = "Blur";
        info.create = &create_blur_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( blur_node_t::blur_node_metaclass());

} // namespace
} // namespace
