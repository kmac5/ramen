// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/channel/copy_channels_node.hpp>

#include<ramen/assert.hpp>

//#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

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
    copy_source = 0,
    copy_red,
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

copy_channels_node_t::copy_channels_node_t() : image_node_t()
{
    set_name( "copy ch");
    add_input_plug( "src1", false, ui::palette_t::instance().color( "front plug"), "Source");
    add_input_plug( "src2", false, ui::palette_t::instance().color( "front plug"), "Source 2");
    add_output_plug();
}

void copy_channels_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Red"));
    p->set_id( "red");
    p->menu_items() = boost::assign::list_of( "Source")( "Src2 Red")( "Src2 Green")( "Src2 Blue")
											( "Src2 Luminance")( "Src2 Alpha")( "One")( "Zero");
    add_param( p);

    p.reset( new popup_param_t( "Green"));
    p->set_id( "green");
    p->menu_items() = boost::assign::list_of( "Source")( "Src2 Red")( "Src2 Green")( "Src2 Blue")
											( "Src2 Luminance")( "Src2 Alpha")( "One")( "Zero");
    add_param( p);

    p.reset( new popup_param_t( "Blue"));
    p->set_id( "blue");
    p->menu_items() = boost::assign::list_of( "Source")( "Src2 Red")( "Src2 Green")( "Src2 Blue")
											( "Src2 Luminance")( "Src2 Alpha")( "One")( "Zero");
    add_param( p);

    p.reset( new popup_param_t( "Alpha"));
    p->set_id( "alpha");
    p->menu_items() = boost::assign::list_of( "Source")( "Src2 Red")( "Src2 Green")( "Src2 Blue")
											( "Src2 Luminance")( "Src2 Alpha")( "One")( "Zero");
    add_param( p);
}

void copy_channels_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i rod1 = input_as<image_node_t>(0)->bounds();
    Imath::Box2i rod2 = input_as<image_node_t>(1)->bounds();

    int ch_r = get_value<int>( param( "red"));
    int ch_g = get_value<int>( param( "green"));
    int ch_b = get_value<int>( param( "blue"));
    int ch_a = get_value<int>( param( "alpha"));

    // use alpha from input 1
    if( ch_a == copy_source)
    {
		set_bounds( rod1);
		return;
    }

    // alpha comes from input2
    if( ch_a != set_one && ch_a != set_zero)
    {
		set_bounds( rod2);
		return;
    }

    // alpha is zero or one, look at the other channels
    Imath::Box2i rod;

    if( ch_r == copy_source)
		rod = rod1;
    else
    {
		if( ch_r != set_zero && ch_r != set_one)
		    rod = rod2;
    }

    if( ch_g == copy_source)
		rod.extendBy( rod1);
    else
    {
		if( ch_g != set_zero && ch_g != set_one)
		    rod.extendBy( rod2);
    }

    if( ch_b == copy_source)
		rod.extendBy( rod1);
    else
    {
		if( ch_b != set_zero && ch_b != set_one)
		    rod.extendBy( rod2);
    }

    if( rod.isEmpty())
		rod = rod1;

    set_bounds( rod);
}

void copy_channels_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i src1_area( Imath::intersect( input_as<image_node_t>(0)->defined(), defined()));
    image::const_image_view_t src1( input_as<image_node_t>(0)->const_subimage_view( src1_area));

    Imath::Box2i src2_area( Imath::intersect( input_as<image_node_t>(1)->defined(), defined()));
    image::const_image_view_t src2( input_as<image_node_t>(1)->const_subimage_view( src2_area));

    image::image_view_t dst1( subimage_view( src1_area));
    image::image_view_t dst2( subimage_view( src2_area));

    int ch = get_value<int>( param( "red"));
	if( ch == copy_source)
		copy_channel( src1, 0, dst1, 0);
	else
		copy_channel( src2, ch-1, dst2, 0);
	
    ch = get_value<int>( param( "green"));
	if( ch == copy_source)
		copy_channel( src1, 1, dst1, 1);
	else
		copy_channel( src2, ch-1, dst2, 1);

    ch = get_value<int>( param( "blue"));
	if( ch == copy_source)
		copy_channel( src1, 2, dst1, 2);
	else
		copy_channel( src2, ch-1, dst2, 2);

    ch = get_value<int>( param( "alpha"));
    switch( ch)
    {
		case copy_source:
			copy_channel( src1, 3, dst1, 3);
		break;
	
		case set_one:
		case set_zero:
		{
			Imath::Box2i area( Imath::intersect( defined(), format()));
			copy_channel( src1, ch-1, subimage_view( area), 3);
		}
		break;
	
		default:
			copy_channel( src2, ch-1, dst2, 3);
    }
}

void copy_channels_node_t::copy_channel( const image::const_image_view_t& src, int src_ch, const image::image_view_t& dst , int dst_ch)
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
	}	
}

// factory
node_t *create_copy_channels_node() { return new copy_channels_node_t();}

const node_metaclass_t *copy_channels_node_t::metaclass() const { return &copy_channels_node_metaclass();}

const node_metaclass_t& copy_channels_node_t::copy_channels_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.copy_channels";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Channel";
        m.menu_item = "Copy Channels";
        m.help = "Copy channels from the second input to the first";
        m.create = &create_copy_channels_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( copy_channels_node_t::copy_channels_node_metaclass());

} // namespace
} // namespace
