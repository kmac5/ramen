// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/clip.hpp>

#include<sstream>

#include"ofxImageEffect.h"

#include<ramen/assert.hpp>

#include<ramen/app/preferences.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/util.hpp>

#include<ramen/render/context_guard.hpp>
#include<ramen/render/image_node_renderer.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{
namespace
{
	
void *view_get_ptr_and_stride( const image::const_image_view_t& src, int& rowbytes)
{
	image::const_image_view_t::xy_locator loc( src.xy_at( 0, src.height() - 1));
	rowbytes = - (int) loc.row_size();
	return const_cast<void*>( reinterpret_cast<const void*>( &( *src.row_begin( src.height() - 1))));
}

} // unnamed	
	
clip_t::clip_t( OFX::Host::ImageEffect::Instance *effectInstance,
			   OFX::Host::ImageEffect::ClipDescriptor& desc) : OFX::Host::ImageEffect::ClipInstance( effectInstance, desc)
{
	node_ = dynamic_cast<image_effect_t*>( effectInstance)->node();
	RAMEN_ASSERT( node_);
	port_ = node_->index_for_clip_name( desc.getName());
}

const std::string& clip_t::getUnmappedBitDepth() const
{
    static const std::string sfloat( kOfxBitDepthFloat);
    static const std::string snone( kOfxBitDepthNone);

    if( node()->all_inputs_optional())
		return sfloat;

    if( getConnected())
		return sfloat;

    return snone;
}

const std::string& clip_t::getUnmappedComponents() const
{
    static const std::string srgba( kOfxImageComponentRGBA);
    static const std::string snone( kOfxImageComponentNone);

    if( node()->all_inputs_optional())
	{
		#ifndef NDEBUG
			DLOG( INFO) << "clip_t::getUnmappedComponents, node = " << node()->name() << ", port = " << port() << ", result = srgba";
		#endif
			
		return srgba;
	}

    if( getConnected())
	{
		#ifndef NDEBUG
			DLOG( INFO) << "clip_t::getUnmappedComponents, node = " << node()->name() << ", port = " << port() << ", result = srgba";
		#endif
			
		return srgba;
	}

	#ifndef NDEBUG
		DLOG( INFO) << "clip_t::getUnmappedComponents, node = " << node()->name() << ", port = " << port() << ", result = none";
	#endif
		
    return snone;
}

const std::string& clip_t::getPremult() const
{
    static const std::string spremult( kOfxImagePreMultiplied);
    return spremult;
}

double clip_t::getAspectRatio() const 
{ 
	float aspect = 1.0f;
	
	if( port_ != -1)
	{
		if( const image_node_t *in = node()->input_as<const image_node_t>( port()))
			aspect = in->aspect_ratio();
	}
	else
		aspect = node()->aspect_ratio();
	
	#ifndef NDEBUG
		DLOG( INFO) << "clip_t::getAspectRatio, node = " << node()->name() << ", port = " << port() << ", result = " << aspect;
	#endif
		
	return aspect;
}

double clip_t::getFrameRate() const
{
    if( node()->composition())
		return node()->composition()->frame_rate();
    else
		return 25.0;
}

double clip_t::getUnmappedFrameRate() const { return getFrameRate();}

void clip_t::getFrameRange(double &startFrame, double &endFrame) const
{
    if( node()->composition())
    {
		startFrame = node()->composition()->start_frame();
		endFrame = node()->composition()->end_frame();
    }
    else
    {
		startFrame = 1;
		endFrame = 100;
    }
}

void clip_t::getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) const
{
	getFrameRange( unmappedStartFrame, unmappedEndFrame);
}

const std::string& clip_t::getFieldOrder() const
{
    static const std::string sfieldnone( kOfxImageFieldNone);
    return sfieldnone;
}

bool clip_t::getContinuousSamples() const { return false;}

bool clip_t::getConnected() const
{
    // is output
    if( port_ == -1)
		return true;
    else
    {
		if( node_->num_inputs() == 0)
		    return false;

		if( node_->input( port_) != 0)
		    return true;
		else
		    return false;
    }
}

OfxRectD clip_t::getRegionOfDefinition( OfxTime time) const
{
    RAMEN_ASSERT( node());

    const image_node_t *in = 0;
	bool use_default = false;
	bool restore_time = false;
	float saved_frame;
	
	// calling getRoD for the output clip does not make a lot of sense, but some plugins do...
	if( port_ == -1) 
		in = node();		
	else // normal case
		in = node()->input_as<const image_node_t>( port_);

	if( !in)
	{
		in = node();
		use_default = true;
	}
	
	if( node()->composition())
	{
		saved_frame = node()->composition()->frame();
		
		if( saved_frame != time)
		{
			restore_time = true;
			composition_t *c = const_cast<composition_t*>( node()->composition());
			c->set_frame( time);
		}
	}
	
	Imath::Box2i b;

	if( use_default)
	{
		for( int i = 0; i < in->num_inputs(); ++i)
		{
			if( const image_node_t *src = in->input_as<const image_node_t>( i))
				b.extendBy( src->bounds());
		}

		if( b.isEmpty())
		{
			if( in->composition())
				b = node()->composition()->default_format().area();
			else
			    b = preferences_t::Instance().default_format().area();
		}
	}
	else
		b = in->bounds();
	
	b = node()->vertical_flip( b);

    OfxRectD v;
	v.x1 = b.min.x;
	v.y1 = b.min.y;
	v.x2 = b.max.x + 1;
	v.y2 = b.max.y + 1;
	
	if( restore_time)
	{
		composition_t *c = const_cast<composition_t*>( node()->composition());
		c->set_frame( saved_frame);		
	}
	
	#ifndef NDEBUG
		DLOG( INFO) << "clip_t::getRoD, node = " << node()->name() << ", port = " << port() << ", result = " << v;
	#endif
		
    return v;
}

