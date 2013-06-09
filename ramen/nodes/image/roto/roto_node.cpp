// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/roto_node.hpp>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/assert.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/button_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/motion_blur_param.hpp>
#include<ramen/params/inspector_param.hpp>

#include<ramen/nodes/image/roto/roto_shape_param.hpp>
#include<ramen/nodes/image/roto/toolbar.hpp>
#include<ramen/nodes/image/roto/manipulator.hpp>
#include<ramen/nodes/image/roto/scene_renderer.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/accumulator.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<iostream>

namespace ramen
{
namespace image
{
namespace
{

enum
{
	output_premult = 0,
	output_unpremult
};

enum
{
	replace_alpha = 0,
	use_alpha
};

enum
{
	update_mouse_drag = 0,
	update_mouse_up
};

} // unnamed

roto_node_t::roto_node_t() : image_node_t(), toolbar_( 0), scene_( this)
{
    set_name( "roto");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
	add_output_plug();
	inspector_ = 0;
	shape_param_ = 0;
    param_set().param_changed.connect( boost::bind( &roto_node_t::param_changed, this, _1, _2));	
}

roto_node_t::roto_node_t( const roto_node_t& other) : image_node_t( other), scene_( other.scene_)
{
	scene_.set_parent( this);
	scene_.update_all_xforms();
	toolbar_ = 0;
	inspector_ = 0;
    param_set().param_changed.connect( boost::bind( &roto_node_t::param_changed, this, _1, _2));

	shape_param_ = dynamic_cast<roto_shape_param_t*>( &param( "shape_panel"));
	RAMEN_ASSERT( shape_param_);

	inspector_ = dynamic_cast<inspector_param_t*>( &param( "inspector"));
	RAMEN_ASSERT( inspector_);
}

void roto_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> pop( new popup_param_t( "Output"));
    pop->set_id( "output");
    pop->menu_items() = boost::assign::list_of( "Premultiplied")( "Unpremultiplied");
    pop->set_default_value( 1);
    add_param( pop);

    pop.reset( new popup_param_t( "Input Alpha"));
    pop->set_id( "alpha");
    pop->menu_items() = boost::assign::list_of( "Replace")( "Use");
    add_param( pop);

    pop.reset( new popup_param_t( "Update"));
    pop->set_id( "update");
    pop->menu_items() = boost::assign::list_of( "Mouse Drag")( "Mouse Up");
	pop->set_include_in_hash( false);
    add_param( pop);
	
    std::auto_ptr<motion_blur_param_t> mb( new motion_blur_param_t( "Motion Blur"));
    mb->set_id( "motion_blur");
    add_param( mb);
	
	std::auto_ptr<separator_param_t> sep( new separator_param_t());
	add_param( sep);
	
	{
		std::auto_ptr<roto_shape_param_t> rs( new roto_shape_param_t( "shape_panel"));
		shape_param_ = rs.get();
		add_param( rs);
		
		std::auto_ptr<inspector_param_t> ip( new inspector_param_t( "inspector"));
		inspector_ = ip.get();
		add_param( ip);	
	}
	
