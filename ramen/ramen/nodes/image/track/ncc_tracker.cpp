// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/nodes/image/track/ncc_tracker.hpp>
#include<ramen/nodes/image/track/ncc_tracker_impl.hpp>

#include<algorithm>

#include<ramen/params/bool_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/tracker_areas_param.hpp>

#include<iostream>

namespace ramen
{
namespace track
{
namespace
{

Imath::Box2f drag_box_corner( const Imath::Box2f& b, int corner, const Imath::V2f& d, bool symmetric = false)
{
	Imath::Box2f result( b);
	
	if( symmetric)
	{
		switch( corner)
		{
			case 0:  // box_topleft_corner_picked
				result.min.x -= d.x;
				result.max.x -= d.x;
				result.min.y -= d.y;
				result.max.y -= d.y;
			break;
			
			case 1:  // box_topright_corner_picked
				result.min.x += d.x;
				result.max.x += d.x;
				result.min.y -= d.y;
				result.max.y -= d.y;
			break;

			case 2:  // box_botleft_corner_picked
				result.min.x -= d.x;
				result.max.x -= d.x;
				result.min.y += d.y;
				result.max.y += d.y;
			break;
			
			case 3:  // box_botright_corner_picked
				result.min.x += d.x;
				result.max.x += d.x;
				result.min.y += d.y;
				result.max.y += d.y;
			break;
	
			case 4: // box_top_edge_picked
				result.min.y -= d.y;
				result.max.y -= d.y;
			break;
			
			case 5: // box_left_edge_picked			
				result.min.x -= d.x;
				result.max.x -= d.x;
			break;
				
			case 6: // box_bot_edge_picked
				result.min.y += d.y;
				result.max.y += d.y;
			break;
			
			case 7: // box_right_edge_picked
				result.min.x += d.x;
				result.max.x += d.x;
			break;
		}		
	}
	else
	{
		switch( corner)
		{
			case 0: // box_topleft_corner_picked
				result.min.x -= d.x;
				result.min.y -= d.y;
			break;
			
			case 1: // box_topright_corner_picked
				result.max.x += d.x;
				result.min.y -= d.y;
			break;
			
			case 2: // box_botleft_corner_picked
				result.min.x -= d.x;
				result.max.y += d.y;
			break;
			
			case 3: // box_botright_corner_picked
				result.max.x += d.x;
				result.max.y += d.y;
			break;
	
			case 4: // box_top_edge_picked
				result.min.y -= d.y;
			break;
			
			case 5: // box_left_edge_picked
				result.min.x -= d.x;
			break;
			
			case 6: // box_bot_edge_picked
				result.max.y += d.y;
			break;
			
			case 7: // box_right_edge_picked
				result.max.x += d.x;
			break;
		}
	}
	
	return result;	
}

} // unnamed

ncc_tracker_t::ncc_tracker_t( bool_param_t *active, 
							  color_param_t *color,
							  float2_param_t *track,
							  float2_param_t *offset,
							  tracker_areas_param_t *areas,
							  float_param_t *adapt_tol, 
							  float_param_t *confidence) : pimpl_( 0)
{
	RAMEN_ASSERT( active && color && track && offset && areas && adapt_tol && confidence);
	
	active_ = active;
	color_ = color;
	track_ = track;
	offset_ = offset;
	areas_ = areas;
	adapt_tol_ = adapt_tol;
	confidence_ = confidence;
	pimpl_ = new impl( get_value<float>( *adapt_tol_));
}

ncc_tracker_t::~ncc_tracker_t() { delete pimpl_;}

bool ncc_tracker_t::active() const { return get_value<bool>( *active_);}

Imath::Color4f ncc_tracker_t::color() const { return get_value<Imath::Color4f>( *color_);}

Imath::V2f ncc_tracker_t::track_pos() const					{ return get_value<Imath::V2f>( *track_);}
Imath::V2f ncc_tracker_t::track_pos( float frame) const		{ return get_value_at_frame<Imath::V2f>( *track_, frame);}

void ncc_tracker_t::move_track_pos( const Imath::V2f& d) { track_->set_value( track_pos() + d);}

void ncc_tracker_t::move_track_pos( const Imath::V2f& d, float frame)
{ 
	track_->set_value_at_frame( track_pos( frame) + d, frame);
}

void ncc_tracker_t::set_track_pos( const Imath::V2f& pos, float frame)
{
	track_->set_value_at_frame( pos, frame);
}

Imath::V2f ncc_tracker_t::offset() const				{ return get_value<Imath::V2f>( *offset_);}
void ncc_tracker_t::move_offset( const Imath::V2f& d)	{ offset_->set_value( offset() + d);}

void ncc_tracker_t::set_confidence( float c) { confidence_->set_value( c);}

Imath::Box2f ncc_tracker_t::reference_area() const
{
	tracker_areas_t areas( get_value<tracker_areas_t>( *areas_));
	Imath::V2f p( track_pos() + offset());

	Imath::Box2f box( p);
	box.min.x -= areas.reference.min.x;
	box.min.y -= areas.reference.min.y;
	box.max.x += areas.reference.max.x;
	box.max.y += areas.reference.max.y;
	return box;
}

Imath::Box2f ncc_tracker_t::reference_area( float frame) const
{
	tracker_areas_t areas( get_value<tracker_areas_t>( *areas_));
	Imath::V2f p( track_pos( frame) + offset());

	Imath::Box2f box( p);
	box.min.x -= areas.reference.min.x;
	box.min.y -= areas.reference.min.y;
	box.max.x += areas.reference.max.x;
	box.max.y += areas.reference.max.y;
	return box;
}

void ncc_tracker_t::move_reference_corner( int corner, const Imath::V2f& d, bool symmetric)
{
	tracker_areas_t areas( get_value<tracker_areas_t>( *areas_));
	Imath::Box2f new_ref_area = areas.reference;
	new_ref_area = drag_box_corner( new_ref_area, corner, d, symmetric);
	areas.set_reference_area( new_ref_area);
	areas_->set_value( areas);
}

Imath::Box2f ncc_tracker_t::search_area() const
{
	tracker_areas_t areas( get_value<tracker_areas_t>( *areas_));
	Imath::V2f p( track_pos() + offset());
	Imath::Box2f box( p);
	box.min.x -= areas.search.min.x;
	box.min.y -= areas.search.min.y;
	box.max.x += areas.search.max.x;
	box.max.y += areas.search.max.y;
	return box;
}

Imath::Box2f ncc_tracker_t::search_area( float frame) const
{
	tracker_areas_t areas( get_value<tracker_areas_t>( *areas_));
	Imath::V2f p( track_pos( frame) + offset());
	Imath::Box2f box( p);
	box.min.x -= areas.search.min.x;
	box.min.y -= areas.search.min.y;
	box.max.x += areas.search.max.x;
	box.max.y += areas.search.max.y;
	return box;
}

void ncc_tracker_t::move_search_corner( int corner, const Imath::V2f& d, bool symmetric)
{
	tracker_areas_t areas( get_value<tracker_areas_t>( *areas_));
	Imath::Box2f new_search_area = areas.search;
	new_search_area = drag_box_corner( new_search_area, corner, d, symmetric);
	areas.set_search_area( new_search_area);
	areas_->set_value( areas);
}

const anim::float_curve_t& ncc_tracker_t::track_x_curve() const { return track_->curve( 0);}
anim::float_curve_t& ncc_tracker_t::track_x_curve()				{ return track_->curve( 0);}

const anim::float_curve_t& ncc_tracker_t::track_y_curve() const { return track_->curve( 1);}
anim::float_curve_t& ncc_tracker_t::track_y_curve()				{ return track_->curve( 1);}

bool ncc_tracker_t::has_keys_at( float frame) const
{
	return track_x_curve().has_keyframe_at( frame) ||
			track_y_curve().has_keyframe_at( frame);
}

void ncc_tracker_t::delete_keys( float frame)
{
	track_->param_set()->add_command( track_);
	track_->curve( 0).erase( frame);
	track_->curve( 1).erase( frame);

	confidence_->param_set()->add_command( confidence_);
	confidence_->curve( 0).erase( frame);
	
	track_->curve( 0).recalc_tangents_and_coefficients();
	track_->curve( 1).recalc_tangents_and_coefficients();
	confidence_->curve( 0).recalc_tangents_and_coefficients();
}

void ncc_tracker_t::param_changed( param_t *p)
{
	if( p == track_ || p == offset_ || p == areas_)
	{
		invalidate_reference();
		return;
	}
	
	if( p == adapt_tol_)
		pimpl_->set_adapt_tolerance( get_value<float>( *adapt_tol_));
}

void ncc_tracker_t::clear_images() { pimpl_->clear_images();}

bool ncc_tracker_t::valid_reference() const { return pimpl_->valid_reference();}
void ncc_tracker_t::invalidate_reference()	{ pimpl_->invalidate_reference();}

void ncc_tracker_t::update_reference_pos( float frame)
{ 
	pimpl_->set_reference_pos( reference_area( frame).min);
}

void ncc_tracker_t::update_reference( float frame, const image::buffer_t& pixels)
{
	pimpl_->update_reference( reference_area( frame), pixels);
}

void ncc_tracker_t::update_search( float frame, const image::buffer_t& pixels)
{
	pimpl_->update_search( search_area( frame), pixels);
}

void ncc_tracker_t::track( float ref_frame, float search_frame)
{
	Imath::V2f offset = pimpl_->track();
	Imath::V2f pos = track_pos( ref_frame);
	set_track_pos( pos + offset, search_frame);
	set_confidence( pimpl_->confidence());
}

} // track
} // ramen
