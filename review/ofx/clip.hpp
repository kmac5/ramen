// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_CLIP_HPP
#define RAMEN_OFX_CLIP_HPP

#include<ramen/ofx/ofx_node_fwd.hpp>
#include<ramen/ofx/image.hpp>

namespace ramen
{
namespace ofx
{

class clip_t : public OFX::Host::ImageEffect::ClipInstance
{
public:
	
	clip_t( OFX::Host::ImageEffect::Instance *effectInstance, OFX::Host::ImageEffect::ClipDescriptor& desc);

    const image::ofx_node_t *node() const  { return node_;}
    image::ofx_node_t *node()		    { return node_;}

    int port() const { return port_;}

    const std::string& getUnmappedBitDepth() const;
    virtual const std::string& getUnmappedComponents() const;
    virtual const std::string& getPremult() const;
    virtual double getAspectRatio() const;
    virtual double getFrameRate() const;
    virtual double getUnmappedFrameRate() const;
	
    virtual void getFrameRange(double &startFrame, double &endFrame) const ;
    virtual void getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) const;

    virtual const std::string& getFieldOrder() const;
    virtual bool getContinuousSamples() const;
	
	virtual bool getConnected() const;

	virtual OfxRectD getRegionOfDefinition( OfxTime time) const;
	virtual image_t *getImage( OfxTime time, OfxRectD *optionalBounds);
	
private:
	
	image_t *get_input_image( OfxTime time, OfxRectD *optionalBounds);
	image_t *get_output_image( OfxTime time, OfxRectD *optionalBounds);
	
    image::ofx_node_t *node_;
    int port_;
};
	
} // namespace
} // namespace

#endif
