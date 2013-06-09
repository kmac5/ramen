// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/gradient_node.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/manipulators/point2_manipulator.hpp>

#include<ramen/half/halfExpLog.h>

namespace ramen
{
namespace image
{
namespace
{

struct gradient_fun
{
    gradient_fun( const Imath::V2f& p1, const Imath::V2f& p2, const Imath::Color4f& c1, const Imath::Color4f& c2, 
				  float gamma, float aspect = 1.0f)
    {
		assert( aspect > 0.0f);
		
		p1_ = p1;
		p2_ = p2;
		c1_ = c1;
		c2_ = c2;
		gamma_ = gamma;
		aspect_ = aspect;
        length2_ = ( p2_ - p1_).length2();
    }

    image::pixel_t operator()( const Imath::V2i& p) const
    {
		Imath::V2f q( p.x * aspect_, p.y);
        float t = ( ( q.x - p1_.x) * ( p2_.x - p1_.x)) + ( ( q.y - p1_.y) * ( p2_.y - p1_.y));
        t = clamp( t / length2_, 0.0f, 1.0f);
		float tg = pow_h( t, gamma_);

		float r = c1_.r * (1.0f - tg) + c2_.r * tg;
		float g = c1_.g * (1.0f - tg) + c2_.g * tg;
		float b = c1_.b * (1.0f - tg) + c2_.b * tg;
        float a = clamp( c1_.a * (1.0f - t) + c2_.a * t, 0.0f, 1.0f);
        return image::pixel_t( r, g, b, a);
    }

private:

    Imath::V2f p1_, p2_;
    float length2_;
    Imath::Color4f c1_, c2_;
	float gamma_;
	float aspect_;
};

} // namespace

gradient_node_t::gradient_node_t() : generator_node_t() { set_name( "gradient");}

void gradient_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Start"));
    p->set_id( "start");
    p->set_default_value( Imath::V2f( 0.5, 0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    std::auto_ptr<color_param_t> q( new color_param_t( "Start Color"));
    q->set_id( "startcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( q);

    p.reset( new float2_param_t( "End"));
    p->set_id( "end");
    p->set_default_value( Imath::V2f( 0.5, 1));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);
    
    q.reset( new color_param_t( "End Color"));
    q->set_id( "endcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 1, 1, 1, 1));
    add_param( q);
	
	std::auto_ptr<float_param_t> r( new float_param_t( "Gamma"));
	r->set_id( "gamma");
	r->set_default_value( 1);
    r->set_range( 0.1f, 20);
    r->set_step( 0.05);
    add_param( r);	

	std::auto_ptr<bool_param_t> b( new bool_param_t( "Premultiply"));
	b->set_id( "premult");
	b->set_default_value( false);
	add_param( b);
}

void gradient_node_t::do_create_manipulators()
{
    std::auto_ptr<point2_manipulator_t> m( new point2_manipulator_t( dynamic_cast<float2_param_t*>( &param( "start"))));
    add_manipulator( m);

    m.reset( new point2_manipulator_t( dynamic_cast<float2_param_t*>( &param( "end"))));
    add_manipulator( m);
}

void gradient_node_t::do_process( const render::context_t& context)
{
    gradient_fun grad( get_absolute_value<Imath::V2f>( param( "start")) / context.subsample,
						get_absolute_value<Imath::V2f>( param( "end")) / context.subsample,
						get_value<Imath::Color4f>( param( "startcol")),
						get_value<Imath::Color4f>( param( "endcol")),
						get_value<float>( param( "gamma")), aspect_ratio());

	generate_pixels( grad);
	
	if( get_value<bool>( param( "premult")))
		image::premultiply( image_view(), image_view());
}

// factory
node_t *create_gradient_node() { return new gradient_node_t();}

const node_metaclass_t *gradient_node_t::metaclass() const { return &gradient_node_metaclass();}

const node_metaclass_t& gradient_node_t::gradient_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.gradient";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Gradient";
        info.create = &create_gradient_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( gradient_node_t::gradient_node_metaclass());

} // namespace
} // namespace
