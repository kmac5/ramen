// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/image.hpp>

#include"ofxImageEffect.h"

#include<ramen/ofx/util.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{

image_t::image_t( OFX::Host::ImageEffect::ClipInstance& instance, image::buffer_t image, double renderScale, void *data,
				  const OfxRectI &bounds, const OfxRectI &rod, int rowBytes,
				  std::string uniqueIdentifier) : OFX::Host::ImageEffect::Image( instance, renderScale, renderScale,
																				  data, bounds, rod, rowBytes,
																				  std::string( kOfxImageFieldNone),
																				  uniqueIdentifier)
{
	pixels_ = image;
	DLOG( INFO) << "image " << ( void *) this << " created. renderscale = " << renderScale
				<< " data = " << data << " bounds = " << bounds << " rod = " << rod
				<< " stride = " << rowBytes;
}

image_t::~image_t()
{
	#ifndef NDEBUG
		DLOG( INFO) << "image " << ( void *) this << " released.";
	#endif
}

} // namespace
} // namespace
