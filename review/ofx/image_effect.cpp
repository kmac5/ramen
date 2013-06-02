// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/assert.hpp>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/host.hpp>
#include<ramen/ofx/clip.hpp>
#include<ramen/ofx/param.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/manipulator.hpp>
#include<ramen/ofx/util.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/preferences.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ui/user_interface.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{

image_effect_t::image_effect_t(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
								 OFX::Host::ImageEffect::Descriptor& desc, const std::string& context,
							   image::ofx_node_t *node) : OFX::Host::ImageEffect::Instance(plugin,desc,context,false)
{
	RAMEN_ASSERT( node);
	node_ = node;
}
	
image_effect_t::~image_effect_t() { endInstanceEditAction();}

OFX::Host::ImageEffect::ClipInstance *image_effect_t::newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
								      OFX::Host::ImageEffect::ClipDescriptor* descriptor,
								      int index)
{
    return new clip_t( this, *descriptor);
}

// make a parameter instance
OFX::Host::Param::Instance* image_effect_t::newParam( const std::string& name, OFX::Host::Param::Descriptor& descriptor)
{
    if( descriptor.getType()==kOfxParamTypeInteger)
		return new integer_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeDouble)
		return new double_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeBoolean)
		return new boolean_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeChoice)
		return new choice_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeRGBA)
		return new rgba_color_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeRGB)
		return new rgb_color_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeDouble2D)
		return new double2d_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypePushButton)
		return new pushbutton_param_t( name, descriptor, this);
	
	if( descriptor.getType() == kOfxParamTypeString)
		return new string_param_t( name, descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypeGroup)
		return new OFX::Host::Param::GroupInstance( descriptor, this);
	
    if( descriptor.getType()==kOfxParamTypePage)
		return new OFX::Host::Param::PageInstance( descriptor, this);

	#ifndef NDEBUG	
		DLOG( INFO) << "image_effect_t::newParam, unimplemented param type: " << descriptor.getType();
	#endif
		
	RAMEN_ASSERT( 0);
    return 0;
}

OfxStatus image_effect_t::editBegin( const std::string& name)
{
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::editBegin, name = " << name;
	#endif
		
    node()->param_set().begin_edit();
    return kOfxStatOK;
}

OfxStatus image_effect_t::editEnd()
{
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::editBegin, notify = " << !manipulator_t::mouse_down();
	#endif
		
    node()->param_set().end_edit( !manipulator_t::mouse_down());
    return kOfxStatOK;
}

// instance
OfxStatus image_effect_t::beginInstanceChangedAction( const std::string& why)
{
	OFX::Host::ImageEffect::Instance::beginInstanceChangedAction( why);
}

OfxStatus image_effect_t::paramInstanceChangedAction( const std::string& paramName,
											 const std::string& why,
											 OfxTime time, OfxPointD renderScale)
{
	OFX::Host::ImageEffect::Instance::paramInstanceChangedAction( paramName, why, time, renderScale);
}

OfxStatus image_effect_t::clipInstanceChangedAction( const std::string& clipName,
											 const std::string & why,
											OfxTime time, OfxPointD renderScale)
{
	OFX::Host::ImageEffect::Instance::clipInstanceChangedAction( clipName, why, time, renderScale);
}

OfxStatus image_effect_t::endInstanceChangedAction(const std::string& why)
{
	OFX::Host::ImageEffect::Instance::endInstanceChangedAction( why);
}

// info
void image_effect_t::getProjectSize( double& xSize, double& ySize) const
{
	Imath::Box2i proj_size = node()->format();
	xSize = proj_size.size().x + 1;
	ySize = proj_size.size().y + 1;
	
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::getProjectSize, x y = " << xSize << " " << ySize;
	#endif
}

void image_effect_t::getProjectOffset(double& xOffset, double& yOffset) const
{
	Imath::Box2i proj_size = node()->format();
	xOffset = proj_size.min.x;
	yOffset = proj_size.min.y;
	
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::getProjectOffset, x y = " << xOffset << " " << yOffset;	
	#endif
}

void image_effect_t::getProjectExtent(double& xSize, double& ySize) const
{
	Imath::Box2i proj_size = node()->format();
	xSize = proj_size.max.x + 1;
	ySize = proj_size.max.y + 1;
	
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::getProjectExtent, x y = " << xSize << " " << ySize;
	#endif
}

double image_effect_t::getProjectPixelAspectRatio() const
{
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::getProjectAspectRatio = " << node()->aspect_ratio();
	#endif
		
	return node()->aspect_ratio();
}

double image_effect_t::getEffectDuration() const
{
    if( node()->composition())
		return node()->composition()->end_frame() - node()->composition()->start_frame();
    else
		return 100;
}

double image_effect_t::getFrameRate() const
{
	if( node()->composition())
		return node()->composition()->frame_rate();
	
	return preferences_t::Instance().frame_rate();
}

double image_effect_t::getFrameRecursive() const
{
    if( node()->composition())
		return node()->composition()->frame();
    else
		return 1;
}

void image_effect_t::getRenderScaleRecursive( double& x, double& y) const
{
	x = 1.0 / node()->render_context().subsample;
	y = x;
	
	#ifndef NDEBUG
		DLOG( INFO) << "image_effect_t::getRenderScaleRecursive, x y = " << x << " " << y;
	#endif
}

const std::string& image_effect_t::getDefaultOutputFielding() const
{
    static const std::string v( kOfxImageFieldNone);
    return v;
}

// memory
memory_instance_t *image_effect_t::newMemoryInstance(size_t nBytes)
{
	memory_instance_t *instance = new memory_instance_t();
	instance->alloc( nBytes);
	return instance;
}

// progress
void image_effect_t::progressStart( const std::string &message) {}
void image_effect_t::progressEnd() {}
bool image_effect_t::progressUpdate( double t) { return true;}

// timeline
double image_effect_t::timeLineGetTime()
{
    double time = 1.0;

    if( node()->composition())
		time = node()->composition()->frame();

    return time;
}

void image_effect_t::timeLineGotoTime(double t)
{
    RAMEN_ASSERT( node());
    RAMEN_ASSERT( node()->composition());

    if( application_t::Instance().command_line())
		node()->composition()->set_frame( t);
    else
    {
		ui::user_interface_t::Instance().set_frame( t);
		ui::user_interface_t::Instance().process_events();
    }
}

void image_effect_t::timeLineGetBounds(double &t1, double &t2)
{
    if( node()->composition())
    {
		t1 = node()->composition()->start_frame();
		t2 = node()->composition()->end_frame();
    }
    else
    {
		t1 = 1;
		t2 = 100;
    }
}

// util
OfxStatus image_effect_t::vmessage(const char* type, const char* id, const char* format, va_list args)
{
    return ofx::host_t::Instance().vmessage( type, id, format, args);
}

} // namespace
} // namespace
