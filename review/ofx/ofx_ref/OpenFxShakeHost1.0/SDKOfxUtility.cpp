/*
 * Copyright (c) 2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_BSD_LICENSE_HEADER_START@
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * @APPLE_BSD_LICENSE_HEADER_END@
 */
/*
 * Copyright 1998-2006 Apple Computer, Inc.
 * All Rights Reserved.
 *
 * $Header: /cvsroot/openfxshakehost/OFX/SDKOfxUtility.cpp,v 1.2 2006/09/12 19:51:07 dcandela Exp $
 *
 *
 */

#include <SDKOfxUtility.h>
#include <NRiBuffer.h>
#include <NRiBytes.h>
#include <NRiCrop.h>
#include <NRiCursor.h>
#include <NRiEvSrc.h>
#include <NRiGlobals.h>
#include <NRiIBuffer.h>
#include <NRiLex.h>
#include <NRiMemOut.h>
#include <NRiReorder.h>
#include <NRiScript.h>
#include <NRiSPlug.h>
#include <NRiString.h>
#include <NRiUpdater.h>
#include <NRiViewer.h>

#include <ofxCore.h>
#include <ofxProperty.h>
#include <ofxImageEffect.h>

#include <SDKOfxPluginHost.h>
#include <SDKOfxEffect.h>

static NRiPlug *globalActionData      = NRiGlobals::def("sys.actionData",    0, kPtr);        // points to the data used by the action.
static NRiPlug *globalTime            = NRiGlobals::ref("script.time", kFloat);
static NRiPlug *globalPreProcessing   = NRiGlobals::ref("script.preProcessing", kInt);
static NRiPlug *globalScript          = NRiGlobals::ref("script.currentScript", kPtr);
static NRiPlug *globalUpdater	      = NRiGlobals::ref("gui._globalUpdater", kPtr);
static NRiPlug *globalDefAspect       = NRiGlobals::ref("script.defaultAspect", kFloat);
static NRiPlug *globalFramesPerSecond = NRiGlobals::ref("script.framesPerSecond", kFloat);
static NRiPlug * globalXProxyScale     = NRiGlobals::ref("script.xProxyScale", kFloat);
static NRiPlug * globalYProxyScale     = NRiGlobals::ref("script.yProxyScale", kFloat);

static SDKOfxPluginHost *thePluginHost;

static const char *shakeImagePtrProperty = "shakeImagePtr";
static const char *shakeImageRefCountProperty = "shakeImageRefCount";

int SDKOfxClip::_interrupted = 0;

SDKOfxBase::~SDKOfxBase()
{
    NRiHashIterator<SDKOfxProperty*> iterator(&m_properties);
    while(iterator.valid())
    {
        SDKOfxProperty *prop = iterator.value();
        delete(prop);
        iterator.next();
    }
}

SDKOfxClip::SDKOfxClip()
{
#ifdef _DEBUG
    printf("debug: SDKOfxClip 0x%X properySet = 0x%X   \n", (int)this, (int)getPropertySetHandle());
#endif    
    
    for(size_t i = 0; i < int(SDKOfx::kComponentEnumMax); ++i) m_supportedComponents[i] = false;
    createBaseProperties();
}


void SDKOfxClip::createBaseProperties()
{
    OfxPropertySetHandle clipProps = getPropertySetHandle();
    SDKOfxPluginHost::createProp(clipProps,kOfxPropName,kString);

    SDKOfxPluginHost::createProp(clipProps,kOfxPropType,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropType,0,kOfxTypeClip);
    SDKOfxPluginHost::createProp(clipProps,kOfxPropChangeReason,kString);    
    SDKOfxPluginHost::createProp(clipProps,kOfxPropTime,kDouble);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropRenderScale,kDouble,0,2);
    
    // Effect display name  - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxPropLabel,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropLabel,0,"EffectDisplayName");
    // Short effect name  - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxPropShortLabel,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropShortLabel,0,"EffectShortName");
    // long effect name  - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxPropLongLabel,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropLongLabel,0,"EffectLongName");
    // supported component types - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropSupportedComponents,kString);
    // clip needs temporal access - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropTemporalClipAccess,kInt);
    SDKOfxPluginHost::propSetInt(clipProps,kOfxImageEffectPropTemporalClipAccess,0,0);
    // clip supports tiles - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropSupportsTiles,kInt);
    SDKOfxPluginHost::propSetInt(clipProps,kOfxImageEffectPropSupportsTiles,0,1);
    // is clip optional - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropOptional,kInt);
    SDKOfxPluginHost::propSetInt(clipProps,kOfxImageClipPropOptional,0,0);
    // clip field extraction  - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropFieldExtraction,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageClipPropFieldExtraction,0,kOfxImageFieldDoubled);

    // is clip a mask - created, should be set by host when the effect descriptor is read
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropIsMask,kInt);
    SDKOfxPluginHost::propSetInt(clipProps,kOfxImageClipPropIsMask,0,0);
}

void SDKOfxClip::createInstanceProperties(const NRiString &clipName)
{
    OfxPropertySetHandle clipProps = getPropertySetHandle();
    
    SDKOfxPluginHost::propSetString(clipProps, kOfxPropName, 0, SDKOfx::mapClipName(clipName.toCStr()).toCStr());
    
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropPixelDepth,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageEffectPropPixelDepth,0,kOfxBitDepthNone);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropComponents,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageEffectPropComponents,0,kOfxImageComponentNone);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropFrameRange,kDouble);
    SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropFrameRange,0,1.0);
    SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropFrameRange,1,1.0);
    // TODO - determine if any of these parameters can affect the output
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropUnmappedPixelDepth,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageClipPropUnmappedPixelDepth,0,kOfxBitDepthNone);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropUnmappedComponents,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageClipPropUnmappedComponents,0,kOfxImageComponentNone);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropPreMultiplication,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageEffectPropPreMultiplication,0,kOfxImageUnPreMultiplied);
    SDKOfxPluginHost::createProp(clipProps,kOfxImagePropPixelAspectRatio,kDouble,1,1,1);
    SDKOfxPluginHost::propSetPlug(clipProps,kOfxImagePropPixelAspectRatio,0,globalDefAspect);    
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropFrameRate,kDouble,1,1,1);
    SDKOfxPluginHost::propSetPlug(clipProps,kOfxImageEffectPropFrameRate,0,globalFramesPerSecond);        
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropFieldOrder,kString);
    SDKOfxPluginHost::propSetString(clipProps,kOfxImageClipPropFieldOrder,0,kOfxImageFieldNone);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropConnected,kInt);
    SDKOfxPluginHost::propSetInt(clipProps,kOfxImageClipPropConnected,0,0);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropUnmappedFrameRange,kDouble);
    SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropUnmappedFrameRange,0,1.0);
    SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropUnmappedFrameRange,1,1.0);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageEffectPropUnmappedFrameRate,kDouble);
    SDKOfxPluginHost::createProp(clipProps,kOfxImageClipPropContinuousSamples,kInt);
    SDKOfxPluginHost::propSetInt(clipProps,kOfxImageClipPropContinuousSamples,0,0);
    SDKOfxPluginHost::createProp(clipProps, kOfxImagePropRegionOfDefinition, kInt, 0, 4);    

    OfxPropertySetHandle imagePropSet = (OfxPropertySetHandle)getImagePropertyTable();
    
    // These properties are used exclusively by Shake and are not used by the OFX plugin
    SDKOfxPluginHost::createProp(imagePropSet, shakeImageRefCountProperty, kInt);
    SDKOfxPluginHost::propSetInt(imagePropSet, shakeImageRefCountProperty, 0, 0);
    SDKOfxPluginHost::createProp(imagePropSet, shakeImagePtrProperty, kPtr);
    SDKOfxPluginHost::propSetPointer(imagePropSet, shakeImagePtrProperty, 0, 0);    

    // at this point we have an instance, fill in the array of supported components
#if 0
    int nSupportedComps = 0;
    if(SDKOfxPluginHost::propGetDimension(clipProps, kOfxImageEffectPropSupportedComponents, &nSupportedComps) == kOfxStatOK) {
      for(int d = 0; d < nSupportedComps; d++) {
        char *cstr;
        if(SDKOfxPluginHost::propGetString(clipProps, kOfxImageEffectPropSupportedComponents, d, &cstr) == kOfxStatOK) {
          NRiString compStr = cstr;
          if(compStr == kOfxImageComponentRGBA)
            m_supportedComponents[SDKOfx::eComponentRGBA] = true;
          else if(compStr == kOfxImageComponentAlpha)
            m_supportedComponents[SDKOfx::eComponentAlpha] = true;
        }
      }
    }
#endif
}



SDKOfxClip &SDKOfxClip::operator=( SDKOfxClip& clip )
{
    OfxPropertySetHandle clipProps = getPropertySetHandle();
    OfxPropertySetHandle ctrlClipProps = clip.getPropertySetHandle();
    NRiHashTable<SDKOfxProperty*> *ctrlClipTable= (NRiHashTable<SDKOfxProperty*> *)ctrlClipProps;

    NRiHashIterator<SDKOfxProperty*> ctrlClipIt(ctrlClipTable);
    while(ctrlClipIt.valid())
    {
        SDKOfxProperty*prop = ctrlClipIt.value();
        NRiName propName = ctrlClipIt.key();
        int i,dimension = prop->getDimension();
        int type = prop->getType();
        for (i=0;i<dimension;i++) {
            if (type = kInt) {
                SDKOfxPluginHost::propSetInt(clipProps,propName.getString(),i,prop->getValuePtr(i)->i);
            } else if (type == kString) {
                SDKOfxPluginHost::propSetString(clipProps,propName.getString(),i,prop->getNamePtr(i)->getString());
            } else if (type == kDouble) {
                SDKOfxPluginHost::propSetDouble(clipProps,propName.getString(),i,prop->getValuePtr(i)->d);
            } else if (type == kPtr) {
                SDKOfxPluginHost::propSetPointer(clipProps,propName.getString(),i,prop->getValuePtr(i)->p);
            } 
        }
        ctrlClipIt.next();
    }
    
    for(size_t i = 0; i < int(SDKOfx::kComponentEnumMax); ++i) m_supportedComponents[i] = false;
    int nSupportedComps = 0;
    if(SDKOfxPluginHost::propGetDimension(ctrlClipProps, kOfxImageEffectPropSupportedComponents, &nSupportedComps) == kOfxStatOK) {
      for(int d = 0; d < nSupportedComps; d++) {
        char *cstr;
        if(SDKOfxPluginHost::propGetString(ctrlClipProps, kOfxImageEffectPropSupportedComponents, d, &cstr) == kOfxStatOK) {
          NRiString compStr = cstr;
          if(compStr == kOfxImageComponentRGBA)
            m_supportedComponents[SDKOfx::eComponentRGBA] = true;
          else if(compStr == kOfxImageComponentAlpha)
            m_supportedComponents[SDKOfx::eComponentAlpha] = true;
        }
      }
    }
    return *this;
}

