// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/key/color_diff_keyer_node.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>
#include<ramen/gil/extension/algorithm/tbb/tbb_transform2.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct color_diff_key_fun
{
    color_diff_key_fun( const Imath::Color4f& key, float screen, float complement, float shadows, float alpha_low, float alpha_high, int output)
    {
		key_ = key;
		screen_ = screen;
		complement_ = complement;
		shadows_ = shadows;
		alpha_low_ = alpha_low;
		alpha_high_ = alpha_high;
		output_ = output;
    }

    image::pixel_t operator()( const image::pixel_t& p) const
    {
		static image::pixel_t one( 1, 1, 1, 1);
		return (*this)( p, one);
    }

    image::pixel_t operator()( const image::pixel_t& p, const image::pixel_t& msk) const
    {
		float rix, gix, bix, rsc, gsc, bsc;

		if( key_.g > key_.b)
		{
		    rix = boost::gil::get_color( p, boost::gil::red_t());
		    gix = boost::gil::get_color( p, boost::gil::green_t());
		    bix = boost::gil::get_color( p, boost::gil::blue_t());
		    rsc = key_.r;
		    gsc = key_.g;
		    bsc = key_.b;
		}
		else
		{
		    rix = boost::gil::get_color( p, boost::gil::red_t());
		    gix = boost::gil::get_color( p, boost::gil::blue_t());
		    bix = boost::gil::get_color( p, boost::gil::green_t());
		    rsc = key_.r;
		    gsc = key_.b;
		    bsc = key_.g;
		}

		float ax = 1.0f - ( screen_ * adobe::clamp( gix, 0.0f, 1.0f));
		ax += ( complement_ * std::max( adobe::clamp( rix - rsc, 0.0f, 1.0f), adobe::clamp( bix - bsc, 0.0f, 1.0f)));
	
		if( gsc != 0 && gix < gsc)
		    ax += shadows_ * ( 1.0f - gix / gsc);

		if( ax <= alpha_low_)
		    ax = 0;
		else
		{
		    if( ax >= alpha_high_ || alpha_high_ == alpha_low_)
				ax = 1;
		    else
				ax = ( ax - alpha_low_) / ( alpha_high_ - alpha_low_);
		}

		ax *= boost::gil::get_color( msk, boost::gil::alpha_t());

		image::pixel_t result;

		switch( output_)
		{
			case 0:
				boost::gil::get_color( result, boost::gil::red_t())   = boost::gil::get_color( p, boost::gil::red_t()) * ax;
				boost::gil::get_color( result, boost::gil::green_t()) = boost::gil::get_color( p, boost::gil::green_t()) * ax;
				boost::gil::get_color( result, boost::gil::blue_t())  = boost::gil::get_color( p, boost::gil::blue_t()) * ax;
				boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
			break;
		
			case 1:
				boost::gil::get_color( result, boost::gil::red_t())   = boost::gil::get_color( p, boost::gil::red_t());
				boost::gil::get_color( result, boost::gil::green_t()) = boost::gil::get_color( p, boost::gil::green_t());
				boost::gil::get_color( result, boost::gil::blue_t())  = boost::gil::get_color( p, boost::gil::blue_t());
				boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
			break;
		
			case 2: // alpha
				boost::gil::get_color( result, boost::gil::red_t())   = ax;
				boost::gil::get_color( result, boost::gil::green_t()) = ax;
				boost::gil::get_color( result, boost::gil::blue_t())  = ax;
				boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
			break;
		
			case 3: // status
				if( ax > 0.0f && ax < 1.0f)
					ax = 0.5f;
		
				boost::gil::get_color( result, boost::gil::red_t())   = ax;
				boost::gil::get_color( result, boost::gil::green_t()) = ax;
				boost::gil::get_color( result, boost::gil::blue_t())  = ax;
				boost::gil::get_color( result, boost::gil::alpha_t()) = ax;
			break;
			}

		return result;
    }

private:

    Imath::Color4f key_;
    float screen_;
    float complement_;
    float shadows_;
    float alpha_low_;
    float alpha_high_;
    int output_;
};

} // unnamed

