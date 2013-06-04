// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/tint_node.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/image/color.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct tint_fun
{
	tint_fun( const Imath::Color4f& black, const Imath::Color4f& white, float tint)
	{
		black_ = black;
		white_ = white;
		tint_ = tint;
	}

	image::pixel_t operator()( const image::pixel_t& src) const
	{
		using namespace boost::gil;
		
		if( tint_ == 0.0f)
			return src;
		
		float l = image::luminance( src);
		
		Imath::Color4f c( Imath::lerp( black_.r, white_.r, l),
							Imath::lerp( black_.g, white_.g, l),
							Imath::lerp( black_.b, white_.b, l),
							Imath::lerp( black_.a, white_.a, get_color( src, alpha_t())));

		if( tint_ != 1.0f)
		{
			Imath::Color4f s( get_color( src, red_t()),
							  get_color( src, green_t()),
							  get_color( src, blue_t()),
							  get_color( src, alpha_t()));
			
			c = Imath::lerp( s, c, tint_);
		}

		return image::pixel_t( c.r, c.g, c.b, c.a);
	}

private:

	Imath::Color4f black_;
	Imath::Color4f white_;
	float tint_;	
};
	
} // unnamed

tint_node_t::tint_node_t() : pointop_node_t() { set_name("tint");}

void tint_node_t::do_create_params()
{
    std::auto_ptr<color_param_t> q( new color_param_t( "Black"));
    q->set_id( "black");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( q);
    
    q.reset( new color_param_t( "White"));
    q->set_id( "white");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 1, 1, 1, 1));
    add_param( q);
	
    std::auto_ptr<float_param_t> p( new float_param_t( "Tint"));
    p->set_id( "tint");
    p->set_default_value( 1);
	p->set_range( 0, 1);
    p->set_step( 0.05f);
    add_param( p);
}

void tint_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    boost::gil::tbb_transform_pixels( src, dst, tint_fun( get_value<Imath::Color4f>( param( "black")),
														  get_value<Imath::Color4f>( param( "white")),
														  get_value<float>( param( "tint"))));
}

// factory
node_t *create_tint_node() { return new tint_node_t();}

const node_metaclass_t *tint_node_t::metaclass() const { return &tint_node_metaclass();}

const node_metaclass_t& tint_node_t::tint_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.tint";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Color";
        m.menu_item = "Tint";
        m.create = &create_tint_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( tint_node_t::tint_node_metaclass());

} // namespace
} // namespace
