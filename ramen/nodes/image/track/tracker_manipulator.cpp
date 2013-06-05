// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/nodes/image/track/tracker_manipulator.hpp>

#include<limits>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>
#include<adobe/algorithm/clamp.hpp>

#include<OpenEXR/ImathMath.h>

#include<ramen/nodes/image/track/tracker_node.hpp>
#include<ramen/nodes/image/track/ncc_tracker.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/viewer_strategy.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{
namespace
{

template<class Fun>
Fun for_each_point_in_path( const Imath::V2f& default_value, const anim::float_curve_t& xcurve, const anim::float_curve_t& ycurve, Fun f)
{
	anim::float_curve_t::const_iterator x_it( xcurve.begin()), x_end( xcurve.end());
	anim::float_curve_t::const_iterator y_it( ycurve.begin()), y_end( ycurve.end());

	while( x_it != x_end || y_it != y_end)
	{
		float xtime = std::numeric_limits<float>::max();
		float ytime = xtime;
		
		if( x_it != x_end)
			xtime = x_it->time();

		if( y_it != y_end)
			ytime = y_it->time();
			
		if( Imath::Math<float>::fabs( xtime - ytime) <= anim::keyframe_t::time_tolerance())
		{
			f( x_it->time(), x_it->value(), y_it->value());
			++x_it;
			++y_it;
		}
		else
		{
			if( xtime < ytime)
			{
				if( ycurve.empty())
					f( x_it->time(), x_it->value(), default_value.y);
				else
					f( x_it->time(), x_it->value(), ycurve.evaluate( x_it->time()));
				
				++x_it;
			}
			else
			{
				if( xcurve.empty())
					f( y_it->time(), default_value.x, y_it->value());
				else
					f( y_it->time(), xcurve.evaluate( y_it->time()), y_it->value());
				
				++y_it;
			}
		}
	}
	
	return f;	
}

void put_gl_vertex( float frame, float x, float y, float aspect) { gl_vertex2f( x * aspect, y);}

struct pick_point_in_path
{
	pick_point_in_path( const Imath::V2f& pos, float pixel_scale) : picked( false)
	{
		pos_ = pos;
		pixel_scale_ = pixel_scale;
	}
	
	void operator()( float f, float x, float y)
	{
		if( picked)
			return;
		
		if( manipulators::inside_pick_distance( pos_, Imath::V2f( x, y), pixel_scale_))
		{
			picked = true;
			frame = f;
		}
	}
	
	bool picked;
	float frame;
	
	Imath::V2f pos_;
	float pixel_scale_;
};

} // unnamed
	
tracker_manipulator_t::tracker_manipulator_t() : manipulator_t()
{
	picked_tracker_ = -1;
	mode_ = no_drag;
	picked_corner_ = -1;
	frame_ = 0;
}

const image::tracker_node_t *tracker_manipulator_t::tracker_node() const
{
	return dynamic_cast<const image::tracker_node_t*>( parent());
}

image::tracker_node_t *tracker_manipulator_t::tracker_node()
{
	return dynamic_cast<image::tracker_node_t*>( parent());
}

void tracker_manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
	for( int i = 0; i < tracker_node()->num_trackers(); ++i)
		draw_tracker( tracker_node()->trackers()[i], event);
}

bool tracker_manipulator_t::do_key_press_event( const ui::key_press_event_t& event)
{
	if( event.key == ui::key_event_t::backspace_key)
		return true;
	
	return false;
}

void tracker_manipulator_t::do_key_release_event( const ui::key_release_event_t& event)
{
	if( event.key == ui::key_event_t::backspace_key)
	{
		image::tracker_node_t *node = tracker_node();
		
		if( !node->any_tracker_active())
			return;
		
		bool any_key = false;
		float frame = node->composition()->frame();
		
		for( int i = 0; i < node->num_trackers(); ++i)
		{
			if( node->trackers()[i]->active())
			{
				if( node->trackers()[i]->has_keys_at( frame))
					any_key = true;
			}
		}
		
		if( any_key)
		{
			tracker_node()->param_set().begin_edit();
			
			for( int i = 0; i < node->num_trackers(); ++i)
			{
				if( node->trackers()[i]->active())
					node->trackers()[i]->delete_keys( frame);
			}
			
			tracker_node()->param_set().end_edit();			
			app().ui()->update_anim_editors();
			app().ui()->set_frame( frame - 1);
		}
	}
}

bool tracker_manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{ 
	picked_tracker_ = -1;
	
	image::tracker_node_t *node = tracker_node();
	
	for( int i = 0; i < node->num_trackers(); ++i)
	{
		if( pick_tracker( tracker_node()->trackers()[i], event))
		{
			picked_tracker_ = i;
			break;
		}
	}
	
	return picked_tracker_ != -1;
}

