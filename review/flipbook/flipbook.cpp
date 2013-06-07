// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/flipbook/flipbook.hpp>

#include<ramen/assert.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/image/ocio_transform.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace flipbook
{

flipbook_t::flipbook_t( int frame_rate, const std::string& display_device, const std::string& display_transform)
{
	frame_rate_ = frame_rate;
	aspect_ = 1.0f;

	display_device_ = display_device;
	display_transform_ = display_transform;
	
    try
    {
        OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
		OCIO::ConstContextRcPtr context = get_local_context();
		transform_ = OCIO::DisplayTransform::Create();
		transform_->setInputColorSpaceName( OCIO::ROLE_SCENE_LINEAR);
		transform_->setDisplay( display_device.c_str());
        transform_->setView( display_transform.c_str());
		processor_ = config->getProcessor( context, transform_, OCIO::TRANSFORM_DIR_FORWARD);
		apply_display_transform_ = true;
	}
	catch( OCIO::Exception& e)
	{
		app().ui()->error( e.what());
		apply_display_transform_ = false;
	}
}

flipbook_t::~flipbook_t() {}

bool flipbook_t::empty() const { return true;}

void flipbook_t::set_frame_range( int start, int end)
{
	start_ = start;
	end_ = end;
}

void flipbook_t::set_format( const Imath::Box2i& f, float aspect, int subsample)
{
	RAMEN_ASSERT( !f.isEmpty());
	
    format_ = ImathExt::scale( f, 1.0f / subsample);
	buffer_ = image::buffer_t( format_, 4);
	aspect_ = aspect;
}

void flipbook_t::play()
{
	// free tmp images
	buffer_ = image::buffer_t();
}

void flipbook_t::add_frame( int frame, image::buffer_t pixels)
{			
	boost::gil::fill_pixels( buffer_.rgba_view(), image::pixel_t( 0, 0, 0, 0));
	
    defined_ = ImathExt::intersect( pixels.bounds(), format_);
	
	if( !defined_.isEmpty())
		boost::gil::copy_pixels( pixels.const_rgba_subimage_view( defined_),
								 buffer_.rgba_subimage_view( defined_));

	if( apply_display_transform_)
		apply_display_transform( buffer_.rgba_view());
}

void flipbook_t::apply_display_transform( const image::image_view_t& view) { image::ocio_transform( view, processor_);}

OCIO::ConstContextRcPtr flipbook_t::get_local_context()
{
	OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
	OCIO::ConstContextRcPtr context = config->getCurrentContext();
		
    OCIO::ContextRcPtr mutable_context;
	
	std::pair<std::string, std::string> ctx_pair;

	ctx_pair = app().document().composition().ocio_context_pairs()[0];
	
    if( !ctx_pair.first.empty())
    {
        if( !mutable_context)
			mutable_context = context->createEditableCopy();

        mutable_context->setStringVar( ctx_pair.first.c_str(), ctx_pair.second.c_str());		
    }
	
	ctx_pair = app().document().composition().ocio_context_pairs()[1];
	
    if( !ctx_pair.first.empty())
	{
		if( !mutable_context)
			mutable_context = context->createEditableCopy();
		
        mutable_context->setStringVar( ctx_pair.first.c_str(), ctx_pair.second.c_str());		
    }

	ctx_pair = app().document().composition().ocio_context_pairs()[2];
	
    if( !ctx_pair.first.empty())
    {
		if( !mutable_context)
			mutable_context = context->createEditableCopy();
		
        mutable_context->setStringVar( ctx_pair.first.c_str(), ctx_pair.second.c_str());		
    }
	
	ctx_pair = app().document().composition().ocio_context_pairs()[3];
	
    if( !ctx_pair.first.empty())		
    {
        if(!mutable_context)
			mutable_context = context->createEditableCopy();

        mutable_context->setStringVar( ctx_pair.first.c_str(), ctx_pair.second.c_str());
    }
	
    if( mutable_context)
		context = mutable_context;
	
	return context;
}

} // namespace
} // namespace
