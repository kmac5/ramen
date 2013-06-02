// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/channel/set_channels_node.hpp>

#include<ramen/assert.hpp>

#include<ramen/image/color.hpp>

#include<ramen/params/popup_param.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
    copy_red = 0,
    copy_green,
    copy_blue,
    copy_alpha,
	copy_lum,
    set_one,
    set_zero
};

void copy_luminance( const image::const_image_view_t& src, const image::channel_view_t& dst)
{
	RAMEN_ASSERT( src.dimensions() == dst.dimensions());
	
	for( int y = 0, ye = src.height(); y < ye; ++y)
	{
		image::const_image_view_t::x_iterator s_it( src.row_begin( y));
		image::channel_view_t::x_iterator d_it( dst.row_begin( y));
		
		for( int x = 0, xe = src.width(); x < xe; ++x)
		{
			float lum = image::luminance( *s_it++);
			*d_it++ = lum;
		}
	}
}

} // unnamed

set_channels_node_t::set_channels_node_t() : pointop_node_t() { set_name( "set ch");}

void set_channels_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Red"));
    p->set_id( "red");
    p->set_default_value( (int) copy_red);
    p->menu_items() = boost::assign::list_of( "Red")( "Green")( "Blue")( "Alpha")( "Luminance")( "One")( "Zero");
    add_param( p);

    p.reset( new popup_param_t( "Green"));
    p->set_id( "green");
    p->set_default_value( (int) copy_green);
    p->menu_items() = boost::assign::list_of( "Red")( "Green")( "Blue")( "Alpha")( "Luminance")( "One")( "Zero");
    add_param( p);

    p.reset( new popup_param_t( "Blue"));
    p->set_id( "blue");
    p->set_default_value( (int) copy_blue);
    p->menu_items() = boost::assign::list_of( "Red")( "Green")( "Blue")( "Alpha")( "Luminance")( "One")( "Zero");
    add_param( p);

    p.reset( new popup_param_t( "Alpha"));
    p->set_id( "alpha");
    p->set_default_value( (int) copy_alpha);
    p->menu_items() = boost::assign::list_of( "Red")( "Green")( "Blue")( "Alpha")( "Luminance")( "One")( "Zero");
    add_param( p);
}

void set_channels_node_t::do_calc_bounds( const render::context_t& context)
{
    int ch_a = get_value<int>( param( "alpha"));

    if( ch_a == set_one)
		set_bounds( format());
    else
        set_bounds( input_as<image_node_t>()->bounds());
}

void set_channels_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    Imath::Box2i area( Imath::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
        return;

    copy_channel( input_as<image_node_t>()->const_subimage_view( area), get_value<int>( param( "red"))  , subimage_view( area), 0);
    copy_channel( input_as<image_node_t>()->const_subimage_view( area), get_value<int>( param( "green")), subimage_view( area), 1);
    copy_channel( input_as<image_node_t>()->const_subimage_view( area), get_value<int>( param( "blue")) , subimage_view( area), 2);

    int alpha_op = get_value<int>( param( "alpha"));

    // the alpha channel is special if we fill with 0 or 1.
    if( alpha_op == set_one || alpha_op == set_zero)
		copy_channel( src, get_value<int>( param( "alpha")), image_view(), 3);
    else
		copy_channel( src, get_value<int>( param( "alpha")) , dst, 3);
}

void set_channels_node_t::copy_channel( const image::const_image_view_t& src, int src_ch, const image::image_view_t& dst , int dst_ch)
{
	switch( src_ch)
	{
		case set_zero:
			boost::gil::fill_pixels( boost::gil::nth_channel_view( dst, dst_ch), boost::gil::gray32f_pixel_t( 0.0f));
		break;
		
		case set_one:
			boost::gil::fill_pixels( boost::gil::nth_channel_view( dst, dst_ch), boost::gil::gray32f_pixel_t( 1.0f));
		break;
		
		case copy_lum:
			copy_luminance( src, boost::gil::nth_channel_view( dst, dst_ch));
		break;
		
		default:
			boost::gil::copy_pixels( boost::gil::nth_channel_view( src, src_ch), boost::gil::nth_channel_view( dst, dst_ch));
		break;
	}
}

// factory
node_t *create_set_channels_node() { return new set_channels_node_t();}

const node_metaclass_t *set_channels_node_t::metaclass() const { return &set_channels_node_metaclass();}

const node_metaclass_t& set_channels_node_t::set_channels_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.set_channels";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Channel";
        m.menu_item = "Set Channels";
        m.help = "Reorders the channels of the input image.";
        m.create = &create_set_channels_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( set_channels_node_t::set_channels_node_metaclass());

} // namespace
} // namespace