void tracker_manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	if( event.first_drag)
		tracker_node()->param_set().begin_edit();
	
	Imath::V2f offset( event.wpos - event.last_wpos);
	offset.x = offset.x / event.aspect_ratio;
	
	switch( mode_)
	{
		case track_drag:
		case offset_drag:
		{			
			if( mode_ == track_drag)
				tracker_node()->trackers()[picked_tracker_]->move_track_pos( offset);
			else
				tracker_node()->trackers()[picked_tracker_]->move_offset( offset);
		}
		break;

		case ref_drag:
		case search_drag:		
		{
			bool symmetric = !( event.modifiers & ui::event_t::shift_modifier);
			
			if( mode_ == ref_drag)
				tracker_node()->trackers()[picked_tracker_]->move_reference_corner( picked_corner_, offset, symmetric);
			else
				tracker_node()->trackers()[picked_tracker_]->move_search_corner( picked_corner_, offset, symmetric);
		}
		break;

		case track_key_drag:
			tracker_node()->trackers()[picked_tracker_]->move_track_pos( offset, frame_);
			tracker_node()->set_frame( tracker_node()->composition()->frame()); // <<-- TODO: check this. Fix bugs in floatX_param_t.
		break;
	};
	
	tracker_node()->update_widgets();
	app().ui()->update_anim_editors();
	event.view->update();
}

void tracker_manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( event.dragged)
		tracker_node()->param_set().end_edit();
	
	picked_tracker_ = -1;
	mode_ = no_drag;
	picked_corner_ = -1;
}

void tracker_manipulator_t::draw_tracker( const track::ncc_tracker_t* t, const ui::paint_event_t& event) const
{
	if( t->active())
	{
		Imath::Color4f c = t->color();		
		Imath::Color3c color( adobe::clamp( c.r, 0.0f, 1.0f) * 255,
							  adobe::clamp( c.g, 0.0f, 1.0f) * 255,
							  adobe::clamp( c.b, 0.0f, 1.0f) * 255);
		
		gl_line_width( 1);
		gl_point_size( default_control_point_size());
		
		draw_tracker_path( t, event, color);
				
		Imath::Box2f box( t->search_area());
        box = ImathExt::scale( box, event.aspect_ratio, 1);
		manipulators::draw_box( box, color, event.pixel_scale, true);

		box = t->reference_area();
        box = ImathExt::scale( box, event.aspect_ratio, 1);
		manipulators::draw_box( box, color, event.pixel_scale, true);
		
		float size = 10 / event.pixel_scale;
		Imath::V2f pos( t->track_pos());
		pos.x *= event.aspect_ratio;
		manipulators::draw_cross( pos, size, size, color, event.pixel_scale);
	}
}

void tracker_manipulator_t::draw_tracker_path( const track::ncc_tracker_t* t, const ui::paint_event_t& event, const Imath::Color3c& color) const
{
	Imath::V2f pos( t->track_pos());
	const anim::float_curve_t& xcurve = t->track_x_curve();
	const anim::float_curve_t& ycurve = t->track_y_curve();
	
	gl_color( color);
	
	gl_begin( GL_LINE_STRIP);
		for_each_point_in_path( pos, xcurve, ycurve, boost::bind( put_gl_vertex, _1, _2, _3, event.aspect_ratio));
	gl_end();
	
	gl_begin( GL_POINTS);
		for_each_point_in_path( pos, xcurve, ycurve, boost::bind( put_gl_vertex, _1, _2, _3, event.aspect_ratio));
	gl_end();	
}

bool tracker_manipulator_t::pick_tracker( const track::ncc_tracker_t *t, const ui::mouse_event_t& event)
{
	if( !t->active())
		return false;
	
	Imath::V2f pos( event.wpos);
	pos.x /= event.aspect_ratio;
		
	if( manipulators::inside_pick_distance( t->track_pos(), pos, event.pixel_scale))
	{
		if( event.modifiers & ui::event_t::control_modifier)
			mode_ = offset_drag;
		else
			mode_ = track_drag;

		return true;
	}

	Imath::Box2f box = t->search_area();
	manipulators::pick_box_result r = manipulators::pick_box( pos, box, event.pixel_scale);

	if( r != manipulators::box_no_pick)
	{
		mode_ = search_drag;
		picked_corner_ = ( int) r;
		return true;
	}

	box = t->reference_area();
	r = manipulators::pick_box( pos, box, event.pixel_scale);

	if( r != manipulators::box_no_pick)
	{
		mode_ = ref_drag;
		picked_corner_ = ( int) r;
		return true;
	}

	if( event.modifiers & ui::event_t::control_modifier)
	{
		if( box.intersects( pos))
		{
			mode_ = offset_drag;
			return true;
		}
	}	
	
	Imath::V2f track_pos( t->track_pos());
	const anim::float_curve_t& xcurve = t->track_x_curve();
	const anim::float_curve_t& ycurve = t->track_y_curve();	
	pick_point_in_path fun( pos, event.pixel_scale);
	fun = for_each_point_in_path( track_pos, xcurve, ycurve, fun);

	if( fun.picked)
	{
		mode_ = track_key_drag;
		frame_ = fun.frame;
		return true;
	}
	
	mode_ = no_drag;
	return false;
}

} // namespace