/// maps an Shake active mask into one that this clips supports
int SDKOfxClip::mapToSupportedActive(int active)
{
  if(m_supportedComponents[int(SDKOfx::eComponentAlpha)] && active == kA) {
    return kA;
  }
  else if(m_supportedComponents[int(SDKOfx::eComponentAlpha)] && !m_supportedComponents[int(SDKOfx::eComponentRGBA)]) {
    return kA;
  }
  else {
    return kRGBA;
  }
}
  
int SDKOfxClip::setImageProperties(int oBytes, const NRiIRect &dataRect, const NRiIRect &fullImageRect, int stride, const char *uniqueId, void *dataPtr, int components)
{
    // Set the clip instance properties that can change. The host
    // currently doesn't support changing the following properties.
    // kOfxImageEffectPropComponents - (always RGBA)
    // kOfxImageEffectPropPreMultiplication - (always kOfxImageUnPreMultiplied)
    // kOfxImageEffectPropRenderScale - (always 1.0)
    // kOfxImagePropPixelAspectRatio - (linked to script.defaultAspect)
    // kOfxImagePropField  - (always kOfxImageFieldNone)
    
    OfxPropertySetHandle propSet = (OfxPropertySetHandle)getImagePropertyTable();
    SDKOfxPluginHost::createProp(propSet,kOfxPropType,kString);
    SDKOfxPluginHost::propSetString(propSet,kOfxPropType,0,kOfxTypeImage);
    SDKOfxPluginHost::createProp(propSet,kOfxImageEffectPropPixelDepth,kString);
    SDKOfxPluginHost::propSetString(propSet,kOfxImageEffectPropPixelDepth,0, SDKOfx::mapToOfxDepth(oBytes));
    SDKOfxPluginHost::createProp(propSet,kOfxImageEffectPropComponents,kString);
    SDKOfxPluginHost::propSetString(propSet,kOfxImageEffectPropComponents,0,SDKOfx::mapToOfxComponents(components));
    SDKOfxPluginHost::createProp(propSet,kOfxImagePropData,kPtr);
    SDKOfxPluginHost::propSetPointer(propSet,kOfxImagePropData,0,dataPtr);
    SDKOfxPluginHost::createProp(propSet,kOfxImagePropRowBytes,kInt);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropRowBytes,0,(dataRect.X2 - dataRect.X1)*oBytes*4 + stride);
    SDKOfxPluginHost::createProp(propSet,kOfxImagePropBounds,kInt);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropBounds,0,dataRect.X1);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropBounds,1,dataRect.Y1);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropBounds,2,dataRect.X2);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropBounds,3,dataRect.Y2);

    SDKOfxPluginHost::createProp(propSet,kOfxImageEffectPropPreMultiplication,kString);
    SDKOfxPluginHost::propSetString(propSet,kOfxImageEffectPropPreMultiplication,0,kOfxImageUnPreMultiplied);

    // always frames
    SDKOfxPluginHost::createProp(propSet, kOfxImagePropField,kString);
    SDKOfxPluginHost::propSetString(propSet,kOfxImagePropField,0,kOfxImageFieldNone);

    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropRegionOfDefinition,0,fullImageRect.X1);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropRegionOfDefinition,1,fullImageRect.Y1);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropRegionOfDefinition,2,fullImageRect.X2);
    SDKOfxPluginHost::propSetInt(propSet,kOfxImagePropRegionOfDefinition,3,fullImageRect.Y2);

    SDKOfxPluginHost::createProp(propSet,kOfxImagePropUniqueIdentifier,kString);
    SDKOfxPluginHost::propSetString(propSet,kOfxImagePropUniqueIdentifier,0,uniqueId);
    SDKOfxPluginHost::createProp(propSet,kOfxImagePropPixelAspectRatio,kDouble,1,1,1);
    SDKOfxPluginHost::propSetPlug(propSet,kOfxImagePropPixelAspectRatio,0,globalDefAspect);    

    SDKOfxPluginHost::createProp(propSet,kOfxImageEffectPropRenderScale,kDouble);
    SDKOfxPluginHost::propSetDouble(propSet, kOfxImageEffectPropRenderScale, 0, globalXProxyScale->asDouble());
    SDKOfxPluginHost::propSetDouble(propSet, kOfxImageEffectPropRenderScale, 1, globalYProxyScale->asDouble());
    
    return 1;
}

static int interruptCallback( void *es )
{
    NRiEvSrc* src = static_cast<NRiEvSrc*>(es);
    SDKOfxClip::_interrupted = src->checkForInterrupt();
    return ( SDKOfxClip::_interrupted );
}


namespace SDKOfx {

  /// converts float to float
  inline void convertComponent(float src, float &dst)
  {
    dst = src;
  }

  /// converts float to 16bit
  inline void convertComponent(float src, uint16_t &dst)
  {
    dst = NRiF2Sn(src);
  }

  /// converts float to 8bit
  inline void convertComponent(float src, uint8_t &dst)
  {
    dst = NRiF2Bn(src);
  }


  /// converts 16bit to float
  inline void convertComponent(uint16_t src, float &dst)
  {
    dst = float(src)/65535.0f;
  }

  /// converts 16bit to 16bit
  inline void convertComponent(uint16_t src, uint16_t &dst)
  {
    dst = src;
  }

  /// converts 16bit to 8bit
  inline void convertComponent(uint16_t src, uint8_t &dst)
  {
    dst = (uint8_t)(src >> 8);
  }

  /// converts 8bit to float
  inline void convertComponent(uint8_t src, float &dst)
  {
    dst = float(src)/65535.0f;
  }

  /// converts 8bit to 8bit
  inline void convertComponent(uint8_t src, uint16_t &dst)
  {
    dst = src + (src << 8);
  }

  /// converts 8bit to 8bit
  inline void convertComponent(uint8_t src, uint8_t &dst)
  {
    dst = src;
  }

  /// wraps up a pixel slightly
  template<class COMPONENT, int NCOMPS, int COMPMAX>
  struct Pixel {
    typedef COMPONENT Component;
    static const int kNComps = NCOMPS;
    static const int kMax = COMPMAX;
    COMPONENT comps[kNComps];
  };
  
  typedef Pixel<uint8_t, 4, 255> PixRGBAB;
  typedef Pixel<uint16_t, 4, 65535> PixRGBAS;
  typedef Pixel<float, 4, 1> PixRGBAF;
  
  typedef Pixel<uint8_t, 1, 255> PixAB;
  typedef Pixel<uint16_t, 1, 65535> PixAS;
  typedef Pixel<float, 1, 1> PixAF;

  /// Converts an NRiIBuffer's pixels into OFX pixels
  template<class DSTPIX, class SRCPIX>
  void ConvertToOFXPixels(DSTPIX *dst,         // A or RGBA 8, 16 or float, kNComps indicates A or RGBA,
                          const NRiIRect &dstRect, // where that is defined
                          SRCPIX *srcBase,    // RGBA always, 8, 16 or float
                          NRiIBuffer *srcBuf)  // what SrcBase lives inside
  {
    NRiIRect srcDod = srcBuf->getDod();
    int dodWidth = srcBuf->dodWidth();
    int srcActivity = srcBuf->active();

    // is the activity of the back the same as the buffer?
    DSTPIX back;
    if(DSTPIX::kNComps==4) {
      convertComponent(back.comps[0], srcBuf->back.r);
      convertComponent(back.comps[1], srcBuf->back.g);
      convertComponent(back.comps[2], srcBuf->back.b);
      convertComponent(back.comps[3], srcBuf->back.a);
    }
    else {
      convertComponent(back.comps[0], srcBuf->back.a);
    }

    for (int y = dstRect.Y1; y < dstRect.Y2; y++) {
        SRCPIX *src = srcBase + (y-srcDod.Y1)*dodWidth + (dstRect.X1-srcDod.X1);
        for (int x = dstRect.X1; x < dstRect.X2; x++) {

          // TODO, make some interesects to remove this if statement
          if (x < srcDod.X1 || x >= srcDod.X2 || y < srcDod.Y1 || y >= srcDod.Y2)
            *dst = back;
          else {
            // this switch optimised out by any sane compiler
            switch(DSTPIX::kNComps) { 

            case 1 :// ALPHA
              if(srcActivity & kA) {
                convertComponent(src->comps[0], dst->comps[0]);
              }
              else {
                dst->comps[0] = 0;
              }
              break;
        
            case 4 : // RGBA
              if(srcActivity & kRGB) {
                convertComponent(src->comps[3], dst->comps[0]);
                convertComponent(src->comps[2], dst->comps[1]);
                convertComponent(src->comps[1], dst->comps[2]);
              }
              else if(srcActivity & kL) {
                typename DSTPIX::Component d;
                convertComponent(src->comps[2], d);
                dst->comps[0] = d;
                dst->comps[1] = d;
                dst->comps[2] = d;
              }
              else {
                // only alpha into an RGBA image, make RGB 0
                dst->comps[0] = 0;
                dst->comps[1] = 0;
                dst->comps[2] = 0;
              }

              // any alpha
              if(srcActivity & kA) {
                convertComponent(src->comps[0], dst->comps[3]);
              }
              else {
                dst->comps[3] = DSTPIX::kMax;
              }
              break;
            }
            
            src++;
            dst++;  
          }
        }
    }
  }

  /// templated function that turns a buffer of RGBARGBRGBA... into AAAA...
  template <class COMP>
  void convertToSingleAlpha(COMP *src, int width, int height)
  {
    COMP *dst = src;
    src += 3; // move it onto the A component
    int n = width * height;
    while(n--) {
      *dst = *src;
      dst++; 
      src+=4;
    }
  }
};


