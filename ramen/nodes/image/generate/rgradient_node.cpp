// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/rgradient_node.hpp>

#include<algorithm>
#include<cmath>

#include<OpenEXR/ImathMatrix.h>

#include<ramen/algorithm/clamp.hpp>
#include<ramen/math/constants.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/math/constants.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/manipulators/point2_manipulator.hpp>

#include<ramen/half/halfExpLog.h>

namespace ramen
{
namespace image
{
namespace
{

struct rgradient_fun
{
    rgradient_fun( const Imath::V2f& center, float r1, float r2, float gamma, const Imath::Color4f& c1, const Imath::Color4f& c2, 
				   const Imath::M33f& m)
    {
		center_ = Imath::V2f( center.x, center.y);
		r1_ = r1;
		r2_ = r2;
		gamma_ = gamma;
		c1_ = c1;
		c2_ = c2;
		m_ = m;
    }

    image::pixel_t operator()( const Imath::V2i& p) const
    {
		Imath::V2f q( p.x, p.y);
		q *= m_;

		Imath::V2f diff( center_ - q);
		float t, tg;
		double dist = diff.length2();
		
		if( dist < ( r1_ * r1_))
		{
			t = 0;
			tg = 0;
		}
		else
		{
			if( dist > ( r2_ * r2_))
			{
				t = 1;
				tg = 1;
			}
			else
			{
				dist = std::sqrt( dist);
				t = ( dist - r1_) / ( r2_ - r1_);
				tg = pow_h( t, gamma_);
			}
		}
		
		float r = c1_.r * (1.0f - tg) + c2_.r * tg;
		float g = c1_.g * (1.0f - tg) + c2_.g * tg;
		float b = c1_.b * (1.0f - tg) + c2_.b * tg;
        float a = clamp( c1_.a * (1.0f - t) + c2_.a * t, 0.0f, 1.0f);
        return image::pixel_t( r, g, b, a);
    }

private:

    Imath::V2f center_;
	float r1_, r2_;
    Imath::Color4f c1_, c2_;
	float aspect_;
	float gamma_;
	Imath::M33f m_;
};

} // namespace

rgradient_node_t::rgradient_node_t() : generator_node_t() { set_name( "rgrad");}

void rgradient_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Center"));
    p->set_id( "center");
    p->set_default_value( Imath::V2f( 0.5, 0.5));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

	std::auto_ptr<float_param_t> r( new float_param_t( "R0"));
	r->set_id( "r0");
	r->set_default_value( 0.05);
    r->set_numeric_type( numeric_param_t::relative_size_x);
    add_param( r);
	
	r.reset( new float_param_t( "R1"));
	r->set_id( "r1");
	r->set_default_value( 0.25);
    r->set_numeric_type( numeric_param_t::relative_size_x);
    add_param( r);

	r.reset( new float_param_t( "Gamma"));
	r->set_id( "gamma");
	r->set_default_value( 1);
    r->set_range( 0.1f, 20);
    r->set_step( 0.05);
    add_param( r);
	
    std::auto_ptr<color_param_t> q( new color_param_t( "Start Color"));
    q->set_id( "startcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 1, 1, 1, 1));
    add_param( q);
    
    q.reset( new color_param_t( "End Color"));
    q->set_id( "endcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( q);

    p.reset( new float2_param_t( "Scale"));
    p->set_id( "scale");
    p->set_default_value( Imath::V2f( 1.0, 1.0));
    p->set_step( 0.05);
	p->set_proportional( true);
    add_param( p);

	r.reset( new float_param_t( "angle"));
	r->set_id( "angle");
	r->set_default_value( 0);
    r->set_step( 0.05);
    add_param( r);
	
	std::auto_ptr<bool_param_t> b( new bool_param_t( "Premultiply"));
	b->set_id( "premult");
	b->set_default_value( false);
	add_param( b);
	
	b.reset( new bool_param_t( "Crop"));
	b->set_id( "crop");
	b->set_default_value( false);
	b->set_include_in_hash( false);
	add_param( b);	
}

void rgradient_node_t::do_create_manipulators()
{
    std::auto_ptr<point2_manipulator_t> m( new point2_manipulator_t( dynamic_cast<float2_param_t*>( &param( "center"))));
    add_manipulator( m);
}

void rgradient_node_t::do_calc_bounds( const render::context_t& context)
{
	if( get_value<bool>( param( "crop")))
	{
		Imath::V2f center = get_absolute_value<Imath::V2f>( param( "center"));
		center.x = center.x * aspect_ratio();
	
		float r = std::max( get_absolute_value<float>( param( "r0")),
							get_absolute_value<float>( param( "r1")));
		
		Imath::Box2f box( Imath::V2f( center.x - r, center.y - r),
						  Imath::V2f( center.x + r, center.y + r));
		
		Imath::V2f scale = get_value<Imath::V2f>( param( "scale"));
		float angle = get_value<float>( param( "angle"));
	
		Imath::M33f m = Imath::M33f().setTranslation( -center) *
						Imath::M33f().setScale( scale) *
						Imath::M33f().setRotation( angle * math::constants<float>::deg2rad()) *
						Imath::M33f().setTranslation( center) *
						Imath::M33f().setScale( Imath::V2f( 1.0f / aspect_ratio(), 1.0));
		
		box = ImathExt::transform( box, m);
        set_bounds( ImathExt::intersect( format(), ImathExt::roundBox( box)));
	}
	else
		generator_node_t::do_calc_bounds( context);
}

void rgradient_node_t::do_calc_defined( const render::context_t& context)
{
	//if( get_value<bool>( param( "crop")))
	    set_defined( ImathExt::intersect( bounds(), interest()));
	//else
		//set_defined( interest());
}

void rgradient_node_t::do_process( const render::context_t& context)
{	
    Imath::V2f center = get_absolute_value<Imath::V2f>( param( "center"));
	center.x = center.x * aspect_ratio() / context.subsample;
	center.y /= context.subsample;

	Imath::V2f scale = get_value<Imath::V2f>( param( "scale"));
	float angle = get_value<float>( param( "angle"));

    Imath::M33f m = Imath::M33f().setTranslation( -center) *
                    Imath::M33f().setScale( scale) *
                    Imath::M33f().setRotation( angle * math::constants<float>::deg2rad()) *
                    Imath::M33f().setTranslation( center) *
					Imath::M33f().setScale( Imath::V2f( 1.0f / aspect_ratio(), 1.0));

	try
	{
		Imath::M33f inv_m( m.inverse( true));
	    rgradient_fun grad( center, get_absolute_value<float>( param( "r0")) / context.subsample,
									get_absolute_value<float>( param( "r1")) / context.subsample,
									get_value<float>( param( "gamma")),
									get_value<Imath::Color4f>( param( "startcol")),
									get_value<Imath::Color4f>( param( "endcol")), inv_m);

		generate_pixels( grad);
	}
	catch( Iex::MathExc& e)
	{
		Imath::Color4f end_col = get_value<Imath::Color4f>( param( "endcol"));
		boost::gil::fill_pixels( image_view(), image::pixel_t( end_col.r, end_col.g, end_col.b, end_col.a));
	}
	
	if( get_value<bool>( param( "premult")))
		image::premultiply( image_view(), image_view());
}

// factory
node_t *create_rgradient_node() { return new rgradient_node_t();}

const node_metaclass_t *rgradient_node_t::metaclass() const { return &rgradient_node_metaclass();}

const node_metaclass_t& rgradient_node_t::rgradient_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.rgradient";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Radial Gradient";
        info.create = &create_rgradient_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( rgradient_node_t::rgradient_node_metaclass());

} // namespace
} // namespace
