// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/curves_node.hpp>

#include<halfLimits.h>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/params/curve_param.hpp>

#include<ramen/anim/curve_to_half_lut.hpp>

namespace ramen
{
namespace image
{

struct apply_curves_fun
{
    apply_curves_fun( const halfFunction<half>& r_lut, const halfFunction<half>& g_lut,
						const halfFunction<half>& b_lut, const halfFunction<half>& v_lut) : r_lut_( r_lut), g_lut_( g_lut),
																							b_lut_( b_lut), v_lut_( v_lut)
    {
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
		using namespace boost::gil;

		float r = get_color( src, red_t());
		float g = get_color( src, green_t());
		float b = get_color( src, blue_t());
		float a = get_color( src, alpha_t());

		r = r_lut_( v_lut_( r));
		g = g_lut_( v_lut_( g));
		b = b_lut_( v_lut_( b));

		image::pixel_t dst;
		get_color( dst, red_t())    = r;
		get_color( dst, green_t())  = g;
		get_color( dst, blue_t())   = b;
		get_color( dst, alpha_t())  = a;
		return dst;
    }

private:

    const halfFunction<half>& r_lut_;
    const halfFunction<half>& g_lut_;
    const halfFunction<half>& b_lut_;
    const halfFunction<half>& v_lut_;
};

curves_node_t::curves_node_t() : pointop_node_t() { set_name("curves");}

void curves_node_t::add_curve_param( const std::string& name, const std::string& id, const Imath::Color3c col)
{
    std::auto_ptr<curve_param_t> p( new curve_param_t( name));
    p->set_id( id);
    p->set_min( 0.0f);
    p->set_max( std::numeric_limits<half>::max());
    p->set_color( col);

    p->curve().set_default_auto_tangents( anim::keyframe_t::tangent_linear);
    p->curve().insert( 0, 0, false);
    p->curve().insert( 1, 1, false);
    p->curve().set_extrapolation( anim::extrapolate_linear);
    p->curve().recalc_tangents_and_coefficients();
    add_param( p);
}

void curves_node_t::do_create_params()
{
    add_curve_param( "RGB", "rgb");
    add_curve_param( "Red", "red"		, Imath::Color3c( 255, 0, 0));
    add_curve_param( "Green", "green"	, Imath::Color3c( 0, 255, 0));
    add_curve_param( "Blue", "blue"		, Imath::Color3c( 0, 0, 255));
}

void curves_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    const curve_param_t *c = dynamic_cast<const curve_param_t*>( &param( "rgb"));
	halfFunction<half> v_lut( anim::eval_float_curve( c->curve()));

    c = dynamic_cast<const curve_param_t*>( &param( "red"));
    halfFunction<half> r_lut(anim::eval_float_curve( c->curve()));

    c = dynamic_cast<const curve_param_t*>( &param( "green"));
    halfFunction<half> g_lut(anim::eval_float_curve( c->curve()));

    c = dynamic_cast<const curve_param_t*>( &param( "blue"));
    halfFunction<half> b_lut(anim::eval_float_curve( c->curve()));

	boost::gil::tbb_transform_pixels( src, dst, apply_curves_fun( r_lut, g_lut, b_lut, v_lut));
}

// factory
node_t *create_curves_node() { return new curves_node_t();}

const node_metaclass_t *curves_node_t::metaclass() const { return &curves_node_metaclass();}

const node_metaclass_t& curves_node_t::curves_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.curves";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "Curves";
        info.create = &create_curves_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( curves_node_t::curves_node_metaclass());

} // namespace
} // namespace