//  Prepare the image buffer requested by the plugin.
//  This request can come from various contexts each of which needs to handled uniquely.  
//  Possible contexts are : overlay, image render, tweaker (initialed by a parameter change)
//
//  The reason for the contexts is that we need to know where the image request is coming from in order to determine where to get the image data.  
//  If an overlay is requesting it, the image has already been rendered and we can grab it from the viewer buffer.  
//  The tweaker context basically means that a parameter changed action is requesting the buffer (eg. a button press).  We can't assume that anything particular 
//  is in the viewer so here we have to spawn a render to generate the image requested.   
//  The kGeneral action (ok, perhaps I could have named that something more obvious) is the case where the Shake engine is doing its normal image rendering.   
//  We don't do anything special in this case because the buffer has already set up by SDKOfxEffect.
//
//  time : we ignore this since Shake doesn't support temporal access.  Assume current time.
//

OfxStatus SDKOfxClip::getImage(OfxTime time, OfxRectD *region, OfxPropertySetHandle *imageHandle)
{
    OfxPropertySetHandle imagePropSet = (OfxPropertySetHandle)getImagePropertyTable();

    void *dataPtr = 0;
    SDKOfxPluginHost::propGetPointer(imagePropSet,kOfxImagePropData,0,&dataPtr);
    
    SDKOfxActionData *actionDataPtr = (SDKOfxActionData *)globalActionData->asPtr();
    if(!actionDataPtr && dataPtr == NULL) {
      // no image data n a render means no image, so politely fail
      *imageHandle = 0;
      return kOfxStatFailed;
    }
    
    int useCount;
    SDKOfxPluginHost::propGetInt(imagePropSet,shakeImageRefCountProperty, 0, &useCount);

    if(useCount == 0) {
      int isOutput = 0;
      NRiString clipName = getName();
      if (clipName == "ofx_Output") isOutput = 1;

      SDKOfxEffect *effectInstance = 0;
      SDKOfxPluginHost::propGetPointer(getPropertySetHandle(), SDKOfxEffect::shakeEffectProperty, 0, (void **)&effectInstance);
      int convertBytesTo = 0;
      int convertToComps = 0;
      if (effectInstance) {
        convertBytesTo = effectInstance->getClipBitDepth(getName().toCStr());
        convertToComps = effectInstance->getClipComponents(getName().toCStr());
      }
      else
        return kOfxStatFailed; // how?
    
    
      // Handle the overlay case
      if (actionDataPtr)
        {
          NRiIRect imgRect;
          const char *cacheIdStr;
        
          if (actionDataPtr->_actionContext == SDKOfxActionData::kTweaker)
            {
              _interrupted = 0;
              NRiScript *script = globalScript ? (NRiScript*)globalScript->asPtr() : 0;            
              NRiIPlug *inPlug = getInputPlug();
              NRiUpdater *updater = (NRiUpdater*)globalUpdater->asPtr();
              assert(updater);
              if (updater) updater->beginCalculations();
            
              // Get some preliminary info about frame about to be computed
              int width = inPlug->getWidth();
              int height = inPlug->getHeight();
              cacheIdStr = inPlug->getCacheId().getString();            
              int srcActive = inPlug->getActive() & ~kZ;
              imgRect.X1 = 0;
              imgRect.Y1 = 0;
              imgRect.X2 = width;
              imgRect.Y2 = height;
            
              if (region)
                {
                  if ( region->x1 < imgRect.X1 || region->x1 > imgRect.X2 ||
                       region->x2 < imgRect.X1 || region->x2 > imgRect.X2 ||
                       region->x2 < region->x1 || 
                       region->y1 < imgRect.Y1 || region->y1 > imgRect.Y2 ||
                       region->y2 < imgRect.Y1 || region->y2 > imgRect.Y2 ||
                       region->y2 < region->y1)
                    {
                      return kOfxStatFailed;
                    }
                  else
                    {
                      width = int(region->x2 - region->x1);
                      height = int(region->y2 - region->y1);
                      imgRect.X1 = int(region->x1);
                      imgRect.Y1 = int(region->y1);
                      imgRect.X2 = int(region->x2);
                      imgRect.Y2 = int(region->y2);
                    }
                }
                
              if (convertBytesTo == 0) convertBytesTo = inPlug->bytes()->asInt();
            
              NRiBytes *bytes = new NRiBytes;
              bytes->setFlag(NRiNode::kNoSerialization,1);
              bytes->setParent(script);            
              bytes->setName("__ofxBufferBytes", NRiNode::kNoDuplicateCheck);
              bytes->outBytes->set(convertBytesTo);
              bytes->in->connect(inPlug);

              NRiReorder *reorder = new NRiReorder;
              reorder->setFlag(NRiNode::kNoSerialization,1);            
              reorder->setParent(script);            
              reorder->setName("__ofxReorder", NRiNode::kNoDuplicateCheck);     
              // reorder to get buffer into rgba (ok this is confusing right?, Shake's internal format is ABGR so a reorder node with ABGR will put the internal buffer in RGBA)
              switch(srcActive) {
              case kL : reorder->pChannels->set("1ggg"); break;
              case kLA : reorder->pChannels->set("aggg"); break;
              case kRGB : reorder->pChannels->set("1bgr"); break;
              case kRGBA : reorder->pChannels->set("abgr"); break;
              case kA : reorder->pChannels->set("a000"); break;
              }
              reorder->in->connect(bytes->getOutput());
            
              NRiCrop * c = 0;
              if (region)
                {
                  c = new NRiCrop;
                  c->setFlag(NRiNode::kNoSerialization,1);            
                  c->setParent(script);
                  c->setName("__ofxCrop", NRiNode::kNoDuplicateCheck);                            
                  c->pX1->set(imgRect.X1);
                  c->pY1->set(imgRect.Y1);
                  c->pX2->set(imgRect.X2);
                  c->pY2->set(imgRect.Y2);
                  c->in->connect(reorder->getOutput());
                }
            
              NRiMemOut *memOut = new NRiMemOut;
              memOut->setFlag(NRiNode::kNoSerialization,1);
              memOut->setParent(script);
              memOut->setName("__ofxBufferMemOut", NRiNode::kNoDuplicateCheck);
              memOut->in->connect(region ? c->getOutput() : reorder->getOutput());
            
              script->initialize(globalTime->asFloat());

              _tmpBuf.allocate(width,height,convertBytesTo,4);
            
              // Setup memOut accordingly
              memOut->pPacked->set(1);
              memOut->pCStride->set(0);
              memOut->pBytes->set(convertBytesTo);
              memOut->pWidth->set(width);
              memOut->pHeight->set(height);
              memOut->pMask->set(kRGBA);            
              memOut->settings->render()->set(1);
              memOut->pZMemPtr->set(0);
              memOut->pZStride->set(0);        
              memOut->pCMemPtr->set(_tmpBuf.data.v);
            
              // Render
              NRiEvSrc *es = NRiEvSrc::getDefaultSrc();
              NRiCursor::find("cursors/hourglass.nri",es)->set((NRiWin*)(updater->getTopWidgetParent()));            
              void *icb= NRiNode::addInterruptCallback( interruptCallback, (void*)es );
            
              globalPreProcessing->set(1);
              globalActionData->set(0);                        // the render will call clipGetImage, don't want that going into here so set the action data to 0
              script->execute(globalTime->asFloat());
              globalActionData->set(actionDataPtr);            // reset it
              globalPreProcessing->set(0);
            
              NRiNode::removeInterruptCallback(icb);
              NRiCursor::find("cursors/arrow.nri",es)->set((NRiWin*)updater->getTopWidgetParent());
            
              delete memOut;
              delete bytes;   
              delete reorder;
              delete c;
            
              if (_interrupted) 
                {
                  _tmpBuf.deallocate();
                  return kOfxStatFailed;
                }

              // if single component alpha, we need to pack down the components in tmp buf
              if(convertToComps == kA) {
                switch(convertBytesTo) {
                case 4 :
                  SDKOfx::convertToSingleAlpha(_tmpBuf.data.f, width, height);
                  break;
                
                case 2 :
                  SDKOfx::convertToSingleAlpha(_tmpBuf.data.s, width, height);
                  break;
                
                case 1 :
                  SDKOfx::convertToSingleAlpha(_tmpBuf.data.b, width, height);
                  break;
                }
              }
              if (updater) updater->endCalculations();
            }
          else if (actionDataPtr->_actionContext == SDKOfxActionData::kOverlay)
            {
              NRiIBuffer *iiBuf = 0;        
              // If the clip is the output of the node, then grab the viewer buffer (pre-viewer LUT)
              if (isOutput)
                {
                  const int activeBuf = actionDataPtr->_viewer->activeBuffer()->asInt();
                  NRiVNode* pVNode = activeBuf==0 ? actionDataPtr->_viewer->vnodeA : actionDataPtr->_viewer->vnodeB;
                
                  NRiVScriptNode* pVScriptNode = dynamic_cast<NRiVScriptNode *>(pVNode);
                  if (pVScriptNode)
                    {
                      pVNode = pVScriptNode->getPreLookupVNode();
                    }
                  if (pVNode) iiBuf = pVNode->iBuf;
                }
              else // otherwise, grab the image buffer that is the first input to the viewed node.
                {
                  iiBuf= actionDataPtr->_viewer->vnodeInput ? actionDataPtr->_viewer->vnodeInput->iBuf : 0;            
                }
            
              if (iiBuf) 
                {
                  cacheIdStr = iiBuf->cacheId().getString();
                  NRiIRect dod = iiBuf->getDod();
                  imgRect = dod;
                  if (region)
                    {
                      if ( region->x1 < imgRect.X1 || region->x1 > imgRect.X2 ||
                           region->x2 < imgRect.X1 || region->x2 > imgRect.X2 ||
                           region->x2 < region->x1 || 
                           region->y1 < imgRect.Y1 || region->y1 > imgRect.Y2 ||
                           region->y2 < imgRect.Y1 || region->y2 > imgRect.Y2 ||
                           region->y2 < region->y1)
                        {
                          return kOfxStatFailed;
                        }
                      else
                        {
                          imgRect.X1 = int(region->x1);
                          imgRect.Y1 = int(region->y1);
                          imgRect.X2 = int(region->x2);
                          imgRect.Y2 = int(region->y2);
                        }
                    }
                  int width = imgRect.X2-imgRect.X1;
                  int height = imgRect.Y2-imgRect.Y1;                
                
                  if (convertBytesTo == 0) convertBytesTo = iiBuf->bytes();  /// is this healthy?
                  NRiBuffer fullBuf;
                  fullBuf.allocate(iiBuf->imgWidth()*iiBuf->imgHeight()*4*iiBuf->bytes());                
                  _tmpBuf.allocate(width*height*convertBytesTo * (convertToComps == kA ? 1 : 4));                
                  iiBuf->scrubLines(fullBuf.data.b,0,iiBuf->imgHeight(),0);                  // fill out the buffer so that we have all channels (ABGR)                
                
                  // Reorder the channels to RGBA which is what OFX needs.
                  switch(iiBuf->bytes()) {
                  case 4 :
                    switch(convertBytesTo) {
                    case 4 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAF *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAF *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAF *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAF *)fullBuf.data.f, iiBuf);                    
                      break;

                    case 2 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAS *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAF *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAS *)_tmpBuf.data.s, imgRect,
                                                   (SDKOfx::PixRGBAF *)fullBuf.data.f, iiBuf);
                      break;

                    case 1 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAB *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAF *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAB *)_tmpBuf.data.b, imgRect,
                                                   (SDKOfx::PixRGBAF *)fullBuf.data.f, iiBuf);
                      break;
                    }
                    break;

                  case 2 :
                    switch(convertBytesTo) {
                    case 4 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAF *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAS *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAF *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAS *)fullBuf.data.s, iiBuf);
                      break;

                    case 2 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAS *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAS *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAS *)_tmpBuf.data.s, imgRect,
                                                   (SDKOfx::PixRGBAS *)fullBuf.data.s, iiBuf);
                      break;

                    case 1 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAB *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAS *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAB *)_tmpBuf.data.b, imgRect,
                                                   (SDKOfx::PixRGBAS *)fullBuf.data.s, iiBuf);
                      break;
                    }
                    break;

                  case 1 :
                    switch(convertBytesTo) {
                    case 4 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAF *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAB *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAF *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAB *)fullBuf.data.b, iiBuf);
                      break;

                    case 2 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAS *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAB *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAS *)_tmpBuf.data.s, imgRect,
                                                   (SDKOfx::PixRGBAB *)fullBuf.data.b, iiBuf);
                      break;

                    case 1 :
                      if(convertToComps == kA) 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixAB *)_tmpBuf.data.f, imgRect,
                                                   (SDKOfx::PixRGBAB *)fullBuf.data.f, iiBuf);
                      else 
                        SDKOfx::ConvertToOFXPixels((SDKOfx::PixRGBAB *)_tmpBuf.data.b, imgRect,
                                                   (SDKOfx::PixRGBAB *)fullBuf.data.b, iiBuf);
                      break;
                    }
                    break;
                  }                                                    
                  if (convertBytesTo != iiBuf->bytes()) fullBuf.deallocate();
                }
              else
                {
                  NRiSys::error("%EFailed to get the image buffer for the overlay\n");
                }
            }

          setImageProperties(convertBytesTo, imgRect, imgRect, 0, cacheIdStr, (void *)_tmpBuf.data.b, convertToComps);
          SDKOfxPluginHost::propSetPointer(imagePropSet, shakeImagePtrProperty, 0, &_tmpBuf);    
                    
        }
    }      

    useCount += 1;
    SDKOfxPluginHost::propSetInt(imagePropSet,shakeImageRefCountProperty, 0, useCount);
    *imageHandle = imagePropSet;
    return kOfxStatOK;
}

