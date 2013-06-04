// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/distort/twirl_node.hpp>

#include<OpenEXR/ImathMath.h>

#include<ramen/math/constants.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/manipulators/circle_manipulator.hpp>

// spherize y(x) = sqtr( 1 - x*x);

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

struct twirl_fun
{
    twirl_fun( const Imath::V2f& center, float radius, float angle, float aspect)
	{
		center_ = center;
		radius_ = radius;
		angle_ = angle;
		aspect_ = aspect;
		
		radius2_ = radius_ * radius_;
		center_.x *= aspect_;
	}

    Imath::V2f operator()( const Imath::V2f& p) const
    {
		Imath::V2f q( p);
		q.x *= aspect_;
		
		float d2 = ( q - center_).length2();
		
		if( d2 == 0 || d2 >= radius2_)
			return p;
		
		float d = Imath::Math<float>::sqrt( d2);
		float a = angle_ * ( 1.0f - ( d / radius_));
		q = rotate_point( q, a);
		q.x /= aspect_;
		return q;
    }

private:

	Imath::V2f rotate_point( const Imath::V2f& p, float angle) const
	{
		float cs = Imath::Math<float>::cos( math::constants<float>::deg2rad() * angle);
		float ss = Imath::Math<float>::sin( math::constants<float>::deg2rad() * angle);
		
		Imath::V2f q( p - center_);
		Imath::V2f r;
		r.x = q.x * cs - q.y * ss;
		r.y = q.x * ss + q.y * cs;
		return r + center_;
	}
	
	Imath::V2f center_;
	float radius_;
	float radius2_;
	float angle_;
	float aspect_;
};

} // unnamed

twirl_node_t::twirl_node_t() : distort_node_t() { set_name( "twirl");}

void twirl_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> p( new float2_param_t( "Center"));
    p->set_id( "center");
    p->set_default_value( Imath::V2f( 0.5, 0.5));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

	std::auto_ptr<float_param_t> r( new float_param_t( "Radius"));
	r->set_id( "radius");
	r->set_default_value( 0.25);
    r->set_numeric_type( numeric_param_t::relative_size_x);
    add_param( r);
	
	r.reset( new float_param_t( "Angle"));
	r->set_id( "angle");
	r->set_default_value( 0);
	add_param( r);
	
    std::auto_ptr<popup_param_t> q( new popup_param_t( "Borders"));
    q->set_id( "borders");
    q->menu_items() = boost::assign::list_of( "Black")( "Tile")( "Mirror");
    add_param( q);	
}

void twirl_node_t::do_create_manipulators()
{
	float_param_t *radius = dynamic_cast<float_param_t*>( &param( "radius"));
	float2_param_t *center = dynamic_cast<float2_param_t*>( &param( "center"));
	std::auto_ptr<circle_manipulator_t> m( new circle_manipulator_t( radius, center));
	add_manipulator( m);
}

bool twirl_node_t::do_is_identity() const
{
	return get_value<float>( param( "angle")) == 0.0f;
}

void twirl_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i in_bounds( input_as<image_node_t>()->bounds());

	Imath::V2f c = get_absolute_value<Imath::V2f>( param( "center"));
	float r = get_absolute_value<float>( param( "radius"));
	
	Imath::Box2f box( Imath::V2f( c.x - r, c.y - r), 
					  Imath::V2f( c.x + r, c.y + r));

	box = Imath::intersect( box, Imath::Box2f( Imath::V2f( in_bounds.min), Imath::V2f( in_bounds.max)));
    twirl_fun f( c, r, get_value<float>( param( "angle")), aspect_ratio());
	Imath::Box2i ibox = Imath::roundBox( Imath::warpBox( box, f));
	ibox.extendBy( in_bounds);
    set_bounds( ibox);
}

void twirl_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi( interest());
	image_node_t *in = input_as<image_node_t>();
	
	if( get_value<int>( param( "borders")) == border_black)
	{
		Imath::V2f c = get_absolute_value<Imath::V2f>( param( "center"));
		float r = get_absolute_value<float>( param( "radius"));
		
		Imath::Box2f box( Imath::V2f( c.x - r, c.y - r), 
						  Imath::V2f( c.x + r, c.y + r));
		
		box = Imath::intersect( box, Imath::Box2f( Imath::V2f( roi.min), Imath::V2f( roi.max)));

		if( box.isEmpty())
		{
			in->add_interest( roi);
			return;
		}
		
		twirl_fun f( c, r, -get_value<float>( param( "angle")), aspect_ratio());
		Imath::Box2i ibox = Imath::roundBox( Imath::warpBox( box, f));
		
		// add some margin for filtering
		ibox.min.x -= 2;
		ibox.min.y -= 2;
		ibox.max.x += 2;
		ibox.max.y += 2;
		ibox.extendBy( roi);
		in->add_interest( ibox);
	}
	else
		in->add_interest( in->format());
}

void twirl_node_t::do_process( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
		
    twirl_fun f( get_absolute_value<Imath::V2f>( param( "center")) / context.subsample, 
				 get_absolute_value<float>( param( "radius")) / context.subsample,
				 get_value<float>( param( "angle")),
				 aspect_ratio());

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
node_t *create_twirl_node() { return new twirl_node_t();}

const node_metaclass_t *twirl_node_t::metaclass() const { return &twirl_node_metaclass();}

const node_metaclass_t& twirl_node_t::twirl_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.twirl";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Distort";
		info.menu_item = "Twirl";
        info.create = &create_twirl_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( twirl_node_t::twirl_node_metaclass());

} // namespace
} // namespace