color_diff_keyer_node_t::color_diff_keyer_node_t() : keyer_node_t()
{
    set_name("cdiff_key");
}

void color_diff_keyer_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Output"));
    r->set_id( "output");
    r->menu_items() = boost::assign::list_of( "Premultiplied")( "Unpremultiplied")( "Alpha")( "Status");
    add_param( r);

    std::auto_ptr<color_param_t> p( new color_param_t( "Color"));
    p->set_id( "color");
    p->set_is_rgba( false);
    p->set_default_value( Imath::Color4f( 0, 0, 1, 0));
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Screen"));
    q->set_id( "screen_mult");
    q->set_default_value( 1);
    q->set_range( 0, 50);
    q->set_step( 0.05);
    add_param( q);

    q.reset( new float_param_t( "Complement"));
    q->set_id( "comp_mult");
    q->set_default_value( 1);
    q->set_range( 0, 30);
    q->set_step( 0.05);
    add_param( q);
    
    q.reset( new float_param_t( "Shadows"));
    q->set_id( "shd_mult");
    q->set_default_value( 0.0);
    q->set_range( 0, 20);
    q->set_step( 0.05);
    add_param( q);

    q.reset( new float_param_t( "Alpha Low"));
    q->set_id( "alpha_low");
    q->set_default_value( 0);
    q->set_range( 0, 1);
    q->set_step( 0.05);
    add_param( q);

    q.reset( new float_param_t( "Alpha High"));
    q->set_id( "alpha_high");
    q->set_default_value( 1);
    q->set_range( 0, 1);
    q->set_step( 0.05);
    add_param( q);
}

void color_diff_keyer_node_t::do_process( const render::context_t& context)
{
	image_node_t *in0 = input_as<image_node_t>( 0);
	
    if( !input(1))
    {
		Imath::Box2i area( ImathExt::intersect( defined(), in0->defined()));

		if( area.isEmpty())
		    return;

		boost::gil::tbb_transform_pixels(  in0->const_subimage_view( area),
											subimage_view( area), 
											color_diff_key_fun( get_value<Imath::Color4f>( param( "color")),
																get_value<float>( param( "screen_mult")),
																get_value<float>( param( "comp_mult")),
																get_value<float>( param( "shd_mult")),
																get_value<float>( param( "alpha_low")),
																get_value<float>( param( "alpha_high")),
																get_value<int>( param( "output"))));
    }
    else
    {
		image_node_t *in1 = input_as<image_node_t>( 1);
		
		Imath::Box2i area( ImathExt::intersect( ImathExt::intersect( defined(), in0->defined()), in1->defined()));

		if( area.isEmpty())
		    return;

		boost::gil::tbb_transform2_pixels(  in0->const_subimage_view( area),
											in1->const_subimage_view( area),
											subimage_view( area),
											color_diff_key_fun( get_value<Imath::Color4f>( param( "color")),
																	get_value<float>( param( "screen_mult")),
																	get_value<float>( param( "comp_mult")),
																	get_value<float>( param( "shd_mult")),
																	get_value<float>( param( "alpha_low")),
																	get_value<float>( param( "alpha_high")),							
																	get_value<int>( param( "output"))));
    }
}

// factory

node_t *create_color_diff_keyer_node() { return new color_diff_keyer_node_t();}

const node_metaclass_t *color_diff_keyer_node_t::metaclass() const { return &color_diff_keyer_node_metaclass();}

const node_metaclass_t& color_diff_keyer_node_t::color_diff_keyer_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.cdiff_keyer";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Key";
        info.menu_item = "Color Diff Keyer";
        info.create = &create_color_diff_keyer_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( color_diff_keyer_node_t::color_diff_keyer_node_metaclass());

} // namespace
} // namespace