OfxStatus SDKOfxClip::releaseImage(OfxPropertySetHandle imageHandle)
{
  SDKOfxActionData *actionDataPtr = (SDKOfxActionData *)globalActionData->asPtr();

  int useCount;
  SDKOfxPluginHost::propGetInt(imageHandle,shakeImageRefCountProperty, 0, &useCount);

  if (actionDataPtr && (actionDataPtr->_actionContext == SDKOfxActionData::kOverlay ||actionDataPtr->_actionContext == SDKOfxActionData::kTweaker)
      && useCount == 1)
    {
      NRiBuffer *tmpBuf;
      SDKOfxPluginHost::propGetPointer(imageHandle, shakeImagePtrProperty, 0, (void **)&tmpBuf);   
      if (tmpBuf) tmpBuf->deallocate(); 
      SDKOfxPluginHost::propSetPointer(imageHandle, shakeImagePtrProperty, 0, NULL);
    }
  useCount = useCount <= 0 ? 0 : useCount - 1;
  SDKOfxPluginHost::propSetInt(imageHandle,shakeImageRefCountProperty, 0, useCount);
  return kOfxStatOK;
}

// called after any effect action which may have fetched images and not freed them all
void SDKOfxClip::cleanUpFetchedImages()
{
  SDKOfxActionData *actionDataPtr = (SDKOfxActionData *)globalActionData->asPtr();
  OfxPropertySetHandle imageHandle = (OfxPropertySetHandle)getImagePropertyTable();
  int useCount;
  SDKOfxPluginHost::propGetInt(imageHandle,shakeImageRefCountProperty, 0, &useCount);
    
  // if the action it was fetched in was not a render, then we have to free it
  if (actionDataPtr && (actionDataPtr->_actionContext == SDKOfxActionData::kOverlay ||actionDataPtr->_actionContext == SDKOfxActionData::kTweaker)
      && useCount != 0) {
      NRiBuffer *tmpBuf;
      SDKOfxPluginHost::propGetPointer(imageHandle, shakeImagePtrProperty, 0, (void **)&tmpBuf);   
    if (tmpBuf) tmpBuf->deallocate(); 
    SDKOfxPluginHost::propSetPointer(imageHandle, shakeImagePtrProperty, 0, NULL);
  }

  // reset the ref count to zero
  SDKOfxPluginHost::propSetInt(imageHandle,shakeImageRefCountProperty, 0, 0);
}

SDKOfxProperty::SDKOfxProperty(int t, int rw, int d, int hasPlug):
_dimension(d),
_type(t),
_valuePtr(0),
_stringValue(0),
_readWrite(rw),
_plugListPtr(0)
{
  if(_dimension > 0) {
    // TODO creation bounds checking
    // Create new property and allocate memory to hold values
    _valuePtr = new NRiValue[_dimension];
    int i;
    for (i = 0; i< _dimension; i++)
      {
        _valuePtr[i].ui64 = 0;
      }
    _stringValue = new NRiName[_dimension];
    _plugListPtr = hasPlug ? new NRiPlug *[_dimension] : 0;
    if (hasPlug) {
      for (i = 0; i< _dimension; i++)
        {
          _plugListPtr[i] = 0;
        }
    }    
  }    
}

SDKOfxProperty::~SDKOfxProperty()
{
    delete [] _valuePtr;
    delete [] _stringValue;
    if (_plugListPtr) delete [] _plugListPtr;
}

void SDKOfxProperty::resize(int newDimension) 
{
    if (newDimension > 0) 
    {
        //create the new array
        NRiValue *newValues = new NRiValue[newDimension];
        NRiName *newStrings = new NRiName[newDimension];
        NRiPlug **newPlugList = 0;
        if (_plugListPtr) newPlugList = new NRiPlug *[newDimension];
        int i;
        int newBound = NRiMin(_dimension-1,newDimension-1);
        for (i=0;i<=newBound;i++) {
            if (_type == kInt) newValues[i].i = _valuePtr[i].i;
            if (_type == kFloat) newValues[i].f = _valuePtr[i].f;
            if (_type == kDouble) newValues[i].d = _valuePtr[i].d;
            if (_type == kPtr) newValues[i].p = _valuePtr[i].p;
            if (_type == kString) {
                newStrings[i] = _stringValue[i];
                newValues[i].n = _stringValue[i].getString();
            }
            if (_plugListPtr) newPlugList[i] = _plugListPtr[i];
        }
        delete [] _valuePtr;
        delete [] _stringValue;
        if (_plugListPtr) delete [] _plugListPtr;
        _valuePtr = newValues;
        _stringValue = newStrings;
        _dimension = newDimension;

        _plugListPtr = newPlugList;
    }
}

NRiValue *SDKOfxProperty::getValuePtr(int valueIndex)
{
    if (_plugListPtr)
    {  
        if (_plugListPtr[valueIndex]) {
            if (_type == kInt)         _valuePtr[valueIndex].i = _plugListPtr[valueIndex]->asInt();
            else if (_type == kFloat)  _valuePtr[valueIndex].f = _plugListPtr[valueIndex]->asFloat();
            else if (_type == kDouble) _valuePtr[valueIndex].d = _plugListPtr[valueIndex]->asDouble();
            else if (_type ==   kPtr)    _valuePtr[valueIndex].p = _plugListPtr[valueIndex]->asPtr();
            else if (_type == kString) _stringValue[valueIndex]= _plugListPtr[valueIndex]->asString();
        }
    }
    
    // Update the string pointer in case the NRiName has reallocated
    if (valueIndex < _dimension &&
        getType() == kString) 
    {
        _valuePtr[valueIndex].n = _stringValue[valueIndex].getString();
    }
    
    return (_valuePtr+valueIndex);
}


SDKOfxProperty &SDKOfxProperty::operator=( const SDKOfxProperty& property )
{
    _type = property._type;    
    
    if (_dimension < property._dimension)
    {
        resize(property._dimension);
    }
    
    int i;
    for (i=0; i< _dimension; i++) {
        if (_type == kInt) _valuePtr[i].i = property._valuePtr[i].i;
        if (_type == kFloat) _valuePtr[i].f = property._valuePtr[i].f;
        if (_type == kDouble) _valuePtr[i].d = property._valuePtr[i].d;
        if (_type == kPtr) _valuePtr[i].p = property._valuePtr[i].p;
        if (_type == kString) _stringValue[i] = property._stringValue[i];
        if (_plugListPtr) _plugListPtr[i] = property._plugListPtr[i];
    }
    return *this;    
}

