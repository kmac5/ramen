// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/clamp_colors_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct clamp_rgba_colors_fun
{
	clamp_rgba_colors_fun( float lo, float hi) : lo_(lo), hi_(hi) {}

	image::pixel_t operator()( const image::pixel_t& src) const
	{
		float r = boost::gil::get_color( src, boost::gil::red_t());
		float g = boost::gil::get_color( src, boost::gil::green_t());
		float b = boost::gil::get_color( src, boost::gil::blue_t());
		float a = boost::gil::get_color( src, boost::gil::alpha_t());
	
		if( r < lo_) r = 0;
		if( g < lo_) g = 0;
		if( b < lo_) b = 0;
		if( a < lo_) a = 0;

		if( r > hi_) r = hi_;
		if( g > hi_) g = hi_;
		if( b > hi_) b = hi_;
		if( a > hi_) a = hi_;

		return image::pixel_t( r, g, b, a);
	}
	
	float lo_, hi_;
};
	
struct clamp_rgb_colors_fun
{
    clamp_rgb_colors_fun( float lo, float hi) : lo_(lo), hi_(hi) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
		float r = boost::gil::get_color( src, boost::gil::red_t());
		float g = boost::gil::get_color( src, boost::gil::green_t());
		float b = boost::gil::get_color( src, boost::gil::blue_t());

		if( r < lo_) r = 0;
		if( g < lo_) g = 0;
		if( b < lo_) b = 0;

		if( r > hi_) r = hi_;
		if( g > hi_) g = hi_;
		if( b > hi_) b = hi_;

		return image::pixel_t( r, g, b, boost::gil::get_color( src, boost::gil::alpha_t()));
    }

    float lo_, hi_;
};

struct clamp_alpha_colors_fun
{
	clamp_alpha_colors_fun( float lo, float hi) : lo_(lo), hi_(hi) {}

	image::pixel_t operator()( const image::pixel_t& src) const
	{
		float r = boost::gil::get_color( src, boost::gil::red_t());
		float g = boost::gil::get_color( src, boost::gil::green_t());
		float b = boost::gil::get_color( src, boost::gil::blue_t());
		float a = boost::gil::get_color( src, boost::gil::alpha_t());
	
		if( a < lo_) a = 0;
		if( a > hi_) a = hi_;

		return image::pixel_t( r, g, b, a);
	}
	
	float lo_, hi_;
};

} // unnamed

clamp_colors_node_t::clamp_colors_node_t() : pointop_node_t() { set_name("clamp");}

void clamp_colors_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Channels"));
    r->set_id( "channels");
    r->menu_items() = boost::assign::list_of( "RGBA")( "RGB")( "Alpha");
	r->set_default_value( 1);
    add_param( r);
	
    std::auto_ptr<float_param_t> p( new float_param_t( "Min"));
    p->set_id( "min");
    p->set_default_value( 0.0f);
    p->set_step( 0.01f);
    add_param( p);

    p.reset( new float_param_t( "Max"));
    p->set_id( "max");
    p->set_default_value( 1.0f);
    p->set_step( 0.01f);
    add_param( p);
}

void clamp_colors_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
	switch( get_value<int>( param( "channels")))
	{
		case 0:
			boost::gil::tbb_transform_pixels( src, dst, clamp_rgba_colors_fun( get_value<float>( param( "min")), 
																			  get_value<float>( param( "max"))));
		break;
		
		case 1:
			boost::gil::tbb_transform_pixels( src, dst, clamp_rgb_colors_fun( get_value<float>( param( "min")), 
																			  get_value<float>( param( "max"))));
		break;

		case 2:
			boost::gil::tbb_transform_pixels( src, dst, clamp_alpha_colors_fun( get_value<float>( param( "min")), 
																			  get_value<float>( param( "max"))));
		break;
	}
}

// factory
node_t *create_clamp_colors_node() { return new clamp_colors_node_t();}

const node_metaclass_t *clamp_colors_node_t::metaclass() const { return &clamp_colors_node_metaclass();}

const node_metaclass_t& clamp_colors_node_t::clamp_colors_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.clamp_colors";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "Clamp";
		info.help = "Clamps pixel components to specified range";
        info.create = &create_clamp_colors_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( clamp_colors_node_t::clamp_colors_node_metaclass());

} // namespace
} // namespace
