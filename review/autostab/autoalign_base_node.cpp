// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/track/autoalign_base_node.hpp>

#include<OpenEXR/ImathMatrixAlgo.h>

#include<ramen/assert.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/ImathMatrixAlgo.h>

namespace ramen
{
namespace image
{

autoalign_base_node_t::autoalign_base_node_t() : xform_node_t()
{
	for( int i = 0; i < 4; ++i)
	{
		similarity_[i] = 0;
		corner_[i] = 0;
		perspective_[2*i] = 0;
		perspective_[2*i+1] = 0;
	}
}

autoalign_base_node_t::autoalign_base_node_t( const autoalign_base_node_t& other) : xform_node_t( other)
{
	for( int i = 0; i < 4; ++i)
	{
		similarity_[i] = 0;
		corner_[i] = 0;
		perspective_[2*i] = 0;
		perspective_[2*i+1] = 0;
	}
}

// similarity
void autoalign_base_node_t::create_similarity_params()
{
	std::auto_ptr<group_param_t> g( new group_param_t( "Similarity"));
	similarity_[0] = create_float_param( g.get(), "s0", 1.0f);
	similarity_[1] = create_float_param( g.get(), "s1");
	similarity_[2] = create_float_param( g.get(), "s2");
	similarity_[3] = create_float_param( g.get(), "s3");
	add_param( g);
}

void autoalign_base_node_t::get_similarity_params()
{
	similarity_[0] = dynamic_cast<float_param_t*>( &param( "s0"));
	similarity_[1] = dynamic_cast<float_param_t*>( &param( "s1"));
	similarity_[2] = dynamic_cast<float_param_t*>( &param( "s2"));
	similarity_[3] = dynamic_cast<float_param_t*>( &param( "s3"));
}

void autoalign_base_node_t::set_similarity_identity( float frame)
{
	for( int i = 0; i < 4; ++i)
	{
		RAMEN_ASSERT( similarity_[i]);
	}
	
	similarity_[0]->set_value_at_frame( 1, frame);
	similarity_[1]->set_value_at_frame( 0, frame);
	similarity_[2]->set_value_at_frame( 0, frame);
	similarity_[3]->set_value_at_frame( 0, frame);
}

void autoalign_base_node_t::set_similarity_params( float frame, const Imath::M33d& m)
{
	for( int i = 0; i < 4; ++i)
	{
		RAMEN_ASSERT( similarity_[i]);
	}

	similarity_[0]->set_value_at_frame( m[0][0], frame);
	similarity_[1]->set_value_at_frame( m[1][0], frame);
	similarity_[2]->set_value_at_frame( m[2][0], frame);
	similarity_[3]->set_value_at_frame( m[2][1], frame);	
}

Imath::M33d autoalign_base_node_t::similarity_matrix_at_frame( float frame, int subsample) const
{
	for( int i = 0; i < 4; ++i)
	{
		RAMEN_ASSERT( similarity_[i]);
	}

	Imath::M33d m;
	m[0][0] = get_value_at_frame<float>( *similarity_[0], frame);
	m[1][0] = get_value_at_frame<float>( *similarity_[1], frame);
	m[0][1] = -m[1][0];
	m[1][1] =  m[0][0];
	m[2][0] = get_value_at_frame<float>( *similarity_[2], frame) / subsample;
	m[2][1] = get_value_at_frame<float>( *similarity_[3], frame) / subsample;
	return m;
}

// util
Imath::M33d autoalign_base_node_t::remove_SR( const Imath::M33d& m) const
{
	Imath::M33d x;
	x[2][0] = m[2][0];
	x[2][1] = m[2][1];
	return x;
}

Imath::M33d autoalign_base_node_t::remove_S( const Imath::M33d& m) const
{
	Imath::M33d x( m);
	Imath::removeScalingAndShear( x, true);
	return x;
}

Imath::M33d autoalign_base_node_t::remove_R( const Imath::M33d& m) const
{
	Imath::Vec2<double> scl;
	double shr;
	Imath::extractScalingAndShear( m, scl, shr, true);
	
	Imath::M33d x;
	x[0][0] = scl.x;
	x[1][1] = scl.y;
	x[2][0] = m[2][0];
	x[2][1] = m[2][1];
	return x;
}

// perspective
void autoalign_base_node_t::create_perspective_params()
{
	std::auto_ptr<group_param_t> g( new group_param_t( "Homography"));
	perspective_[0] = create_float_param( g.get(), "p0", 1.0f);
	perspective_[1] = create_float_param( g.get(), "p1");
	perspective_[2] = create_float_param( g.get(), "p2");
	perspective_[3] = create_float_param( g.get(), "p3");
	perspective_[4] = create_float_param( g.get(), "p4", 1.0f);
	perspective_[5] = create_float_param( g.get(), "p5");
	perspective_[6] = create_float_param( g.get(), "p6");
	perspective_[7] = create_float_param( g.get(), "p7");
	add_param( g);
}

void autoalign_base_node_t::get_perspective_params()
{
	perspective_[0] = dynamic_cast<float_param_t*>( &param( "p0"));
	perspective_[1] = dynamic_cast<float_param_t*>( &param( "p1"));
	perspective_[2] = dynamic_cast<float_param_t*>( &param( "p2"));
	perspective_[3] = dynamic_cast<float_param_t*>( &param( "p3"));
	perspective_[4] = dynamic_cast<float_param_t*>( &param( "p4"));
	perspective_[5] = dynamic_cast<float_param_t*>( &param( "p5"));
	perspective_[6] = dynamic_cast<float_param_t*>( &param( "p6"));
	perspective_[7] = dynamic_cast<float_param_t*>( &param( "p7"));
}

void autoalign_base_node_t::set_perspective_identity( float frame)
{
	for( int i = 0; i < 8; ++i)
	{
		RAMEN_ASSERT( perspective_[i]);
	}
	
	perspective_[0]->set_value_at_frame( 1, frame);
	perspective_[1]->set_value_at_frame( 0, frame);
	perspective_[2]->set_value_at_frame( 0, frame);
	perspective_[3]->set_value_at_frame( 0, frame);
	perspective_[4]->set_value_at_frame( 1, frame);
	perspective_[5]->set_value_at_frame( 0, frame);
	perspective_[6]->set_value_at_frame( 0, frame);
	perspective_[7]->set_value_at_frame( 0, frame);
}

void autoalign_base_node_t::set_perspective_params( float frame, const Imath::M33d& m)
{
	for( int i = 0; i < 8; ++i)
	{
		RAMEN_ASSERT( perspective_[i]);
	}

	perspective_[0]->set_value_at_frame( m[0][0], frame);
	perspective_[1]->set_value_at_frame( m[0][1], frame);
	perspective_[2]->set_value_at_frame( m[0][2], frame);
	perspective_[3]->set_value_at_frame( m[1][0], frame);
	perspective_[4]->set_value_at_frame( m[1][1], frame);
	perspective_[5]->set_value_at_frame( m[1][2], frame);
	perspective_[6]->set_value_at_frame( m[2][0], frame);
	perspective_[7]->set_value_at_frame( m[2][1], frame);
}

Imath::M33d autoalign_base_node_t::perspective_matrix_at_frame( float frame, int subsample) const
{
	for( int i = 0; i < 8; ++i)
	{
		RAMEN_ASSERT( perspective_[i]);
	}

	Imath::M33d m;
	m[0][0] = get_value_at_frame<float>( *perspective_[0], frame);
	m[0][1] = get_value_at_frame<float>( *perspective_[1], frame);
	m[0][2] = get_value_at_frame<float>( *perspective_[2], frame);
	
	m[1][0] = get_value_at_frame<float>( *perspective_[3], frame);
	m[1][1] = get_value_at_frame<float>( *perspective_[4], frame);
	m[1][2] = get_value_at_frame<float>( *perspective_[5], frame);

	m[2][0] = get_value_at_frame<float>( *perspective_[6], frame);
	m[2][1] = get_value_at_frame<float>( *perspective_[7], frame);
	return m;
}

// corner pin
void autoalign_base_node_t::create_corner_pin_params()
{
	std::auto_ptr<group_param_t> g( new group_param_t( "Corner Pin"));
	corner_[0] = create_float2_param( g.get(), "topleft" , Imath::V2f( 0, 0));
	corner_[1] = create_float2_param( g.get(), "topright", Imath::V2f( 1, 0));
	corner_[2] = create_float2_param( g.get(), "botleft" , Imath::V2f( 0, 1));
	corner_[3] = create_float2_param( g.get(), "botright", Imath::V2f( 1, 1));
	add_param( g);
}

void autoalign_base_node_t::get_corner_pin_params()
{
	corner_[0] = dynamic_cast<float2_param_t*>( &param( "topleft"));
	corner_[1] = dynamic_cast<float2_param_t*>( &param( "topright"));
	corner_[2] = dynamic_cast<float2_param_t*>( &param( "botleft"));
	corner_[3] = dynamic_cast<float2_param_t*>( &param( "botright"));
}

void autoalign_base_node_t::set_corner_pin_identity( float frame)
{
	for( int i = 0; i < 4; ++i)
	{
		RAMEN_ASSERT( corner_[i]);
	}
	
	corner_[0]->set_value_at_frame( Imath::V2f( 0, 0), frame);
	corner_[1]->set_value_at_frame( Imath::V2f( 1, 0), frame);
	corner_[2]->set_value_at_frame( Imath::V2f( 0, 1), frame);
	corner_[3]->set_value_at_frame( Imath::V2f( 1, 1), frame);
}

void autoalign_base_node_t::set_corner_pin_params( float frame, const Imath::M33d& m)
{
	for( int i = 0; i < 4; ++i)
	{
		RAMEN_ASSERT( corner_[i]);
	}

    Imath::Box2i src_area( format());
	src_area.max.x++;
	src_area.max.y++;

	// topleft
	Imath::V2d p( src_area.min);
	Imath::V2d q = p * m;
	corner_[0]->set_absolute_value_at_frame( q, frame);

	// topright
	p = Imath::V2d( src_area.max.x, src_area.min.y);
	q = p * m;
	corner_[1]->set_absolute_value_at_frame( q, frame);

	// botleft
	p = Imath::V2d( src_area.min.x, src_area.max.y);
	q = p * m;
	corner_[2]->set_absolute_value_at_frame( q, frame);

	// botright
	p = Imath::V2d( src_area.max);
	q = p * m;
	corner_[3]->set_absolute_value_at_frame( q, frame);
	
	Imath::M33d test = corner_pin_matrix_at_frame( frame, 1);
}

Imath::M33d autoalign_base_node_t::corner_pin_matrix_at_frame( float frame, int subsample) const
{
	for( int i = 0; i < 4; ++i)
	{
		RAMEN_ASSERT( corner_[i]);
	}
	
    Imath::Box2i src_area( format());
	src_area.max.x++;
	src_area.max.y++;

    boost::array<Imath::V2d, 4> src_pts;
    src_pts[0].x = src_area.min.x;
    src_pts[0].y = src_area.min.y;
    src_pts[1].x = src_area.max.x;
    src_pts[1].y = src_area.min.y;
    src_pts[2].x = src_area.max.x;
    src_pts[2].y = src_area.max.y;
    src_pts[3].x = src_area.min.x;
    src_pts[3].y = src_area.max.y;

    boost::array<Imath::V2d, 4> dst_pts;
    dst_pts[0] = get_absolute_value_at_frame<Imath::V2f>( *corner_[0], frame) / subsample;
    dst_pts[1] = get_absolute_value_at_frame<Imath::V2f>( *corner_[1], frame) / subsample;
    dst_pts[2] = get_absolute_value_at_frame<Imath::V2f>( *corner_[3], frame) / subsample;
    dst_pts[3] = get_absolute_value_at_frame<Imath::V2f>( *corner_[2], frame) / subsample;

    if( quad_is_convex( dst_pts))
	{
		boost::optional<Imath::M33d> m( quadToQuadMatrix( src_pts, dst_pts));
		
		if( m)
			return m.get();
	}
	
	return Imath::M33d( 0, 0, 0, 
						0, 0, 0, 
						0, 0, 0);
}

// util
float_param_t *autoalign_base_node_t::create_float_param( group_param_t *g, const std::string& str, float value)
{
	RAMEN_ASSERT( g);
	std::auto_ptr<float_param_t> p( new float_param_t( str));
	float_param_t *result = p.get();
	p->set_id( str);
	p->set_default_value( value);

	//g->set_secret( true);	
	//p->set_secret( true);
	g->add_param( p);
	return result;
}

float2_param_t *autoalign_base_node_t::create_float2_param( group_param_t *g, const std::string& str, const Imath::V2f& value)
{
	RAMEN_ASSERT( g);
	std::auto_ptr<float2_param_t> p( new float2_param_t( str));
	float2_param_t *result = p.get();
	p->set_id( str);
    p->set_numeric_type( numeric_param_t::relative_xy);
	p->set_default_value( value);

	//g->set_secret( true);
	//p->set_secret( true);
	g->add_param( p);
	return result;
}

} // namespace
} // namespace