SDKOfxParm::SDKOfxParm(const char *name, const char *type)
{
#ifdef _DEBUG
    printf("SDKOfxParm 0x%X properySet = 0x%X   name = %s\n", (int)this, (int)getPropertySetHandle(), name);
#endif    
    setName(name);
    m_parmType = type;
    initialize();
    initializeProperties();
}

static int copyParm(const NRiName &name, SDKOfxProperty *prop, void *srcTable)
{
    NRiHashTable<SDKOfxProperty*> *srcProps = (NRiHashTable<SDKOfxProperty*> *)srcTable;
    SDKOfxProperty *srcProp = srcProps->getValue(name);
    if (srcProp) 
    {
        *prop = *srcProp;
    }
    return 0;
}

SDKOfxParm::SDKOfxParm(SDKOfxParm *parm)    
{
#ifdef _DEBUG
    printf("SDKOfxParm Instance 0x%X properySet = 0x%X    name = %s\n", (int)this, (int)getPropertySetHandle(), parm->getName().toCStr());
#endif    
    
    setName(parm->getName());
    m_parmType = parm->m_parmType;    
    m_plugCount = parm->m_plugCount;
    m_plugType = parm->m_plugType;
    m_Label    = parm->m_Label;
    m_nameProp = parm->m_nameProp;  
    m_allocatedString = 0;
    int i;
    for (i = 0; i < parm->m_parmSuffixes.getNbItems(); i++)
        m_parmSuffixes.append(parm->m_parmSuffixes[i]);
    
    initializeProperties();        
    
    NRiHashTable<SDKOfxProperty*> *props = getPropertyTable();
    props->enumerate(copyParm, (void *)parm->getPropertyTable());
}

NRiString SDKOfxParm::strEmpty("");
NRiString SDKOfxParm::strX("X");
NRiString SDKOfxParm::strY("Y");
NRiString SDKOfxParm::strZ("Z");
NRiString SDKOfxParm::strR("R");
NRiString SDKOfxParm::strG("G");
NRiString SDKOfxParm::strB("B");
NRiString SDKOfxParm::strA("A");

void SDKOfxParm::initialize()
{
    NRiString label;    
    if( thePluginHost->propGetAsString (getPropertySetHandle(), kOfxPropLabel, 0, label) == kOfxStatOK)
    {
        m_Label = label;
    }
    else
    {
        m_Label = getName();
    }
    
    m_plugCount = 1;
    m_parmSuffixes.append(&strEmpty);
    
    if (m_parmType == kOfxParamTypeDouble)
    {
        m_plugType = kFloat;
    }
    else if (m_parmType == kOfxParamTypeDouble2D)
    {
        m_plugType = kFloat;
        m_plugCount = 2;
        m_parmSuffixes[0] = &strX;
        m_parmSuffixes.append(&strY);
    }
    else if (m_parmType == kOfxParamTypeInteger)
    {
        m_plugType = kInt;
    }
    else if (m_parmType == kOfxParamTypeInteger2D)
    {
        m_plugType = kInt;
        m_plugCount = 2;
        m_parmSuffixes[0] = &strX;
        m_parmSuffixes.append(&strY);
    }
    else if (m_parmType == kOfxParamTypeRGB)
    {
        m_plugType = kFloat;
        m_plugCount = 3;
        m_parmSuffixes[0] = &strR;
        m_parmSuffixes.append(&strG);
        m_parmSuffixes.append(&strB);        
    }
    else if (m_parmType == kOfxParamTypeDouble3D)
    {
        m_plugType = kFloat;
        m_plugCount = 3;
        m_parmSuffixes[0] = &strX;
        m_parmSuffixes.append(&strY);
        m_parmSuffixes.append(&strZ);        
    }
    else if (m_parmType == kOfxParamTypeInteger3D)
    {
        m_plugType = kInt;
        m_plugCount = 3;
        m_parmSuffixes[0] = &strX;
        m_parmSuffixes.append(&strY);
        m_parmSuffixes.append(&strZ);        
    }
    else if (m_parmType == kOfxParamTypeRGBA)
    {
        m_plugType = kFloat;
        m_plugCount = 4;
        m_parmSuffixes[0] = &strR;
        m_parmSuffixes.append(&strG);
        m_parmSuffixes.append(&strB);        
        m_parmSuffixes.append(&strA);
    }
    else if (m_parmType == kOfxParamTypeBoolean)
    {
        m_plugType = kInt;
    }
    else if (m_parmType == kOfxParamTypeString ||
             m_parmType == kOfxParamTypeCustom ||
             m_parmType == kOfxParamTypeChoice )
    {
        m_plugType = kString;            // TODO -- handle kOfxParamPropStringMode to determine how this is presented to the user
    }
    else if (m_parmType == kOfxParamTypeGroup ||
             m_parmType == kOfxParamTypePage)
    {
        m_plugType = kInvalid;
        m_plugCount = 0;        
    }
    else if (m_parmType == kOfxParamTypePushButton)
    {
        m_plugType = kInt;
    }
    
}

