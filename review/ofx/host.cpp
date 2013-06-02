// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/host.hpp>

#include<string.h>

#include<cstdio>
#include<iostream>

#include"ofxhPluginCache.h"
#include"ofxhPropertySuite.h"

#include<ramen/ofx/threading.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ui/dialogs/splash_screen.hpp>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/util.hpp>

//#include<ramen/ofx/image_effect_plane.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{

host_impl::host_impl() : OFX::Host::ImageEffect::Host()
{
    _properties.setStringProperty( kOfxPropName, "Ramen");
    _properties.setStringProperty( kOfxPropLabel, "Ramen");
    _properties.setIntProperty( kOfxPropVersion, 0, 0);
    _properties.setIntProperty( kOfxPropVersion, 8, 1);
    _properties.setIntProperty( kOfxPropVersion, 0, 2);

    // we don't support yet all the 1.2 changes
    _properties.setIntProperty( kOfxPropAPIVersion, 1, 0);
    _properties.setIntProperty( kOfxPropAPIVersion, 1, 1);

    _properties.setIntProperty( kOfxImageEffectHostPropIsBackground, application_t::Instance().command_line());

    _properties.setIntProperty( kOfxImageEffectPropSupportsOverlays, 1);
    _properties.setIntProperty( kOfxImageEffectPropSupportsMultiResolution, 1);
    _properties.setIntProperty( kOfxImageEffectPropSupportsTiles, 1);
    _properties.setIntProperty( kOfxImageEffectPropTemporalClipAccess, 1);
    _properties.setIntProperty( kFnOfxImageEffectPropMultiPlanar, 1);

    _properties.setStringProperty( kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGBA, 0);
    //_properties.setStringProperty( kOfxImageEffectPropSupportedComponents, kFnOfxImageComponentMotionVectors, 1);

    _properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGenerator , 0);
    _properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextFilter	 , 1);
    _properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGeneral	 , 2);
    _properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextTransition, 3);

    _properties.setIntProperty( kOfxImageEffectPropSupportsMultipleClipDepths, 0);
    _properties.setIntProperty( kOfxImageEffectPropSupportsMultipleClipPARs, 0);
    _properties.setIntProperty( kOfxImageEffectPropSetableFrameRate, 0);
    _properties.setIntProperty( kOfxImageEffectPropSetableFielding, 0);
    _properties.setIntProperty( kOfxParamHostPropSupportsCustomInteract, 0);
    _properties.setIntProperty( kOfxParamHostPropSupportsStringAnimation, 0);
    _properties.setIntProperty( kOfxParamHostPropSupportsChoiceAnimation, 0);
    _properties.setIntProperty( kOfxParamHostPropSupportsBooleanAnimation, 0);
    _properties.setIntProperty( kOfxParamHostPropSupportsCustomAnimation, 0);

    _properties.setIntProperty( kOfxParamHostPropMaxParameters, -1);
    _properties.setIntProperty( kOfxParamHostPropMaxPages, 0);
    _properties.setIntProperty( kOfxParamHostPropPageRowColumnCount, 0, 0);
    _properties.setIntProperty( kOfxParamHostPropPageRowColumnCount, 0, 1);

    // init suites
    /*
    threading_suite_.multiThread = ofx::multiThread;
    threading_suite_.multiThreadNumCPUs = ofx::multiThreadNumCPUs;
    threading_suite_.multiThreadIndex = ofx::multiThreadIndex;
    threading_suite_.multiThreadIsSpawnedThread = ofx::multiThreadIsSpawnedThread;
    threading_suite_.mutexCreate = ofx::mutexCreate;
    threading_suite_.mutexDestroy = ofx::mutexDestroy;
    threading_suite_.mutexLock = ofx::mutexLock;
    threading_suite_.mutexUnLock = ofx::mutexUnLock;
    threading_suite_.mutexTryLock = ofx::mutexTryLock;

    planev1_suite_.clipGetImagePlane = ofx::clip_get_image_plane_v1;

    planev2_suite_.clipGetImagePlane            = ofx::clip_get_image_plane_v2;
    planev2_suite_.clipGetRegionOfDefinition    = ofx::clip_get_region_of_definition;
    planev2_suite_.getViewCount                 = ofx::get_view_count;
    planev2_suite_.getViewName                  = ofx::get_view_name;
     */
}

void host_impl::loadingStatus( const std::string& s)
{
    if( !application_t::Instance().command_line())
		ui::splash_screen_t::Instance().show_message( s);
}