	// test.
	selection_changed();
}

void roto_node_t::for_each_param( const boost::function<void ( param_t*)>& f)
{
	parameterised_t::for_each_param( f);
    boost::range::for_each( scene_, boost::bind( &parameterised_t::for_each_param, _1, f));
}

void roto_node_t::do_create_manipulators()
{
	std::auto_ptr<manipulator_t> m( new roto::manipulator_t());
	add_manipulator( m);
}

std::auto_ptr<QWidget> roto_node_t::create_toolbar()
{
	RAMEN_ASSERT( !toolbar_);
	toolbar_ = new roto::toolbar_t( *this);
	return std::auto_ptr<QWidget>( toolbar_);
}

const roto::toolbar_t& roto_node_t::toolbar() const
{
	RAMEN_ASSERT( toolbar_);
	return *toolbar_;
}

roto::toolbar_t& roto_node_t::toolbar()
{
	RAMEN_ASSERT( toolbar_);
	return *toolbar_;
}

bool roto_node_t::track_mouse() const
{
	return get_value<int>( param( "update")) == update_mouse_drag;
}

// shapes
std::auto_ptr<roto::shape_t> roto_node_t::create_shape() const
{
	std::auto_ptr<roto::shape_t> new_shape( new roto::shape_t());
	init_shape( *new_shape);
	return new_shape;
}

std::auto_ptr<roto::shape_t> roto_node_t::create_shape( const Imath::Box2f& b) const
{
	std::auto_ptr<roto::shape_t> new_shape( new roto::shape_t( b));
	init_shape( *new_shape);
	new_shape->set_name( "box");
	return new_shape;	
}

std::auto_ptr<roto::shape_t> roto_node_t::create_null() const
{
	std::auto_ptr<roto::shape_t> new_shape( new roto::shape_t());
	new_shape->set_is_null( true);
	init_shape( *new_shape);
	new_shape->set_name( "null");
	return new_shape;
}

void roto_node_t::init_shape( roto::shape_t &s) const
{
	s.create_params();
	s.set_autokey( autokey());
	s.set_track_mouse( track_mouse());
}

void roto_node_t::add_shape( std::auto_ptr<roto::shape_t> s)
{
	scene_.add_shape( s);
	
	if( shape_param_)
		shape_param_->shape_list_changed();
}

std::auto_ptr<roto::shape_t> roto_node_t::release_shape( roto::shape_t *s)
{
	std::auto_ptr<roto::shape_t> shape( scene_.release_shape( s));

	if( shape_param_)
		shape_param_->shape_list_changed();
	
	return shape;
}

void roto_node_t::deselect_all()
{
    boost::range::for_each( scene_, boost::bind( &roto::shape_t::select, _1, false));
    boost::range::for_each( scene_, boost::bind( &roto::shape_t::deselect_all_points, _1));
}

roto::shape_t *roto_node_t::selected()
{
	BOOST_FOREACH( roto::shape_t& s, scene_)
	{
		if( s.selected())
			return &s;
	}

	return 0;
}

void roto_node_t::selection_changed()
{
	RAMEN_ASSERT( shape_param_);
	RAMEN_ASSERT( inspector_);
	
	roto::shape_t *sel = selected();
	shape_param_->set_active_shape( sel);
	inspector_->set_parameterised( sel);
}

// time
void roto_node_t::do_set_frame( float f)
{ 
	scene().set_frame( f);
	scene().update_all_xforms();
}

void roto_node_t::do_create_tracks( anim::track_t *parent)
{
    BOOST_FOREACH( param_t& p, param_set())		{ p.create_tracks( parent);}
	BOOST_FOREACH( roto::shape_t& s, scene())	{ s.create_tracks( parent);}
}

void roto_node_t::do_update_widgets()
{
	BOOST_FOREACH( roto::shape_t& s, scene())	{ s.update_widgets();}
}

// areas & process
void roto_node_t::do_calc_bounds( const render::context_t& context)
{
	frames_.clear();

	motion_blur_param_t *mb = dynamic_cast<motion_blur_param_t*>( &param( "motion_blur"));
	RAMEN_ASSERT( mb);
	
    motion_blur_info_t::loop_data_t d( mb->loop_data( context.frame, context.motion_blur_extra_samples,
													   context.motion_blur_shutter_factor));

	Imath::Box2f bbox;
	
	if( d.num_samples == 1)
		bbox = scene().bounding_box();
	else
	{
	    float t = d.start_time;

	    for( int i = 0; i < d.num_samples; ++i)
	    {
			frames_.push_back( t);
			t += d.time_step;
		}

		bbox = scene().bounding_box( frames_);
	}
	
	scene_bbox_.min.x = std::floor( ( double) bbox.min.x);
	scene_bbox_.min.y = std::floor( ( double) bbox.min.y);
	scene_bbox_.max.x =  std::ceil( ( double) bbox.max.x);
	scene_bbox_.max.y =  std::ceil( ( double) bbox.max.y);
	
	if( get_value<int>( param( "alpha")) == replace_alpha && get_value<int>( param( "output")) == output_premult)
	{
		Imath::Box2i b( input_as<image_node_t>()->bounds());
		set_bounds( ImathExt::intersect( b, scene_bbox_));
	}
	else
		set_bounds( input_as<image_node_t>()->bounds());
}

void roto_node_t::do_calc_inputs_interest( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();

	if( get_value<int>( param( "alpha")) == replace_alpha && get_value<int>( param( "output")) == output_premult)
		in->add_interest( ImathExt::intersect( interest(), scene_bbox_));
	else
		in->add_interest( interest());
}

// hash
void roto_node_t::do_calc_hash_str( const render::context_t& context)
{
	image_node_t::do_calc_hash_str( context);
	
	if( frames_.empty())
		scene().add_to_hash_str( hash_generator());
	else
		scene().add_to_hash_str( frames_, hash_generator());
}

// process
void roto_node_t::do_process( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
    Imath::Box2i area( ImathExt::intersect( in->defined(), defined()));

    if( area.isEmpty())
		return;

	int output = get_value<int>( param( "output"));
	int composite = get_value<int>( param( "alpha"));

	boost::gil::copy_pixels( in->const_subimage_view( area), subimage_view( area));

	motion_blur_param_t *mb = dynamic_cast<motion_blur_param_t*>( &param( "motion_blur"));
	RAMEN_ASSERT( mb);
	
    motion_blur_info_t::loop_data_t d( mb->loop_data( context.frame, context.motion_blur_extra_samples,
													   context.motion_blur_shutter_factor));

	if( d.num_samples == 1)
	{
		roto::scene_renderer_t renderer( scene(), area, aspect_ratio(), context.subsample);
		
		if( composite == use_alpha)
			boost::gil::copy_pixels( boost::gil::nth_channel_view( in->const_subimage_view( area), 3), renderer.view());
		
		renderer.render();
		boost::gil::copy_pixels( renderer.const_view(), boost::gil::nth_channel_view( subimage_view( area), 3));
	}
	else
		do_process_motion_blur( context, area);

	if( output == output_premult)
		image::premultiply( subimage_view( area), subimage_view( area));
}

void roto_node_t::do_process_motion_blur( const render::context_t& context, const Imath::Box2i& area)
{
	image_node_t *in = input_as<image_node_t>();
	
	motion_blur_param_t *mb = dynamic_cast<motion_blur_param_t*>( &param( "motion_blur"));
	RAMEN_ASSERT( mb);

	image::gray_image_t pixels( area.size().x + 1, area.size().y + 1);
	boost::gil::fill_pixels( boost::gil::view( pixels), image::gray_pixel_t( 0));
	
	image::gray_accumulator_t acc( boost::gil::view( pixels));
	
    motion_blur_info_t::loop_data_t d( mb->loop_data( context.frame,
													  context.motion_blur_extra_samples,
													   context.motion_blur_shutter_factor));

	float t = d.start_time;
	float sumw = 0.0f;
	int composite = get_value<int>( param( "alpha"));

	for( int i = 0; i < d.num_samples; ++i)
	{
		scene().set_frame( t, true);
		scene().update_all_xforms( true);
				
		roto::scene_renderer_t renderer( scene(), area, aspect_ratio(), context.subsample);
		
		if( composite == use_alpha)
			boost::gil::copy_pixels( boost::gil::nth_channel_view( in->const_subimage_view( area), 3), renderer.view());

		renderer.render();
		
		float w = d.weight_for_time( t);
		acc.accumulate( renderer.const_view(), w);
		sumw += w;
		
		t += d.time_step;
	}
	
	if( sumw != 0.0f)
		acc.multiply( 1.0f / sumw);
	
	boost::gil::copy_pixels( boost::gil::const_view( pixels), boost::gil::nth_channel_view( subimage_view( area), 3));
	
	scene().set_frame( context.frame);
	scene().update_all_xforms();
}

void roto_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	if( reason != param_t::time_changed && p == &param( "update"))
	{
		bool b = track_mouse();

		BOOST_FOREACH( roto::shape_t& s, scene())
			s.set_track_mouse( b);
	}
}

// serialization
void roto_node_t::do_read( const serialization::yaml_node_t& node, const std::pair<int,int>& version)
{ 
	scene().read( node);
}

void roto_node_t::do_write( serialization::yaml_oarchive_t& out) const { scene().write( out);}

// factory
node_t *create_roto_node() { return new roto_node_t();}

const node_metaclass_t *roto_node_t::metaclass() const { return &roto_node_metaclass();}

const node_metaclass_t& roto_node_t::roto_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.roto";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Matte";
        info.menu_item = "Roto";
        info.create = &create_roto_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( roto_node_t::roto_node_metaclass());

} // namespace
} // namespace