void SDKOfxParm::initializeProperties()
{
    // Set up the properties for the parameter
    int i;
    NRiHashTable<SDKOfxProperty*> *properties = getPropertyTable();
    OfxPropertySetHandle parmProps = getPropertySetHandle();
    
    m_changeReasonProp = new SDKOfxProperty(kString, 0);
    *(m_changeReasonProp->getNamePtr(0)) = kOfxChangeUserEdited;
    properties->addEntry(kOfxPropChangeReason, m_changeReasonProp);
    
    SDKOfxProperty *newProperty = new SDKOfxProperty(kString, 0);
    *(newProperty->getNamePtr(0)) = kOfxTypeParameter;
    properties->addEntry(kOfxPropType, newProperty);
    
    newProperty = new SDKOfxProperty(kString, 0);
    *(newProperty->getNamePtr(0)) = m_parmType.toCStr();
    properties->addEntry(kOfxParamPropType, newProperty);
    
    m_nameProp = new SDKOfxProperty(kString, 0);
    *(m_nameProp->getNamePtr(0)) = getName().toCStr();
    properties->addEntry(kOfxPropName, m_nameProp);
    
    newProperty = new SDKOfxProperty(kString);
    *(newProperty->getNamePtr(0)) = getName().toCStr();
    properties->addEntry(kOfxPropLabel, newProperty);
    
    newProperty = new SDKOfxProperty(kString);
    *(newProperty->getNamePtr(0)) = getName().toCStr();
    properties->addEntry(kOfxPropShortLabel, newProperty);
    
    newProperty = new SDKOfxProperty(kString);
    *(newProperty->getNamePtr(0)) = getName().toCStr();
    properties->addEntry(kOfxPropLongLabel, newProperty);
    
    newProperty = new SDKOfxProperty(kPtr, 0);
    newProperty->getValuePtr(0)->p = 0;
    properties->addEntry(kOfxParamPropDataPtr, newProperty);

    newProperty = new SDKOfxProperty(kPtr, 0);
    newProperty->getValuePtr(0)->p = 0;
    properties->addEntry(kOfxParamPropInteractV1, newProperty);

    newProperty = new SDKOfxProperty(kInt);
    newProperty->getValuePtr(0)->i = 0;
    properties->addEntry(kOfxParamPropSecret, newProperty);
    
    newProperty = new SDKOfxProperty(kInt);
    newProperty->getValuePtr(0)->i = 1;                          // Shake ignores this, every parameter must support undo
    properties->addEntry(kOfxParamPropCanUndo, newProperty);
    

    SDKOfxPluginHost::createProp(parmProps, kOfxParamPropInteractSize, kDouble);
    SDKOfxPluginHost::propSetDouble(parmProps, kOfxParamPropInteractSize, 0, 0);
    SDKOfxPluginHost::propSetDouble(parmProps, kOfxParamPropInteractSize, 1, 0);

    SDKOfxPluginHost::createProp(parmProps, kOfxParamPropInteractSizeAspect, kDouble);
    SDKOfxPluginHost::propSetDouble(parmProps, kOfxParamPropInteractSizeAspect, 0, 1);

    SDKOfxPluginHost::createProp(parmProps, kOfxParamPropInteractMinimumSize, kInt);
    SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropInteractMinimumSize, 0, 10);
    SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropInteractMinimumSize, 1, 10);

    SDKOfxPluginHost::createProp(parmProps, kOfxParamPropInteractPreferedSize, kInt);
    SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropInteractPreferedSize, 0, 10);
    SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropInteractPreferedSize, 1, 10);

    newProperty = new SDKOfxProperty(kInt);                                   // If this is 0, pctrl doesn't get the keyframe icon.
    newProperty->getValuePtr(0)->i = 1;    
    properties->addEntry(kOfxParamPropAnimates, newProperty);
    
    int defType;
    
    if (m_parmType == kOfxParamTypeDouble ||
        m_parmType == kOfxParamTypeDouble2D ||
        m_parmType == kOfxParamTypeDouble3D ) 
    {
      SDKOfxPluginHost::createProp(parmProps, kOfxParamPropIncrement, kDouble);
      SDKOfxPluginHost::propSetDouble(parmProps, kOfxParamPropIncrement, 0, 1);
      
      SDKOfxPluginHost::createProp(parmProps, kOfxParamPropDigits, kInt);
      SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropDigits, 0, 2);
      SDKOfxPluginHost::createProp(parmProps, kOfxParamPropShowTimeMarker, kInt);
      SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropShowTimeMarker, 0, 0);
    }

    if (m_parmType == kOfxParamTypeDouble ||
        m_parmType == kOfxParamTypeDouble2D ||
        m_parmType == kOfxParamTypeRGB ||
        m_parmType == kOfxParamTypeDouble3D ||
        m_parmType == kOfxParamTypeRGBA )
    {
        defType = kDouble;

        SDKOfxPluginHost::createProp(parmProps, kOfxParamPropDoubleType, kString);
        SDKOfxPluginHost::propSetString(parmProps, kOfxParamPropDoubleType, 0, kOfxParamDoubleTypePlain);       
    }
    else if (m_parmType == kOfxParamTypeInteger ||
             m_parmType == kOfxParamTypeInteger2D ||
             m_parmType == kOfxParamTypeInteger3D ||
             m_parmType == kOfxParamTypePushButton ||                        
             m_parmType == kOfxParamTypeBoolean)
    {
        SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropAnimates, 0, 0);
        defType = kInt;
    }
    else if (m_parmType == kOfxParamTypeString)
    {
        defType = kString;
        SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropAnimates, 0, 0);

        SDKOfxPluginHost::createProp(parmProps, kOfxParamPropStringMode, kString);
        SDKOfxPluginHost::propSetString(parmProps, kOfxParamPropStringMode, 0, kOfxParamStringIsSingleLine);       
        
        SDKOfxPluginHost::createProp(parmProps, kOfxParamPropStringFilePathExists, kInt);
        SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropStringFilePathExists, 0, 1);
    }
    else if (m_parmType == kOfxParamTypeCustom)
    {
        defType = kString;
        SDKOfxPluginHost::createProp(parmProps, kOfxParamPropCustomInterpCallbackV1, kPtr);  // Unused since we don't support animation of custom parameters
        SDKOfxPluginHost::createProp(parmProps, kOfxParamPropInterpolationAmount, kDouble);     // Unused since we don't support animation of custom parameters   
        
        SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropAnimates, 0, 0);
    }
    else if (m_parmType == kOfxParamTypeChoice)
    {
        defType = kInt;        
        newProperty = new SDKOfxProperty(kString, 1, 0);
        properties->addEntry(kOfxParamPropChoiceOption, newProperty);
        SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropAnimates, 0, 0);
    }
    else if (m_parmType == kOfxParamTypePage)
    {
        defType = kInt;        
        newProperty = new SDKOfxProperty(kString, 1, m_plugCount);               // Ignored for now, we don't support pages
        properties->addEntry(kOfxParamPropPageChild, newProperty);
    }
    
    newProperty = new SDKOfxProperty(defType, 1, m_plugCount);
    properties->addEntry(kOfxParamPropDefault, newProperty);
    
    if (defType != kString)
    {
        SDKOfxProperty *maxProperty = new SDKOfxProperty(defType, 1, m_plugCount);        
        SDKOfxProperty *minDispProperty = new SDKOfxProperty(defType, 1, m_plugCount);        
        SDKOfxProperty *maxDispProperty = new SDKOfxProperty(defType, 1, m_plugCount);        
        newProperty = new SDKOfxProperty(defType, 1, m_plugCount);
        
        for (i = 0; i < m_plugCount; i++)
        {
            if (defType == kDouble) 
            {
                newProperty->getValuePtr(i)->d = -DBL_MAX;
                maxProperty->getValuePtr(i)->d = DBL_MAX;                
                minDispProperty->getValuePtr(i)->d = -DBL_MAX;
                maxDispProperty->getValuePtr(i)->d = DBL_MAX;     
                SDKOfxPluginHost::propSetDouble(parmProps, kOfxParamPropDefault, i, 0.);                
            }
            else if (defType == kInt) 
            {
                newProperty->getValuePtr(i)->i = INT_MIN;    
                maxProperty->getValuePtr(i)->i = INT_MAX;                    
                minDispProperty->getValuePtr(i)->d = INT_MIN;
                maxDispProperty->getValuePtr(i)->d = INT_MAX;         
                SDKOfxPluginHost::propSetInt(parmProps, kOfxParamPropDefault, i, 0);                        
            }
        }
        properties->addEntry(kOfxParamPropMin, newProperty);
        properties->addEntry(kOfxParamPropMax, maxProperty);        
        properties->addEntry(kOfxParamPropDisplayMin, minDispProperty);
        properties->addEntry(kOfxParamPropDisplayMax, maxDispProperty);
    }

    newProperty = new SDKOfxProperty(kString, 1, m_plugCount);               // Ignored for now, we don't support pages
    properties->addEntry(kOfxPluginPropParamPageOrder, newProperty);
    
    newProperty = new SDKOfxProperty(kInt);                                   // TODO - Make sure non-persistant parms don't get serialized.  
    newProperty->getValuePtr(0)->i = 1;    
    properties->addEntry(kOfxParamPropPersistant, newProperty);
    
    newProperty = new SDKOfxProperty(kInt);
    newProperty->getValuePtr(0)->i = 1;    
    properties->addEntry(kOfxParamPropEvaluateOnChange, newProperty);
    
    newProperty = new SDKOfxProperty(kInt);
    properties->addEntry(kOfxParamPropPluginMayWrite, newProperty);
    
    newProperty = new SDKOfxProperty(kString);                         // Shake's cacheId mechanism  will handle this by default, don't need to do anything with this property 
    *(newProperty->getNamePtr(0)) = kOfxParamInvalidateValueChange;
    properties->addEntry(kOfxParamPropCacheInvalidation, newProperty);
    
    newProperty = new SDKOfxProperty(kString);
    properties->addEntry(kOfxParamPropHint, newProperty);
    
    newProperty = new SDKOfxProperty(kString, 1, m_plugCount);
    for (i = 0; i < m_plugCount; i++)
    {
        NRiName pName;
        getPlugName(pName, i);
        *(newProperty->getNamePtr(i)) = pName.getString();
    }
    properties->addEntry(kOfxParamPropScriptName, newProperty);
    
    newProperty = new SDKOfxProperty(kString);
    properties->addEntry(kOfxParamPropParent, newProperty);
    
    m_enabledProp = new SDKOfxProperty(kInt,1,1,1);
    m_enabledProp->getValuePtr(0)->i = 1;
    properties->addEntry(kOfxParamPropEnabled, m_enabledProp);
    
    m_timeProp = new SDKOfxProperty(kDouble, 0);
    properties->addEntry(kOfxPropTime, m_timeProp);
    
    m_renderScaleProp = new SDKOfxProperty(kDouble, 0, 2);
    m_renderScaleProp->getValuePtr(0)->d = 1.;
    m_renderScaleProp->getValuePtr(1)->d = 1.;
    properties->addEntry(kOfxImageEffectPropRenderScale, m_renderScaleProp);
}

SDKOfxParm::~SDKOfxParm()
{
    // note - don't delete m_parmSuffixes contents since they contain refs to static values.
    if (m_allocatedString) delete[] m_allocatedString;
}

void SDKOfxParm::getPlugName(NRiName &plugName, int index)
{
    plugName = NRiLex::idString(getLabel().toCStr());
    if (index >= 0) plugName += NRiName(getSuffix(index).toCStr());
}


NRiString SDKOfxParm::getParentGroupName()
{
    NRiString groupName;    
    thePluginHost->propGetAsString(getPropertySetHandle(), kOfxParamPropParent, 0, groupName);
    
    return groupName;
}


int SDKOfxParm::getNumValues()
{
    int numValsForParm;
    if (getType() == kOfxParamTypeInteger2D ||
        getType() == kOfxParamTypeDouble2D )
    {
        numValsForParm = 2;
    }
    else if (getType() == kOfxParamTypeInteger3D ||
             getType() == kOfxParamTypeDouble3D ||
             getType() == kOfxParamTypeRGB )
    {
        numValsForParm = 3;
    }
    else if (getType() == kOfxParamTypeRGBA)
    {
        numValsForParm = 4;
    }
    else if (getType() == kOfxParamTypeGroup ||
             getType() == kOfxParamTypePage)
    {
        numValsForParm = 0;        
    }
    else 
    {
        numValsForParm = 1;
    }
    
    return numValsForParm;
}


SDKOfxParameterList::SDKOfxParameterList()
{
    
}

SDKOfxParameterList::SDKOfxParameterList(SDKOfxPlugin* owner)
{
    m_owner = owner;
}

SDKOfxParameterList::~SDKOfxParameterList()
{
    m_parameters.deleteAllEntries();
    m_childLists.deleteAllEntries();
}

SDKOfxParm *SDKOfxParameterList::getParamByName(const char *name)
{
    int numParms = m_parameters.getNbItems();
    int i;
    for (i = 0; i < numParms; i++)
    {
        SDKOfxParm *parm = m_parameters[i];
        if (strcmp(name, parm->getName().toCStr()) == 0)
            return parm;
    }
    
    return 0;
}


// Recursively search for a child parameter list.
SDKOfxParameterList* SDKOfxParameterList::getChildListByName(const NRiString& name)
{
    int i;
    SDKOfxParameterList* pTargetList;
    for (i = 0; i < m_childLists.getNbItems(); i++)
    {
        if(m_childLists[i]->getName() == name)
        {
            return m_childLists[i];
        }
        else
        {
            pTargetList = m_childLists[i]->getChildListByName(name);
            if(pTargetList)
            {
                return pTargetList;
            }
        }
    }

    return 0;
}


void SDKOfxParm::getValue(va_list arg)
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        switch (m_plugType)
        {
            case kInt:
            {
                int *v = va_arg(arg, int *);
                *v = m_plugs[i]->asInt();
                break;
            }
            case kFloat:
            {
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                NRiValue* pVal;
                NRiString theType;
                double divisors[3];  // Accommodate params with up to three dimensions.
                divisors[0] = divisors[1] = divisors[2] = 1.0f;
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                
                if(pDoubleType && pOwnerNode)
                {
                    pVal = pDoubleType->getValuePtr(0);
                    theType = pVal->n;
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        divisors[0] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                        divisors[1] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                }

                double* v = va_arg(arg, double *);  // OFX uses only double.
                if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                {
                    // Adjust the values by the divisors we've set up.
                    *v = m_plugs[i]->asDouble() / divisors[i];
                }
                else    // Use plain value.
                {
                    *v = m_plugs[i]->asDouble();
                }
                break;
            }
            case kString:
            {
                const char *value = m_plugs[i]->asString().getString(); 
                if (m_parmType == kOfxParamTypeChoice)
                {
                    int idx;
                    int numChoices;
                    thePluginHost->propGetDimension(getPropertySetHandle(), kOfxParamPropChoiceOption, &numChoices);
                    int *v = va_arg(arg, int *);
                    for (idx = 0; idx < numChoices; idx++)
                    {
                        char *choice;
                        thePluginHost->propGetString(getPropertySetHandle(), kOfxParamPropChoiceOption, idx, &choice);
                        if (strcmp(choice, value) == 0)
                        {
                            *v = idx;
                            break;
                        }
                    }
                }
                else
                {
                    if (m_allocatedString) {
                        delete[] m_allocatedString;
                        m_allocatedString = 0;
                    }
                    char **v = va_arg(arg, char **);
                    *v = strcpy( new char[ strlen( value ) + 1 ], value );
                    m_allocatedString = *v;
                }
                break;
            }
            default:
                break;
        }
    }
}