bool host_impl::pluginSupported( OFX::Host::ImageEffect::ImageEffectPlugin *plugin, std::string& reason) const
{
    // test if the plugin support any context in which we work.
    if( !plugin->getDescriptor().isContextSupported( kOfxImageEffectContextGenerator) &&
	    !plugin->getDescriptor().isContextSupported( kOfxImageEffectContextFilter) &&
	    !plugin->getDescriptor().isContextSupported( kOfxImageEffectContextGeneral) &&
	    !plugin->getDescriptor().isContextSupported( kOfxImageEffectContextTransition))
    {
		#ifndef NDEBUG		
			DLOG( INFO) << "OFX Host: plugin " << plugin->getIdentifier() << " not supported: No supported context";
		#endif
			
		reason = "No supported context";		
		return false;
    }

    if( !plugin->getDescriptor().isPixelDepthSupported( kOfxBitDepthFloat))
    {
		#ifndef NDEBUG
			DLOG( INFO) << "OFX Host: plugin " << plugin->getIdentifier() << " not supported: Float bit depth not supported by plugin";
		#endif
			
		reason = "Float bit depth not supported by plugin";
		return false;
    }

	/*
    if( plugin->getDescriptor().isSingleInstance())
    {
		DLOG( INFO) << "OFX Host: plugin " << plugin->getIdentifier() << " not supported: Plugin is single instance";
		reason = "Single instance plugins not supported";
		return false;
    }
	*/

    return true;
}

OFX::Host::ImageEffect::Instance* host_impl::newInstance( void* clientData,
                                                          OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                                          OFX::Host::ImageEffect::Descriptor& desc,
                                                          const std::string& context)
{
    return new image_effect_t( plugin, desc, context, reinterpret_cast<image::ofx_node_t*>( clientData));
}

OFX::Host::ImageEffect::Descriptor *host_impl::makeDescriptor( OFX::Host::ImageEffect::ImageEffectPlugin* plugin)
{
    return new OFX::Host::ImageEffect::Descriptor( plugin);
}

OFX::Host::ImageEffect::Descriptor *host_impl::makeDescriptor( const OFX::Host::ImageEffect::Descriptor& rootContext,
                                                               OFX::Host::ImageEffect::ImageEffectPlugin *plugin)
{
    return new OFX::Host::ImageEffect::Descriptor( rootContext, plugin);
}

OFX::Host::ImageEffect::Descriptor *host_impl::makeDescriptor( const std::string& bundlePath, OFX::Host::ImageEffect::ImageEffectPlugin *plugin)
{
    return new OFX::Host::ImageEffect::Descriptor( bundlePath, plugin);
}

void *host_impl::fetchSuite( const char *suiteName, int suiteVersion)
{
    /*
    if( strcmp( suiteName, kOfxMultiThreadSuite)==0)
    {
	if( suiteVersion == 1)
	    return reinterpret_cast<void*>( &threading_suite_);
    }

    if( strcmp( suiteName, kFnOfxImageEffectPlaneSuite) == 0)
    {
	if( suiteVersion == 1)
	    return reinterpret_cast<void*>( &planev1_suite_);
    }
    */

    void *result = OFX::Host::ImageEffect::Host::fetchSuite( suiteName, suiteVersion);
	
	#ifndef NDEBUG
		if( result == 0)
		    DLOG( INFO) << "OFX Host: suite does not exist: " << suiteName << " version = " << suiteVersion;
	#endif

    return result;
}

OfxStatus host_impl::vmessage( const char *type, const char *id, const char *format, va_list args)
{
    std::string message;
    message.reserve( 1024);

    snprintf( &( *message.begin()), 1024, format, args);

    if( ( strcmp( type, kOfxMessageLog) == 0) || ( strcmp( type, kOfxMessageMessage) == 0))
    {
		application_t::Instance().inform( message);
		return kOfxStatOK;
    }

    if( strcmp( type, kOfxMessageFatal) == 0)
    {
		application_t::Instance().fatal_error( message);
		return kOfxStatOK;
    }

    if( strcmp( type, kOfxMessageError) == 0)
    {
		application_t::Instance().error( message);
		return kOfxStatOK;
    }

    if( strcmp( type, kOfxMessageQuestion) == 0)
    {
		if( application_t::Instance().question( message, true))
		    return kOfxStatReplyYes;
		else
		    return kOfxStatReplyNo;
    }

    return kOfxStatOK;
}

} // namespace
} // namespace
