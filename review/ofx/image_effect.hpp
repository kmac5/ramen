// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_IMAGE_EFFECT_HPP
#define RAMEN_OFX_IMAGE_EFFECT_HPP

#include"ofxCore.h"
#include"ofxhImageEffect.h"

#include<boost/noncopyable.hpp>

#include<ramen/ofx/ofx_node_fwd.hpp>
#include<ramen/ofx/memory.hpp>

namespace ramen
{
namespace ofx
{

class image_effect_t : public OFX::Host::ImageEffect::Instance, boost::noncopyable
{
public:

    image_effect_t( OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                     OFX::Host::ImageEffect::Descriptor& desc,
                     const std::string& context, image::ofx_node_t *node);

    virtual ~image_effect_t();

    const image::ofx_node_t *node() const	{ return node_;}
    image::ofx_node_t *node()				{ return node_;}
    
	// clips
    OFX::Host::ImageEffect::ClipInstance *newClipInstance( OFX::Host::ImageEffect::Instance* plugin,
                                                          OFX::Host::ImageEffect::ClipDescriptor* descriptor,
                                                          int index);
	
	// params
    virtual OFX::Host::Param::Instance* newParam( const std::string& name, OFX::Host::Param::Descriptor& Descriptor);

    virtual OfxStatus editBegin( const std::string& name);
    virtual OfxStatus editEnd();
	
	// instance
	virtual OfxStatus beginInstanceChangedAction( const std::string& why);

	virtual OfxStatus paramInstanceChangedAction( const std::string& paramName,
												 const std::string& why,
												 OfxTime time, OfxPointD renderScale);

	virtual OfxStatus clipInstanceChangedAction( const std::string& clipName,
												 const std::string & why,
												OfxTime time, OfxPointD renderScale);

	virtual OfxStatus endInstanceChangedAction(const std::string& why);
	
	// info
    virtual void getProjectSize( double& xSize, double& ySize) const;
    virtual void getProjectOffset( double& xOffset, double& yOffset) const;
    virtual void getProjectExtent( double& xSize, double& ySize) const;

    virtual double getProjectPixelAspectRatio() const;
    virtual double getEffectDuration() const;

    virtual double getFrameRate() const;
    virtual double getFrameRecursive() const;

    virtual void getRenderScaleRecursive( double &x, double &y) const;
	virtual const std::string& getDefaultOutputFielding() const;

	// timeline
    virtual double timeLineGetTime();
    virtual void timeLineGotoTime( double t);
    virtual void timeLineGetBounds( double &t1, double &t2);
	
	// memory
	virtual memory_instance_t *newMemoryInstance(size_t nBytes);	
	
	// progress
    virtual void progressStart( const std::string &message);
    virtual void progressEnd();
    virtual bool progressUpdate( double t);
	
	// util	
	virtual OfxStatus vmessage(const char* type, const char* id, const char* format, va_list args);
	
private:

    image::ofx_node_t *node_;
};

} // namespace
} // namespace

#endif