SDKOfxOverlay *SDKOfxParm::m_OverlayInUse = 0;

void SDKOfxParm::setValue(va_list arg)
{
    int parmOverlayKeying = 0;
    int animates, secret;
    SDKOfxPluginHost::propGetInt(getPropertySetHandle(), kOfxParamPropAnimates, 0, &animates);
    SDKOfxPluginHost::propGetInt(getPropertySetHandle(), kOfxParamPropSecret, 0, &secret);        
    
    NRiString paramType = getType();
    if (m_OverlayInUse && animates && !secret && 
        (paramType == kOfxParamTypeInteger ||
         paramType == kOfxParamTypeDouble ||
         paramType == kOfxParamTypeBoolean ||
         paramType == kOfxParamTypeDouble2D ||
         paramType == kOfxParamTypeDouble3D ||
         paramType == kOfxParamTypeInteger3D))
    {
        parmOverlayKeying = 1;        
    }
    
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        // The plug to set is actually owned by the NRiGrpNode that contains the SDKOfxEffect instance.
        NRiPlug *plug = m_plugs[i];
        NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
        
        int pLocked = grpPlug->getFlag(NRiPlug::kLocked);
        int uLocked = grpPlug->getLogicalUpdater()->getFlag(NRiPlug::kLocked);
        grpPlug->setFlag(NRiPlug::kLocked, 0);
        grpPlug->getLogicalUpdater()->setFlag(NRiPlug::kLocked, 0);

        if (parmOverlayKeying)
        {
            float val = 0.f;
            switch (m_plugType)
            {
                case kInt:
                {
                    val = float(va_arg(arg, int));
                    break;
                }
                case kFloat:
                {
                    // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                    // a broader range.  See if this is one.
                    SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                    NRiValue* pVal;
                    NRiString theType;
                    double factors[3];  // Accommodate params with up to three dimensions.
                    factors[0] = factors[1] = factors[2] = 1.0f;
                    SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                    
                    if(pDoubleType && pOwnerNode)
                    {
                        pVal = pDoubleType->getValuePtr(0);
                        theType = pVal->n;
                        if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                        {
                            factors[0] = factors[1] = factors[2] = pOwnerNode->out->width()->asDouble()/globalXProxyScale->asFloat();
                        }
                        else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                        {
                            factors[0] = factors[1] = factors[2] = pOwnerNode->out->height()->asDouble()/globalYProxyScale->asFloat();
                        }
                        else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                        {
                            factors[0] = pOwnerNode->out->width()->asDouble()/globalXProxyScale->asFloat();
                            factors[1] = pOwnerNode->out->height()->asDouble()/globalYProxyScale->asFloat();
                        }
                    }

                    if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                    {
                        val = float(va_arg(arg, double)) * factors[i];
                    }
                    else    // We have more than three plugs?  Use plain value.
                    {
                        val = float(va_arg(arg, double));
                    }
                    break;
                }
                default:
                    NRiSys::error("%EUnknown type in SDKOfxParm::setValue\n");
                    break;
            }

            m_OverlayInUse->setKey(grpPlug, val);
        }
        else
        {
            switch (m_plugType)
            {
                case kInt:
                {
                    grpPlug->set(va_arg(arg, int));
                    break;
                }
                case kFloat:
                {
                    // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                    // a broader range.  See if this is one.
                    float val = 0.f;
                    SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                    NRiValue* pVal;
                    NRiString theType;
                    double factors[3];  // Accommodate params with up to three dimensions.
                    factors[0] = factors[1] = factors[2] = 1.0f;
                    SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                    
                    if(pDoubleType && pOwnerNode)
                    {
                        pVal = pDoubleType->getValuePtr(0);
                        theType = pVal->n;
                        if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                        {
                            factors[0] = factors[1] = factors[2] = pOwnerNode->out->width()->asDouble()/globalXProxyScale->asFloat();
                        }
                        else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                        {
                            factors[0] = factors[1] = factors[2] = pOwnerNode->out->height()->asDouble()/globalYProxyScale->asFloat();
                        }
                        else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                        {
                            factors[0] = pOwnerNode->out->width()->asDouble()/globalXProxyScale->asFloat();
                            factors[1] = pOwnerNode->out->height()->asDouble()/globalYProxyScale->asFloat();
                        }
                    }
                    
                    if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                    {
                        val = float(va_arg(arg, double)) * factors[i];
                    }
                    else    // We have more than three plugs?  Use plain value.
                    {
                        val = float(va_arg(arg, double));
                    }                    
                    grpPlug->set(val);
                    break;
                }
                case kString:
                {
                    if (m_parmType == kOfxParamTypeChoice)
                    {
                        int idx = va_arg(arg, int);
                        int numChoices;
                        thePluginHost->propGetDimension(getPropertySetHandle(), kOfxParamPropChoiceOption, &numChoices);
                        if (idx < numChoices)
                        {
                            char *choice;
                            thePluginHost->propGetString(getPropertySetHandle(), kOfxParamPropChoiceOption, idx, &choice);
                            grpPlug->set(choice);                            
                        }
                        else
                        {
                            NRiSys::error("%EAttempt to setting paramater %s to choice %d, which exceeds the number of valid choices\n", getLabel().toCStr(), idx);
                        }
                    }
                    else
                    {
                        grpPlug->set(va_arg(arg, char *));
                    }
                    break;
                }
                default:
                    break;
            }
        }

        grpPlug->setFlag(NRiPlug::kLocked, pLocked);
        grpPlug->getLogicalUpdater()->setFlag(NRiPlug::kLocked, uLocked);
    }
}


void SDKOfxParm::setValueAtTime(float time, va_list arg) 
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        // The plug to set is actually owned by the NRiGrpNode that contains the SDKOfxEffect instance
        NRiPlug *plug = m_plugs[i];
        NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
        
        if (grpPlug->getFlag(NRiPlug::kLocked) || grpPlug->getLogicalUpdater()->getFlag(NRiPlug::kLocked)) continue;
        
        if (m_plugType == kString)
        {
            grpPlug->set(va_arg(arg, char *));
        }
        else
        {
            NRiCurve *curve = NRiCurveManager::getCurve(grpPlug->getLogicalUpdater());
            float v;
            if (m_plugType == kInt)
            {
                v = float(va_arg(arg, int));
            }
            else   //  kFloat:
            {
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                NRiValue* pVal;
                NRiString theType;
                double factors[3];  // Accommodate params with up to three dimensions.
                factors[0] = factors[1] = factors[2] = 1.0f;
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                
                if(pDoubleType && pOwnerNode)
                {
                    pVal = pDoubleType->getValuePtr(0);
                    theType = pVal->n;
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        factors[0] = factors[1] = factors[2] = pOwnerNode->out->width()->asDouble()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        factors[0] = factors[1] = factors[2] = pOwnerNode->out->height()->asDouble()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        factors[0] = pOwnerNode->out->width()->asDouble()/globalXProxyScale->asFloat();
                        factors[1] = pOwnerNode->out->height()->asDouble()/globalYProxyScale->asFloat();
                    }
                }
                
                if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                {
                    v = float(va_arg(arg, double)) * factors[i];
                }
                else    // We have more than three plugs?  Use plain value.
                {
                    v = float(va_arg(arg, double));
                }
            }
            
            if (curve->expression() || curve->type() == NRiCurveType::constCurve())
            {
                grpPlug->set(v);
            }
            else
            {
                curve->setCalculating(0);
                uint32_t idx;
                curve->addCV(
                             time, 
                             v, 
                             0, 
                             1, 
                             NRiCurveType::defaultCurve(), 
                             idx, 
                             1
                             );
                curve->setCalculating(1);
            }
        }
    }
}


void SDKOfxParm::getValueAtTime(float time, va_list arg)    
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        switch (m_plugType)
        {
            case kInt:
            {
                int *v = va_arg(arg, int *);
                *v = m_plugs[i]->asInt(time);
                break;
            }
            case kFloat:
            {
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                NRiValue* pVal;
                NRiString theType;
                double divisors[3];  // Accommodate params with up to three dimensions.
                divisors[0] = divisors[1] = divisors[2] = 1.0f;
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                
                if(pDoubleType && pOwnerNode)
                {
                    pVal = pDoubleType->getValuePtr(0);
                    theType = pVal->n;
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        divisors[0] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                        divisors[1] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                }

                double* v = va_arg(arg, double *);  // OFX uses only double.
                if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                {
                    // Adjust the values by the factors we've set up.
                    *v = m_plugs[i]->asDouble(time) / divisors[i];
                }
                else    // Use plain value.
                {
                    *v = m_plugs[i]->asDouble(time);
                }
                break;
            }
            case kString:
            {
                const char *value = m_plugs[i]->asString().getString();
                if (m_parmType == kOfxParamTypeChoice)
                {
                    int idx;
                    int numChoices;
                    thePluginHost->propGetDimension(getPropertySetHandle(), kOfxParamPropChoiceOption, &numChoices);
                    int *v = va_arg(arg, int *);
                    for (idx = 0; idx < numChoices; idx++)
                    {
                        char *choice;
                        thePluginHost->propGetString(getPropertySetHandle(), kOfxParamPropChoiceOption, idx, &choice);
                        if (strcmp(choice, value) == 0)
                        {
                            *v = idx;
                            break;
                        }
                    }
                }
                else
                {
                    if (m_allocatedString) {
                        delete[] m_allocatedString;
                        m_allocatedString = 0;
                    }
                    char **v = va_arg(arg, char **);
                    *v = strcpy( new char[ strlen( value ) + 1 ], value );
                    m_allocatedString = *v;
                }
                break;
            }
            default:
                break;
        }
    }
}

void SDKOfxParm::getDerivativeAtTime(float time, va_list arg)    
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        switch (m_plugType)
        {
            case kInt:
            {
                int *v = va_arg(arg, int *);
                int v1 = m_plugs[i]->asInt(time-.5f);
                int v2 = m_plugs[i]->asInt(time+.5f);                
                *v = v2-v1;
                break;
            }
            case kFloat:
            {
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                NRiValue* pVal;
                NRiString theType;
                double divisors[3];  // Accommodate params with up to three dimensions.
                divisors[0] = divisors[1] = divisors[2] = 1.0f;
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                
                if(pDoubleType && pOwnerNode)
                {
                    pVal = pDoubleType->getValuePtr(0);
                    theType = pVal->n;
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        divisors[0] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                        divisors[1] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                }

                double* v = va_arg(arg, double *);  // OFX uses only double
                double v1 = m_plugs[i]->asDouble(time - .5f);
                double v2 = m_plugs[i]->asDouble(time + .5f);
                if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                {
                    *v = (v2 - v1) / divisors[i];
                }
                else
                {
                    *v = (v2 - v1);
                }

                break;
            }
            default:
                break;
        }
    }
}

