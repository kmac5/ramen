// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLIPBOOK_FLIPBOOK_HPP
#define RAMEN_FLIPBOOK_FLIPBOOK_HPP

#include<ramen/python/python.hpp>

#include<boost/noncopyable.hpp>
#include<boost/signals2/signal.hpp>

#include<OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include<ramen/image/buffer.hpp>

namespace ramen
{
namespace flipbook
{

class flipbook_t : boost::noncopyable
{
protected:
	
    flipbook_t( int frame_rate, 
				const std::string& display_device,
				const std::string& display_transform);
	
public:

	virtual ~flipbook_t();
	
    boost::signals2::signal<void ()> cancelled;

	// ui
	virtual void begin_progress() {}
	virtual void end_progress() {}
	
	int frame_rate() const { return frame_rate_;}
	
	virtual bool empty() const;
	
	virtual void set_frame_range( int start, int end);
	virtual void set_format( const Imath::Box2i& f, float aspect, int subsample);
	
	int image_width() const		{ return format_.size().x + 1;}
	int image_height() const	{ return format_.size().y + 1;}
	float aspect_ratio() const	{ return aspect_;}
	
	virtual void add_frame( int frame, image::buffer_t pixels);
		
	virtual void play() = 0;
	
protected:

	// view
	image::const_image_view_t const_view() const	{ return buffer_.const_rgba_view();}
	image::image_view_t view()						{ return buffer_.rgba_view();}
	
	// OCIO
	
	void apply_display_transform( const image::image_view_t& view);

	OCIO::ConstContextRcPtr get_local_context();
	
	int start_, end_;
	int frame_rate_;
	Imath::Box2i format_;
	Imath::Box2i defined_;
	float aspect_;
	
	// OCIO
	bool apply_display_transform_;	
	std::string display_device_;
	std::string display_transform_;
	OCIO::DisplayTransformRcPtr transform_;
	OCIO::ConstProcessorRcPtr processor_;
	
	image::buffer_t buffer_;
};

} // namespace
} // namespace

#endif
