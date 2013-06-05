// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/filter/blur_channels_node.hpp>

#include<gil/extension/border_algorithm.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur.hpp>

namespace ramen
{
namespace image
{

blur_channels_node_t::blur_channels_node_t() : base_blur_node_t() { set_name( "blur_ch");}

void blur_channels_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> q( new float2_param_t( "R Radius"));
    q->set_id( "r_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "G Radius"));
    q->set_id( "g_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "B Radius"));
    q->set_id( "b_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "A Radius"));
    q->set_id( "a_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    std::auto_ptr<float_param_t> p( new float_param_t( "Iters"));
    p->set_id( "iters");
    p->set_range( 1, 10);
    p->set_default_value( 1);
    p->set_static( true);
    p->set_round_to_int( true);
    add_param( p);

    std::auto_ptr<popup_param_t> b( new popup_param_t( "Border Mode"));
    b->set_id( "border");
    b->menu_items() = boost::assign::list_of( "Black")( "Repeat")( "Reflect");
    add_param( b);
}

bool blur_channels_node_t::do_is_identity() const
{
    Imath::V2f r_radius = get_value<Imath::V2f>( param( "r_radius"));
    Imath::V2f g_radius = get_value<Imath::V2f>( param( "g_radius"));
    Imath::V2f b_radius = get_value<Imath::V2f>( param( "b_radius"));
    Imath::V2f a_radius = get_value<Imath::V2f>( param( "a_radius"));
	return r_radius == Imath::V2f( 0, 0) && g_radius == Imath::V2f( 0, 0) && 
			b_radius == Imath::V2f( 0, 0) && a_radius == Imath::V2f( 0, 0);
}

void blur_channels_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    Imath::V2f r_radius = get_value<Imath::V2f>( param( "r_radius"));
    Imath::V2f g_radius = get_value<Imath::V2f>( param( "g_radius"));
    Imath::V2f b_radius = get_value<Imath::V2f>( param( "b_radius"));
    Imath::V2f a_radius = get_value<Imath::V2f>( param( "a_radius"));

	r_radius = adjust_blur_size( r_radius, 1);
	g_radius = adjust_blur_size( g_radius, 1);
	b_radius = adjust_blur_size( b_radius, 1);
	a_radius = adjust_blur_size( a_radius, 1);
    Imath::V2i radius( round_blur_size( Imath::V2f( std::max( r_radius.x, std::max( g_radius.x, std::max( b_radius.x, a_radius.x))),
							                        std::max( r_radius.y, std::max( g_radius.y, std::max( b_radius.y, a_radius.y))))));

    hradius = radius.x;
    vradius = radius.y;
}

void blur_channels_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( ImathExt::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;

    int iters = get_value<float>( param( "iters"));

    Imath::V2f r_radius = get_value<Imath::V2f>( param( "r_radius")) / iters;
    Imath::V2f g_radius = get_value<Imath::V2f>( param( "g_radius")) / iters;
    Imath::V2f b_radius = get_value<Imath::V2f>( param( "b_radius")) / iters;
    Imath::V2f a_radius = get_value<Imath::V2f>( param( "a_radius")) / iters;

	r_radius = adjust_blur_size( r_radius, context.subsample);
	g_radius = adjust_blur_size( g_radius, context.subsample);
	b_radius = adjust_blur_size( b_radius, context.subsample);
	a_radius = adjust_blur_size( a_radius, context.subsample);

    boost::gil::copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));	
	
	image::buffer_t buffer( image_view().height(), image_view().width(), 1);
	image::gray_image_view_t tmp = buffer.gray_view();

    blur_border_mode bmode = ( blur_border_mode) get_value<int>( param( "border"));
	
	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( 0, area, tmp, r_radius, iters, bmode);

	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( 1, area, tmp, g_radius, iters, bmode);

	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( 2, area, tmp, b_radius, iters, bmode);

	boost::gil::fill_pixels( tmp, 0.0f);
    blur_channel( 3, area, tmp, a_radius, iters, bmode);
}

void blur_channels_node_t::blur_channel( int ch, const Imath::Box2i& area, const boost::gil::gray32f_view_t& tmp,
										 const Imath::V2f& radius, int iters, blur_border_mode border)
{
    if( border != border_black)
    {
        int hradius = std::ceil( radius.x);
        int vradius = std::ceil( radius.y);

        int border_x0 = std::min( area.min.x - defined().min.x, hradius);
        int border_y0 = std::min( area.min.y - defined().min.y, vradius);
        int border_x1 = std::min( defined().max.x - area.max.x, hradius);
        int border_y1 = std::min( defined().max.y - area.max.y, vradius);

        Imath::Box2i box( area);
        box.min.x -= border_x0; box.min.y -= border_y0;
        box.max.x += border_x1; box.max.y += border_y1;

        if( border == border_repeat)
          boost::gil::repeat_border_pixels( boost::gil::nth_channel_view( subimage_view( box), ch), border_x0, border_y0, border_x1, border_y1);
        else
          boost::gil::reflect_border_pixels( boost::gil::nth_channel_view( subimage_view( box), ch), border_x0, border_y0, border_x1, border_y1);
    }

    image::box_blur_channel( boost::gil::nth_channel_view( const_image_view(), ch), tmp,
							 boost::gil::nth_channel_view( image_view(), ch),
							 radius.x, radius.y, iters);
}

// factory
node_t *create_blur_channels_node() { return new blur_channels_node_t();}

const node_metaclass_t *blur_channels_node_t::metaclass() const { return &blur_channels_node_metaclass();}

const node_metaclass_t& blur_channels_node_t::blur_channels_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.blur_channels";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Filter";
        info.menu_item = "Blur Channels";
        info.create = &create_blur_channels_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( blur_channels_node_t::blur_channels_node_metaclass());

} // namespace
} // namespace