void SDKOfxParm::getIntegral(float time1, float time2, va_list arg)    
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        switch (m_plugType)
        {
            case kInt:
            {
                float timeCnt;
                int integral = 0;
                for (timeCnt = time1; timeCnt <= time2; timeCnt += 1.f)
                {
                    integral += m_plugs[i]->asInt(timeCnt);
                }
                int *v = va_arg(arg, int *);
                *v = integral;
                break;
            }
            case kFloat:
            {
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                SDKOfxProperty* pDoubleType = getProperty(kOfxParamPropDoubleType);
                NRiValue* pVal;
                NRiString theType;
                double divisors[3];  // Accommodate params with up to three dimensions.
                divisors[0] = divisors[1] = divisors[2] = 1.0f;
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(m_plugs[0]->getOwner());
                
                if(pDoubleType && pOwnerNode)
                {
                    pVal = pDoubleType->getValuePtr(0);
                    theType = pVal->n;
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        divisors[0] = divisors[1] = divisors[2] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        divisors[0] = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                        divisors[1] = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                }

                float timeCnt;
                double integral = 0;
                for (timeCnt = time1; timeCnt <= time2; timeCnt += 1.f)
                {
                    integral += m_plugs[i]->asDouble(timeCnt);
                }

                double* v = va_arg(arg, double *);  // OFX uses only double
                if((theType != kOfxParamDoubleTypePlain) && (i < 3))
                {
                    *v = integral / divisors[i];
                }
                else
                {
                    *v = integral;
                }

                break;
            }
            default:
                break;
        }
    }
}

void SDKOfxParm::getNumKeys(unsigned int* numberOfKeys) 
{
    if (getPlugType() == kString) 
    {
        *numberOfKeys = 0;
        return;
    }
    
    NRiArray<float> allKeys;        
    getParmKeyList(allKeys);
    
    *numberOfKeys = allKeys.getNbItems();
}

void SDKOfxParm::getKeyTime(unsigned int nthKey, float *time) 
{
    NRiArray<float> allKeys;        
    getParmKeyList(allKeys);
    
    int numberOfKeys = allKeys.getNbItems();
    if (nthKey < (unsigned int)(numberOfKeys)) *time = allKeys[nthKey];
}

OfxStatus SDKOfxParm::getKeyIndex(float time, int direction, int *index) 
{
    NRiArray<float> allKeys;        
    getParmKeyList(allKeys);
    OfxStatus ret=kOfxStatOK;
    
    *index = -1;
    
    int numberOfKeys = allKeys.getNbItems();
    int i;
    
    if (numberOfKeys)
    {
        if (allKeys[0] > time)
        {
            if (direction == 0 || direction < 0) ret = kOfxStatFailed;
            else *index = 0;
        }
    }
    
    for (i = 0; i < numberOfKeys-1; i++)
    {
        if (allKeys[i] == time)
        {
            if (direction == 0)
            {
                *index = i;
            }
            else if (direction > 0)
            {
                *index = 1+i;
            }
            else
            {
                if (i > 0) *index = i-1;
                else ret = kOfxStatFailed;
            }
            break;
        }
        
        if (allKeys[i] < time && allKeys[i+1] > time)
        {
            if (direction == 0)
            {
                ret = kOfxStatFailed;
            }
            else if (direction > 0)
            {
                *index = 1+i;
            }
            else
            {
                *index = i;
            }
            break;
        }
    }
    
    if (ret==kOfxStatOK && *index == -1 && numberOfKeys)
    {
        if (allKeys[numberOfKeys-1] == time)
        {
            if (direction == 0) *index = numberOfKeys-1;
            else if (direction < 0 && numberOfKeys > 1) *index = numberOfKeys-2;
            else ret = kOfxStatFailed;
        }
        if (allKeys[numberOfKeys-1] < time)
        {
            if (direction == 0) ret = kOfxStatFailed;
            else if (direction < 0) *index = numberOfKeys-1;
            else ret = kOfxStatFailed;
        }
    }
    
    return ret;
}

OfxStatus SDKOfxParm::deleteKey(float time) 
{
    int i;
    int oneDeleted = 0;
    for (i = 0; i < m_plugCount; i++)
    {
        NRiPlug *plug = m_plugs[i];
        NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
        
        NRiCurve *curve = NRiCurveManager::getCurve(grpPlug->getLogicalUpdater());
        NRiCV * cv = curve->getCV(time);
        if (cv) 
        {
            curve->removeCV(cv, 1);
            oneDeleted = 1;
        }
    }
    
    return oneDeleted ? kOfxStatErrBadIndex : kOfxStatOK;
}

void SDKOfxParm::deleteAllKeys() 
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        NRiPlug *plug = m_plugs[i];
        NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
        
        switch (m_plugType)
        {
            case kInt:
            {
                int val = grpPlug->asInt();
                grpPlug->set(val);
                break;
            }
            case kFloat:
            {
                float val = grpPlug->asFloat();
                grpPlug->set(val);
                break;
            }
            default:
                break;
        }
    }
}

void SDKOfxParm::copy(SDKOfxParm *from, double timeOffset, OfxRangeD* frameRange) 
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        NRiPlug *plug = m_plugs[i];
        NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
        plug = from->m_plugs[i];
        NRiPlug *grpPlugFrom = plug->getOwner()->getParent()->getPlug(plug->getName());
        
        grpPlug->set(grpPlugFrom->asSource());
        
        if (frameRange)
        {
            double min = frameRange->min, max = frameRange->max;
            NRiCurve *curve = NRiCurveManager::getCurve(grpPlug->getLogicalUpdater());
            int kCount = int(curve->getNbCVs());
            
            int j;
            for (j = kCount-1; j >= 0; j--)
            {
                NRiCV *cv = curve->getNthCV(j);
                double keyTime = cv->key();
                if (keyTime < min || keyTime > max) curve->removeCV(cv, 1);
            }    
        }
        
        NRiCurve *curve = NRiCurveManager::getCurve(grpPlug->getLogicalUpdater());
        curve->shiftCurve(timeOffset);
    }
}

void SDKOfxParm::getParmKeyList(NRiArray<float> &allKeys)
{
    int i;
    for (i = 0; i < m_plugCount; i++)
    {
        NRiPlug *plug = m_plugs[i];
        NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
        
        NRiCurve *curve = NRiCurveManager::getCurve(grpPlug->getLogicalUpdater());
        int kCount = int(curve->getNbCVs());
        
        int j;
        for (j = 0; j < kCount; j++)
        {
            NRiCV *cv = curve->getNthCV(j);
            float keyTime = float(cv->key());
            int nbKeys = allKeys.getNbItems();
            int k;
            int needAdd = 1;
            for (k = 0; k < nbKeys; k++)
            {
                if (keyTime == allKeys[k]) 
                {
                    needAdd = 0;
                    break;
                }
                if (keyTime < allKeys[k])
                {
                    needAdd = 0;                        
                    allKeys.insert(keyTime, k);
                    break;
                }
            }
            if (needAdd) allKeys.append(keyTime);
        }
    }
}

SDKOfxParameterList &SDKOfxParameterList::operator=( const SDKOfxParameterList& parmList )
{
    // TODO: SHOULD WE CLEAR OUT EXISTING PARAMS AND LISTS FIRST?

    int numParms = parmList.m_parameters.getNbItems();
    int i;
    for (i = 0; i < numParms; i++)
    {
        SDKOfxParm *parm = new SDKOfxParm(parmList.m_parameters[i]);
        addParameter(parm);
    }

    const NRiPArray<SDKOfxParameterList>* pOtherLists = parmList.getChildLists();
    int numLists = pOtherLists->getNbItems();
    SDKOfxParameterList* pNewList;
    for (i = 0; i < numLists; i++)
    {
        pNewList = new SDKOfxParameterList;
        *pNewList = *(*pOtherLists)[i];
        addParameterList(pNewList);
    }

    return *this;
}




namespace SDKOfx {
  /// turns the kOfxBitDepthX strings into a shake depth
  int mapFromOfxDepth(const NRiString &s)
  {
    if(s == kOfxBitDepthFloat) return 4;
    if(s == kOfxBitDepthShort) return 2;
    if(s == kOfxBitDepthByte) return 1;
    return 0;
  }

  /// turns the shake bitdepth into a kOfxBitDepth value
  const char * mapToOfxDepth(int d)
  {
    switch(d) {
    case 4 : return kOfxBitDepthFloat;
    case 2 : return kOfxBitDepthShort;
    case 1 : return kOfxBitDepthByte;
    default : return kOfxBitDepthNone;
    }
  }

  /// turns the kOfxImageComponentX strings into a shake 'active' bitmask
  int mapFromOfxComponents(const NRiString &s)
  {
    if(s == kOfxImageComponentRGBA)
      return kRGBA;
    if(s == kOfxImageComponentAlpha)
      return kA;
    return kBlack;
        
  }

  /// turns the shake 'active' bitmask into a kOfxImageComponentX string
  const char *mapToOfxComponents(int d)
  {
    // lose the z bit
    d = d & ~kZ;

    if(d == kA) {
      return kOfxImageComponentAlpha;
    }
    else if(d == kBlack) {
      return kOfxImageComponentNone;
    }
    else {
      return kOfxImageComponentRGBA;
    }
  }

  /// turns our SDK clip's names into OFX clip names
  NRiString mapClipName(const char *rawClipName)
  {
    // make the name nice
    char *actualClipName = strstr(rawClipName, "ofx_");
    if (!actualClipName) actualClipName = (char *)rawClipName;
    else actualClipName += 4;
    return NRiString(actualClipName);
  }

  /// turns our SDK clip's names into OFX clip names
  NRiString mapClipName(const NRiName &n)
  {
    // make the name nice
    return mapClipName(n.getString());
  }
}