image_t *clip_t::getImage (OfxTime time, OfxRectD *optionalBounds)
{
    if( getName() == "Output")
		return get_output_image( time, optionalBounds);
    else
		return get_input_image( time, optionalBounds);	
}
		
image_t *clip_t::get_input_image( OfxTime time, OfxRectD *optionalBounds)
{ 
    RAMEN_ASSERT( node());
	RAMEN_ASSERT( node()->composition());
	RAMEN_ASSERT( port_ != -1);
	RAMEN_ASSERT( getPixelDepth() == kOfxBitDepthFloat);
	
    image_node_t *in = node()->input_as<image_node_t>( port());

    if( !in)
	{
		#ifndef NDEBUG
			DLOG( INFO) << "clip_t::getImage, node = " << node()->name() << ", port = " << port() << ", result = 0";
		#endif
			
		return 0;
	}

	render::context_t context = node()->render_context();
    context.composition = node()->composition();
    context.result_node = in;
    context.frame = time;
		
	render::context_guard_t guard( node()->composition()->current_context(), in);
	render::image_node_renderer_t r( context);

    Imath::Box2i area;

    if( optionalBounds)
    {
        // TODO: is this correct if the effect does not support tiles?
		area = Imath::Box2i( Imath::V2i( optionalBounds->x1, optionalBounds->y1), Imath::V2i( optionalBounds->x2 - 1, optionalBounds->y2 - 1));
		area = node()->vertical_flip( area);
		r.render( area);

		area.min.x = optionalBounds->x1;
		area.min.y = optionalBounds->y1;
		area.max.x = optionalBounds->x2 - 1;
		area.max.y = optionalBounds->y2 - 1;
		area = node()->vertical_flip( area);
		area = intersect( in->defined(), area);
    }
    else
	{
		r.render();
		area = in->defined();
	}

    if( area.isEmpty())
	{
		#ifndef NDEBUG
			DLOG( INFO) << "clip_t::getImage, node = " << node()->name() << ", port = " << port() << ", area == empty";
		#endif
			
		return 0;
	}

    image::buffer_t pixels = in->image();
    image::const_image_view_t view( in->const_subimage_view( area));

    area = node()->vertical_flip( area);

    OfxRectI bounds;
    bounds.x1 = area.min.x;
    bounds.y1 = area.min.y;
    bounds.x2 = area.max.x + 1;
    bounds.y2 = area.max.y + 1;

    std::stringstream s;
    for( int i = 0; i < 16; ++i)
		s << (int) in->digest()[i];
	

	#ifndef NDEBUG
		if( optionalBounds)
			DLOG( INFO) << "clip_t::getImage, node = " << node()->name() << ", port = " << port() << ", bounds = " << *optionalBounds;
		else
			DLOG( INFO) << "clip_t::getImage, node = " << node()->name() << ", port = " << port();
	#endif
	
	image_t *result = 0;
	
	if( getComponents() == kOfxImageComponentRGBA)
	{
		int rowbytes;
		void *ptr = view_get_ptr_and_stride( view, rowbytes);
	    result = new image_t( *this, pixels, 1.0 / node()->render_context().subsample, ptr, bounds, bounds, rowbytes, s.str());
	}
	else
	{
		// TODO: create an gray scale image and copy the alpha channel
		RAMEN_ASSERT( 0);
	}
	
    in->release_image();
    return result;
}

image_t *clip_t::get_output_image( OfxTime time, OfxRectD *optionalBounds)
{
    RAMEN_ASSERT( node());
	RAMEN_ASSERT( node()->composition());
    RAMEN_ASSERT( !node()->image_empty());
	RAMEN_ASSERT( time == node()->composition()->frame());
	RAMEN_ASSERT( getComponents() == kOfxImageComponentRGBA);
	RAMEN_ASSERT( getPixelDepth() == kOfxBitDepthFloat);
	
    Imath::Box2i area;

    if( optionalBounds)
    {
		area = Imath::Box2i( Imath::V2i( optionalBounds->x1, optionalBounds->y1), Imath::V2i( optionalBounds->x2 - 1, optionalBounds->y2 - 1));
		area = Imath::scale( area, 1.0f / node()->render_context().subsample);
		area = node()->vertical_flip( area);
    }
    else
		area = node()->defined();

    if( area.isEmpty())
	{
		#ifndef NDEBUG
			DLOG( INFO) << "clip_t::getOutputImage, node = " << node()->name() << ", area == empty";
		#endif
			
		return 0;
	}
	
    image::const_image_view_t view( node()->const_subimage_view( area));

    int rowbytes;
    void *ptr = view_get_ptr_and_stride( view, rowbytes);

    // convert to OFX coordinate sys
    area = node()->vertical_flip( area);

    OfxRectI bounds;
    bounds.x1 = area.min.x;
    bounds.y1 = area.min.y;
    bounds.x2 = area.max.x + 1;
    bounds.y2 = area.max.y + 1;
	
	#ifndef NDEBUG
		DLOG( INFO) << "clip_t::getOutputImage, node = " << node()->name();
	#endif

    return new image_t( *this, node()->image(), 1.0 / node()->render_context().subsample, ptr, bounds, bounds, rowbytes, std::string( ""));
}

} // namespace
} // namespace
