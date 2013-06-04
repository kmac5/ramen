// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/transform/camera_shake_node.hpp>

#include<boost/lexical_cast.hpp>

#include<OpenEXR/ImathRandom.h>

#include<ramen/assert.hpp>

#include<ramen/math/constants.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/separator_param.hpp>

#include<ramen/noise/global_noise.hpp>
#include<ramen/noise/vector_noise_adaptor.hpp>

namespace ramen
{
namespace image
{
	
camera_shake_node_t::camera_shake_node_t() : xform2d_node_t() { set_name("cam_shake");}

camera_shake_node_t::camera_shake_node_t( const camera_shake_node_t& other) : xform2d_node_t( other)  {}

void camera_shake_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> p2( new float2_param_t( "Center"));
    p2->set_id( "center");
    p2->set_default_value( Imath::V2f( 0.5, 0.5));
    p2->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p2);

    std::auto_ptr<float_param_t> p( new float_param_t( "Frequency"));
    p->set_id( "freq");
	p->set_range( 0.001, 1000);
    p->set_default_value( 1.0f);
	p->set_step( 0.05);
    add_param( p);
	
    p2.reset( new float2_param_t( "Translate"));
    p2->set_id( "t_amp");
	p2->set_min( 0);
    p2->set_default_value( Imath::V2f( 0, 0));
	p2->set_proportional( true);
    add_param( p2);

    p.reset( new float_param_t( "Scale"));
    p->set_id( "s_amp");
	p->set_min( 0);
    p->set_default_value( 0.0f);
	p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Extra Scale"));
    p->set_id( "extra_s");
    p->set_default_value( 0.0f);
	p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Rotate"));
    p->set_id( "r_amp");
	p->set_min( 0);
    p->set_default_value( 0.0f);
	p->set_step( 0.1);
    add_param( p);
	
	p.reset( new float_param_t( "Seed"));
	p->set_id( "seed");
	p->set_round_to_int( true);
	p->set_default_value( 17467);
	add_param( p);

	create_default_filter_param();

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

	create_motion_blur_param();
}

void camera_shake_node_t::do_calc_hash_str( const render::context_t& context)
{
	xform2d_node_t::do_calc_hash_str( context);
    hash_generator() << "(" << boost::lexical_cast<std::string>( context.frame) << ")";
}

camera_shake_node_t::matrix3_type camera_shake_node_t::do_calc_transform_matrix_at_frame( float frame, int subsample) const
{
    Imath::V2d c = get_absolute_value_at_frame<Imath::V2f>( param( "center"), frame) / subsample;
	
	float seed = get_value<float>( param( "seed"));
	Imath::Rand48 rng( seed);

	float freq = 1.0f / get_value<float>( param( "freq"));
	Imath::V2f t_amp = get_value<Imath::V2f>( param( "t_amp"));

	noise::vector_noise_adaptor2_t<noise::simplex_noise_t, noise::simplex_noise_t> noise( noise::global_noise, noise::global_noise1);
	
    Imath::V2d t = noise( Imath::V2f( frame / freq, rng.nextf() + 0.11751));
	t.x = t.x * t_amp.x / subsample;
	t.y = t.y * t_amp.y / subsample;

	float r_amp = get_value<float>( param( "r_amp"));
	float s_amp = get_value<float>( param( "s_amp"));
	float extra_s = get_value<float>( param( "extra_s"));

    Imath::V2d rs = noise( Imath::V2f( ( frame / freq) + 0.11243, rng.nextf() + 1.7567));
	rs.x = rs.x * r_amp;
	rs.y = std::max( ( rs.y * s_amp) + 1.0f + extra_s, 0.0);

    matrix3_type m =	matrix3_type().setTranslation( -c) *
						matrix3_type().setScale( Imath::V2d( aspect_ratio(), 1.0)) *
						matrix3_type().setScale( Imath::V2d( rs.y, rs.y)) *
						matrix3_type().setRotation( rs.x * math::constants<double>::deg2rad()) *
						matrix3_type().setTranslation( t) *
						matrix3_type().setScale( Imath::V2d( 1.0 / aspect_ratio(), 1.0)) *
						matrix3_type().setTranslation( c);
    return m;
}

node_t *create_camera_shake_node() { return new camera_shake_node_t();}

const node_metaclass_t *camera_shake_node_t::metaclass() const { return &camera_shake_node_metaclass();}

const node_metaclass_t& camera_shake_node_t::camera_shake_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.camshake";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Camera Shake";
        info.create = &create_camera_shake_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( camera_shake_node_t::camera_shake_node_metaclass());

} // namespace
} // namespace
