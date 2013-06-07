// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_TRACKER_NCC_TRACKER_HPP
#define RAMEN_TRACKER_NCC_TRACKER_HPP

#include<ramen/nodes/image/track/ncc_tracker_fwd.hpp>

#include<boost/noncopyable.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathColor.h>

#include<ramen/params/param_fwd.hpp>

#include<ramen/image/buffer.hpp>

#include<ramen/anim/float_curve.hpp>

namespace ramen
{

class tracker_areas_param_t;
	
namespace track
{

class ncc_tracker_t : boost::noncopyable
{
public:

	ncc_tracker_t( bool_param_t *active, 
				   color_param_t *color,
				  float2_param_t *track,
				  float2_param_t *offset,
				  tracker_areas_param_t *areas,
				  float_param_t *adapt_tol,
				  float_param_t *confidence);

	~ncc_tracker_t();
	
	bool active() const;
	
	Imath::Color4f color() const;
	
	Imath::V2f track_pos() const;
	Imath::V2f track_pos( float frame) const;
	
	void move_track_pos( const Imath::V2f& d);
	void move_track_pos( const Imath::V2f& d, float frame);
	void set_track_pos( const Imath::V2f& pos, float frame);
	
	Imath::V2f offset() const;
	void move_offset( const Imath::V2f& d);
	
	void set_confidence( float c);
	
	Imath::Box2f reference_area() const;
	Imath::Box2f reference_area( float frame) const;
	void move_reference_corner( int corner, const Imath::V2f& d, bool symmetric = false);
	
	Imath::Box2f search_area() const;
	Imath::Box2f search_area( float frame) const;	
	void move_search_corner( int corner, const Imath::V2f& d, bool symmetric = false);
	
	const anim::float_curve_t& track_x_curve() const;
	anim::float_curve_t& track_x_curve();
	
	const anim::float_curve_t& track_y_curve() const;
	anim::float_curve_t& track_y_curve();
	
	bool has_keys_at( float frame) const;
	void delete_keys( float frame);

	void param_changed( param_t *p);
	
	void clear_images();
	
	// reference
	bool valid_reference() const;	
	void invalidate_reference();
	void update_reference_pos( float frame);
	void update_reference( float frame, const image::buffer_t& pixels);
	
	// search
	void update_search( float frame, const image::buffer_t& pixels);
	
	// track
	void track( float ref_frame, float search_frame);
	
private:

	struct impl;
	impl *pimpl_;
	
	bool_param_t *active_;
	color_param_t *color_;
	float2_param_t *track_;
	float2_param_t *offset_;
	tracker_areas_param_t *areas_;
	float_param_t *adapt_tol_;
	float_param_t *confidence_;

	Imath::Box2f ref_area_;
	Imath::Box2f search_area_;
};

} // tracker
} // ramen

#endif
