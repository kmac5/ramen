// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/filter/smart_blur_node.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/smart_blur.hpp>

namespace ramen
{
namespace image
{

smart_blur_node_t::smart_blur_node_t() : base_blur_node_t() { set_name( "smart_blur");}

void smart_blur_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Channels"));
    r->set_id( "channels");
    r->menu_items() = boost::assign::list_of( "RGBA")( "RGB");
    add_param( r);
	
    std::auto_ptr<float2_param_t> q( new float2_param_t( "Deviation"));
    q->set_id( "stddev");
    q->set_range( 0, 100);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

	std::auto_ptr<float_param_t> f( new float_param_t( "Thereshold"));
	f->set_id( "theresh");
	f->set_min( 0);
	f->set_default_value( 0.05);
	f->set_step( 0.025);
	add_param( f);
	
    std::auto_ptr<popup_param_t> b( new popup_param_t( "Border Mode"));
    b->set_id( "border");
    b->menu_items() = boost::assign::list_of( "Black")( "Repeat")( "Reflect");
    add_param( b);
}

bool smart_blur_node_t::do_is_identity() const
{
    Imath::V2f stddev = get_value<Imath::V2f>( param( "stddev"));
	return stddev.x == 0 && stddev.y == 0;
}

void smart_blur_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    Imath::V2f stddev = get_value<Imath::V2f>( param( "stddev"));
	stddev = adjust_blur_size( stddev, 1);

	int sizex = (int)( stddev.x * 6 + 1) | 1;

	if( sizex == 1)
		sizex = 3;

	int sizey = (int)( stddev.y * 6 + 1) | 1;

	if( sizey == 1)
		sizey = 3;
	
    hradius = ( sizex - 1) / 2;
    vradius = ( sizey - 1) / 2;
}

void smart_blur_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( ImathExt::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
        return;

    Imath::V2f stddev = get_value<Imath::V2f>( param( "stddev"));
	stddev = adjust_blur_size( stddev, context.subsample);
	
	copy_src_image( 0, area, ( blur_border_mode) get_value<int>( param( "border")));
	
    blur_channels_mode channels = ( blur_channels_mode) get_value<int>( param( "channels"));
	
    switch( channels)
    {
		case channels_rgba:
		{
			image::buffer_t tmp( const_image_view().height(), const_image_view().width(), 4);
			image::smart_blur_rgba( const_image_view(), tmp.rgba_view(), image_view(), stddev.x, stddev.y, get_value<float>( param( "theresh")));
		}
		break;
	
		case channels_rgb:
		{
			image::buffer_t tmp( const_image_view().height(), const_image_view().width(), 4);
			image::smart_blur_rgba( const_image_view(), tmp.rgba_view(), image_view(), stddev.x, stddev.y, get_value<float>( param( "theresh")));			
			boost::gil::fill_pixels( boost::gil::nth_channel_view( image_view(), 3), boost::gil::gray32f_pixel_t( 0));
			boost::gil::copy_pixels( boost::gil::nth_channel_view( input_as<image_node_t>()->const_subimage_view( area), 3),
										boost::gil::nth_channel_view( subimage_view( area), 3));
		}
		break;
	}
}

// factory
node_t *create_smart_blur_node() { return new smart_blur_node_t();}

const node_metaclass_t *smart_blur_node_t::metaclass() const { return &smart_blur_node_metaclass();}

const node_metaclass_t& smart_blur_node_t::smart_blur_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.smart_blur";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Filter";
        info.menu_item = "Smart Blur";
        info.create = &create_smart_blur_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( smart_blur_node_t::smart_blur_node_metaclass());

} // namespace
} // namespace
