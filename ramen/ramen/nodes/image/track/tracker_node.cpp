// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/nodes/image/track/tracker_node.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<OpenEXR/ImathMath.h>
#include<OpenEXR/ImathMatrix.h>

#include<ramen/math/constants.hpp>

#include<ramen/params/bool_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/tab_group_param.hpp>
#include<ramen/params/tracker_areas_param.hpp>

#include<ramen/nodes/image/track/ncc_tracker.hpp>
#include<ramen/nodes/image/track/tracker_manipulator.hpp>
#include<ramen/nodes/image/track/tracker_toolbar.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/render/context_guard.hpp>
#include<ramen/render/image_node_renderer.hpp>

#include<ramen/ui/user_interface.hpp>

#include<iostream>

namespace ramen
{
namespace image
{

tracker_node_t::tracker_node_t() : image_node_t(), tracking_( false)
{
    set_name( "tracker");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    param_set().param_changed.connect( boost::bind( &tracker_node_t::param_changed, this, _1, _2));
}

tracker_node_t::tracker_node_t( const tracker_node_t& other) : image_node_t( other), tracking_( false)
{ 
	create_trackers();
    param_set().param_changed.connect( boost::bind( &tracker_node_t::param_changed, this, _1, _2));
}

tracker_node_t::~tracker_node_t()
{
	if( !trackers_.empty())
	{
		for( int i = 0; i < num_trackers(); ++i)
			delete trackers()[i];
	}
}

std::auto_ptr<QWidget> tracker_node_t::create_toolbar()
{
	tracker_toolbar_t *toolbar = new tracker_toolbar_t( *this);
	return std::auto_ptr<QWidget>( toolbar);
}

void tracker_node_t::do_connected( node_t *src, int port)
{
	input_connection_.disconnect();
	
	if( src)
		input_connection_ = src->changed.connect( boost::bind( &tracker_node_t::input_changed, this, _1));
}

void tracker_node_t::input_changed( node_t *n)
{
	//std::cout << "input changed\n";
	
	for( int i = 0; i < num_trackers(); ++i)
		trackers()[i]->invalidate_reference();
}

void tracker_node_t::do_create_params()
{
	float tracker_positions[] = { 177, 121,
								  377, 121,
								  377, 331,
								  177, 331};
	
	float tracker_colors[] =  { 1, 0, 0,
								0, 1, 0,
								0, 0, 1,
								1, 0, 1};
	
    std::auto_ptr<tab_group_param_t> top( new tab_group_param_t());
    std::auto_ptr<composite_param_t> g( new composite_param_t( "Trackers"));
	
	for( int i = 0; i < num_trackers(); ++i)
	{
		std::string suffix( "1");
		suffix[0] += i;
		
		std::auto_ptr<bool_param_t> b( new bool_param_t( std::string( "Tracker ") + suffix));
		b->set_id( std::string( "active") + suffix);
		b->set_default_value( i == 0);
		g->add_param( b);
		
		std::auto_ptr<color_param_t> c( new color_param_t( "Color"));
		c->set_id( std::string( "color") + suffix);
		c->set_is_rgba( false);
		c->set_static( true);
		c->set_default_value( Imath::Color4f( tracker_colors[i*3], tracker_colors[i*3+1], tracker_colors[i*3+2], 1.0f));
		g->add_param( c);
		
		std::auto_ptr<float2_param_t> f( new float2_param_t( std::string( "Track") + suffix));
		f->set_id( std::string( "track") + suffix);
		f->set_track_mouse( false);
		f->set_include_in_hash( false);
		f->set_default_anim_auto_tangents( anim::keyframe_t::tangent_linear);
		f->set_default_value( Imath::V2f( tracker_positions[i*2], tracker_positions[i*2+1]));
		g->add_param( f);

		f.reset( new float2_param_t( "Offset"));
		f->set_id( std::string( "offset") + suffix);
		f->set_track_mouse( false);
		f->set_include_in_hash( false);
		f->set_default_value( Imath::V2f( 0, 0));		
		f->set_static( true);
		g->add_param( f);

		std::auto_ptr<tracker_areas_param_t> a( new tracker_areas_param_t());
		a->set_id( std::string( "area") + suffix);
		g->add_param( a);
		
		std::auto_ptr<float_param_t> f2( new float_param_t( "Confidence"));
		f2->set_id( std::string( "confidence") + suffix);
		f2->set_include_in_hash( false);
		f2->set_default_anim_auto_tangents( anim::keyframe_t::tangent_linear);
		f2->set_default_value( 0);
		f2->set_enabled( false);
		g->add_param( f2);
		
		if( i != num_trackers() - 1)
		{
			std::auto_ptr<separator_param_t> sep( new separator_param_t());
			g->add_param( sep);
		}
	}
	top->add_param( g);

    g.reset( new composite_param_t( "Settings"));
	{
		std::auto_ptr<float_param_t> p( new float_param_t( "Adapt Tolerance"));
		p->set_id( "adapt_tol");
		p->set_static( true);
		p->set_range( 0, 1);
		p->set_default_value( 0.9);
		g->add_param( p);
	}
	top->add_param( g);
	
	add_param( top);
	create_trackers();
}

void tracker_node_t::param_changed( param_t *p, param_t::change_reason reason)
{	
	if( reason != param_t::time_changed && !tracking_)
	{		
		for( int i = 0; i < num_trackers(); ++i)
			trackers()[i]->param_changed( p);
	}
}

void tracker_node_t::do_create_manipulators()
{
	std::auto_ptr<manipulator_t> m( new tracker_manipulator_t());
	add_manipulator( m);
}

bool tracker_node_t::do_is_identity() const { return true;}

void tracker_node_t::do_begin_active()
{
	for( int i = 0; i < num_trackers(); ++i)
		trackers()[i]->invalidate_reference();
}

void tracker_node_t::do_end_active()
{
	for( int i = 0; i < num_trackers(); ++i)
		trackers()[i]->clear_images();
}

int tracker_node_t::num_trackers() const { return 4;}

void tracker_node_t::create_trackers()
{
	trackers_.reserve( num_trackers());
	
	for( int i = 0; i < num_trackers(); ++i)
	{
		std::string suffix( "1");
		suffix[0] += i;

		bool_param_t *active = dynamic_cast<bool_param_t*>( &param( std::string( "active") + suffix));
		color_param_t *color = dynamic_cast<color_param_t*>( &param( std::string( "color") + suffix));
		float2_param_t *track = dynamic_cast<float2_param_t*>( &param( std::string( "track") + suffix));
		float2_param_t *offset = dynamic_cast<float2_param_t*>( &param( std::string( "offset") + suffix));
		tracker_areas_param_t *areas = dynamic_cast<tracker_areas_param_t*>( &param( std::string( "area") + suffix));
		float_param_t *adapt_tol = dynamic_cast<float_param_t*>( &param( "adapt_tol"));
		float_param_t *conf = dynamic_cast<float_param_t*>( &param( std::string( "confidence") + suffix));
		
		track::ncc_tracker_t *t = new track::ncc_tracker_t( active, color, track, offset, areas, adapt_tol, conf);
		trackers_.push_back( t);
	}
}

bool tracker_node_t::any_tracker_active() const
{
	for( int i = 0; i < num_trackers(); ++i)
	{
		if( trackers()[i]->active())
			return true;
	}
	
	return false;
}

void tracker_node_t::grab_references()
{
	if( !any_tracker_active())
		return;

	float frame = composition()->frame();
	image::buffer_t pixels = get_input_frame( frame);
	
	for( int i = 0; i < num_trackers(); ++i)
	{
		track::ncc_tracker_t *t = trackers()[i];
		
		if( t->active())
			t->update_reference( frame, pixels);
	}
}

void tracker_node_t::start_tracking()
{
	RAMEN_ASSERT( composition());
	
	param_set().begin_edit();
	tracking_ = true;
}

void tracker_node_t::end_tracking()
{
	param_set().end_edit( false);
	tracking_ = false;
}

void tracker_node_t::track_forward()
{
	if( !any_tracker_active())
		return;
	
	start_tracking();
	
	app().ui()->start_long_process();
	
	for( ; composition()->frame() < composition()->end_frame();)
	{
		float frame = composition()->frame();
		do_track_one( frame + 1);
		
		if( app().ui()->process_cancelled())
			break;
	}

	app().ui()->end_long_process();
	end_tracking();
}

void tracker_node_t::track_backwards()
{	
	if( !any_tracker_active())
		return;
	
	start_tracking();
	
	app().ui()->start_long_process();
	
	for( ; composition()->frame() > composition()->start_frame();)
	{
		float frame = composition()->frame();
		do_track_one( frame - 1);
		
		if( app().ui()->process_cancelled())
			break;
	}

	app().ui()->end_long_process();
	end_tracking();
}

void tracker_node_t::track_one_forward()
{	
	if( !any_tracker_active())
		return;

	start_tracking();
	
	float endf = composition()->end_frame();
	float frame = composition()->frame();
	
	if( frame < endf)
		do_track_one( frame + 1);
	
	end_tracking();
}

void tracker_node_t::track_one_backwards()
{
	if( !any_tracker_active())
		return;

	start_tracking();
	
	float startf = composition()->start_frame();
	float frame = composition()->frame();
	
	if( frame > startf)
		do_track_one( frame - 1);
	
	end_tracking();
}

void tracker_node_t::do_track_one( float next_frame)
{
	RAMEN_ASSERT( tracking_);
	
	image::buffer_t ref_pixels;
	bool ref_needed = false;

	for( int i = 0; i < num_trackers(); ++i)
	{
		track::ncc_tracker_t *t = trackers()[i];
		
		if( t->active() && !t->valid_reference())
		{
			ref_needed = true;
			break;
		}
	}

	float current_frame = composition()->frame();
	
	if( ref_needed)
		ref_pixels = get_input_frame( current_frame);

	for( int i = 0; i < num_trackers(); ++i)
	{
		track::ncc_tracker_t *t = trackers()[i];
		
		if( t->active())
		{
			if( t->valid_reference())
				t->update_reference_pos( current_frame);
			else
				t->update_reference( current_frame, ref_pixels);
		}
	}

	image::buffer_t search_pixels = get_input_frame( next_frame);
	
	for( int i = 0; i < num_trackers(); ++i)
	{
		track::ncc_tracker_t *t = trackers()[i];
		
		if( t->active())
		{
			t->update_search( current_frame, search_pixels);
			t->track( current_frame, next_frame);
		}
	}
	
	app().ui()->set_frame( next_frame);
	app().ui()->process_events();
}

image::buffer_t tracker_node_t::get_input_frame( float frame)
{
	image_node_t *inode = input_as<image_node_t>();

	render::context_t context = composition()->current_context();
	render::context_guard_t guard( context, inode);

	context.mode = render::analisys_render;
	context.result_node = inode;
	context.frame = frame;
		
	render::image_node_renderer_t r( context);
	r.render();
	return r.image();
}

// apply track
float tracker_node_t::angle_between_vectors( const Imath::V2f& v0, const Imath::V2f& v1) const
{
	Imath::M33f rot_v0_x(  v0.x, -v0.y, 0, 
						   v0.y,  v0.x, 0,
						      0,     0, 1);

	Imath::V2f test;
	rot_v0_x.multDirMatrix( v0, test);
	
	Imath::V2f v1x;
	rot_v0_x.multDirMatrix( v1, v1x);
	
	float result = std::atan2( v1x.y, v1x.x);
	return result * math::constants<float>::rad2deg();
}

void tracker_node_t::get_trs( float frame, Imath::V2f& t, float& r, float& s) const
{
	RAMEN_ASSERT( num_trackers() == 4);
	
	t.x = t.y = r = 0;
	s = 1;
	
	if( !trackers()[0]->active())
		return;

	t = trackers()[0]->track_pos( frame);
	
	int t2 = 0;
	
	if( trackers()[1]->active())
		t2 = 1;
	else
	{
		if( trackers()[2]->active())
			t2 = 2;
		else
		{
			if( trackers()[3]->active())
				t2 = 3;
		}
	}
	
	if( !t2)
		return;

	Imath::V2f p0 = trackers()[0]->track_pos( frame - 1);
	Imath::V2f p1 = t;
	Imath::V2f q0 = trackers()[t2]->track_pos( frame - 1);
	Imath::V2f q1 = trackers()[t2]->track_pos( frame);
	
	Imath::V2f v0 = q0 - p0;
	Imath::V2f v1 = q1 - p1;
	
	float l0 = v0.length2();
	float l1 = v1.length2();

	if( l0 != 0)
	{
		s = v1.length() / v0.length();
		v0.normalize();
	
		if( l1 != 0)
		{
			v1.normalize();
			r = angle_between_vectors( v0, v1);			
		}
	}
	else
		s = 0;
}

boost::optional<Imath::V2f> tracker_node_t::tracker_pos( int index, float frame) const
{
	RAMEN_ASSERT( index >= 0);
	
	if( index < num_trackers())
	{
		if( trackers()[index]->active())
			return trackers()[index]->track_pos( frame);
	}
	
	return boost::optional<Imath::V2f>();
}

void tracker_node_t::apply_track( float start_frame, float end_frame, 
								  apply_track_mode mode, apply_track_use use, const Imath::V2f& center,
								  float2_param_t *trans, float_param_t *rot, float2_param_t *scale) const
{
	// preconditions
	RAMEN_ASSERT( trans);
	
	if( use == trans_rot || use == trans_rot_scale)
		RAMEN_ASSERT( rot);

	if( use == trans_scale || use == trans_rot_scale)
		RAMEN_ASSERT( scale);
	
	float accum_r = 0.0f;
	float accum_s = 1.0f;
	
	for( float frame = start_frame; frame <= end_frame; frame += 1.0f)
	{
		Imath::V2f t;
		float r, s;
		get_trs( frame, t, r, s);
		
		if( mode == stab_mode)
		{
			trans->set_value_at_frame( -t - center, frame);
			
			r = -r;
			
			if( s != 0)
				s = 1.0f / s;
		}
		else
			trans->set_value_at_frame( t - center, frame);

		if( use == trans_rot || use == trans_rot_scale)
		{
			accum_r += r;
			rot->set_value_at_frame( accum_r, frame);
		}

		if( use == trans_scale || use == trans_rot_scale)
		{
			accum_s *= s;
			scale->set_value_at_frame( Imath::V2f( accum_s, accum_s), frame);
		}
	}
}

// factory
node_t *create_tracker_node() { return new tracker_node_t();}

const node_metaclass_t *tracker_node_t::metaclass() const { return &tracker_node_metaclass();}

const node_metaclass_t& tracker_node_t::tracker_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.tracker";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Track";
        info.menu_item = "Tracker";
        info.create = &create_tracker_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( tracker_node_t::tracker_node_metaclass());

} // namespace
} // namespace
