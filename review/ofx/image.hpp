// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_IMAGE_HPP
#define RAMEN_OFX_IMAGE_HPP

#include"ofxCore.h"

#include"ofxhPropertySuite.h"
#include"ofxhClip.h"

#include<ramen/image/buffer.hpp>

namespace ramen
{
namespace ofx
{

class image_t : public OFX::Host::ImageEffect::Image
{
public:
	
	image_t( OFX::Host::ImageEffect::ClipInstance& instance, image::buffer_t image, 
			 double renderScale, void* data, const OfxRectI &bounds, const OfxRectI &rod, int rowBytes, std::string uniqueIdentifier);
	
	~image_t();
	
private:
	
	image::buffer_t pixels_;
};
		
} // namespace
} // namespace

#endif
