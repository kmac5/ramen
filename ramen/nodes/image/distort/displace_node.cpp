// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/distort/displace_node.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/params/group_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

namespace ramen
{
namespace image
{
namespace
{
enum
{
    border_black  = 0,
    border_tile,
    border_mirror
};

enum
{
    red_channel = 0,
    green_channel,
    blue_channel,
    alpha_channel
};

struct displace_fun
{
    displace_fun( const image::const_channel_view_t& xmap, const image::const_channel_view_t& ymap,
                    const Imath::Box2i& map_bounds, const Imath::V2f& amplitude) : xmap_( xmap), ymap_( ymap)
    {
        bounds_ = map_bounds;
        amplitude_ = amplitude;
    }

    Imath::V2f operator()( const Imath::V2f& p) const
    {
        float dx = -1.0f;
        float dy = -1.0f;

        if( ( p.x >= bounds_.min.x) && ( p.x <= bounds_.max.x))
        {
            if( ( p.y >= bounds_.min.y) && ( p.y <= bounds_.max.y))
            {
                dx = xmap_( p.x - bounds_.min.x, p.y - bounds_.min.y)[0];
                dy = ymap_( p.x - bounds_.min.x, p.y - bounds_.min.y)[0];

                dx = ( clamp( dx, 0.0f, 1.0f) - 0.5f) * 2.0f; // map 0 .. 1 to -1 .. 1
                dy = ( clamp( dy, 0.0f, 1.0f) - 0.5f) * 2.0f; // map 0 .. 1 to -1 .. 1
            }
        }

        return Imath::V2f( p.x + ( dx * amplitude_.x), p.y + ( dy * amplitude_.y));
    }

private:

    const image::const_channel_view_t& xmap_, ymap_;
    Imath::Box2i bounds_;
    Imath::V2f amplitude_;
};

} // unnamed

displace_node_t::displace_node_t() : distort_node_t()
{
    set_name( "displace");
    add_input_plug( "displacement", false, ui::palette_t::instance().color( "front plug"), "Displacement Map");
}

void displace_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> p( new float2_param_t( "Amplitude"));
    p->set_id( "amplitude");
    p->set_proportional( true);
    p->set_min( 0);
    p->set_default_value( Imath::V2f( 0, 0));
    p->set_step( 0.05f);
    add_param( p);

    std::auto_ptr<popup_param_t> q( new popup_param_t( "X channel"));
    q->set_id( "xchannel");
    q->menu_items() = boost::assign::list_of( "Red")( "Green")( "Blue")( "Alpha");
    q->set_default_value( 0);
    add_param( q);

    q.reset( new popup_param_t( "Y channel"));
    q->set_id( "ychannel");
    q->menu_items() = boost::assign::list_of( "Red")( "Green")( "Blue")( "Alpha");
    q->set_default_value( 1);
    add_param( q);

    q.reset( new popup_param_t( "Borders"));
    q->set_id( "borders");
    q->menu_items() = boost::assign::list_of( "Black")( "Tile")( "Mirror");
    add_param( q);	
}

bool displace_node_t::do_is_identity() const
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));	
	return amplitude == Imath::V2f( 0, 0);
}

void displace_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));
    Imath::Box2i b( input_as<image_node_t>()->bounds());

    if( !b.isEmpty())
    {
        b.min.x -= amplitude.x;
        b.min.y -= amplitude.y;
        b.max.x += amplitude.x;
        b.max.y += amplitude.y;
    }

    set_bounds( b);
}

void displace_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi( interest());
    input_as<image_node_t>( 1)->add_interest( roi);

	image_node_t *in = input_as<image_node_t>( 0);
	
	if( get_value<int>( param( "borders")) == border_black)
	{
		Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));
	
		roi.min.x -= amplitude.x;
		roi.min.y -= amplitude.y;
		roi.max.x += amplitude.x;
		roi.max.y += amplitude.y;
		in->add_interest( roi);
	}
	else
		in->add_interest( in->format());
}

void displace_node_t::do_process( const render::context_t& context)
{
    using namespace boost::gil;

	Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));
	amplitude.x = amplitude.x / context.subsample / aspect_ratio();
	amplitude.y /= context.subsample;

	image_node_t *in = input_as<image_node_t>( 0);
	image_node_t *dmap = input_as<image_node_t>( 1);
	
    displace_fun f( nth_channel_view( dmap->const_image_view(), get_value<int>( param( "xchannel"))),
                    nth_channel_view( dmap->const_image_view(), get_value<int>( param( "ychannel"))),
                    dmap->defined(), amplitude);
	
    switch( get_value<int>( param( "borders")))
    {
		case border_black:
			image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), f, false, false);
		break;
	
		case border_tile:
			image::warp_bilinear_tile( in->defined(), in->const_image_view(), defined(), image_view(), f, false, false);
		break;
	
		case border_mirror:
			image::warp_bilinear_mirror( in->defined(), in->const_image_view(), defined(), image_view(), f, false, false);
		break;
    }
}

// factory
node_t *create_displace_node() { return new displace_node_t();}

const node_metaclass_t *displace_node_t::metaclass() const { return &displace_node_metaclass();}

const node_metaclass_t& displace_node_t::displace_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.displace";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Distort";
		info.menu_item = "Displace";
        info.help = "Displaces the input image, using the second input as the displacement map."
                    "A value of 0.5 in the displacement map means no displacement,"
                    "Black values warp the image to the left / top and white to the right / bottom.";

        info.create = &create_displace_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( displace_node_t::displace_node_metaclass());

} // namespace
} // namespace
