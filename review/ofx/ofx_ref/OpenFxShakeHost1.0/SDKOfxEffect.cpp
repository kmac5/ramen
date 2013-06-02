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
* $Header: /cvsroot/openfxshakehost/OFX/SDKOfxEffect.cpp,v 1.2 2006/09/12 19:51:07 dcandela Exp $
*
*
*/

#include <NRiCrop.h>
#include <NRiDoBox.h>
#include <NRiEndian.h>
#include <NRiEval.h>
#include <NRiGlobals.h>
#include <NRiMath.h>
#include <NRiMenu.h>
#include <NRiPArray.h>
#include <NRiScript.h>
#include <NRiString.h>

#include <ofxCore.h>
#include <ofxProperty.h>
#include <ofxImageEffect.h>

#include <SDKOfxPluginHost.h>
#include <SDKOfxEffect.h>
#include <SDKOfxUtility.h>

//#define RENDER_BPIXEL    

static NRiPlug * globalDefWidth        = NRiGlobals::ref("script.defaultWidth",  kInt);
static NRiPlug * globalDefHeight       = NRiGlobals::ref("script.defaultHeight", kInt);
static NRiPlug * globalDefAspect       = NRiGlobals::ref("script.defaultAspect", kFloat);
static NRiPlug * globalFramesPerSecond = NRiGlobals::ref("script.framesPerSecond", kFloat);
static NRiPlug * globalRunlevel        = NRiGlobals::ref("sys.runLevel", kInt);
static NRiPlug * globalTime	       = NRiGlobals::ref("script.time", kFloat);
static NRiPlug * globalXProxyScale     = NRiGlobals::ref("script.xProxyScale", kFloat);
static NRiPlug * globalYProxyScale     = NRiGlobals::ref("script.yProxyScale", kFloat);
static NRiPlug * globalActionData      = NRiGlobals::ref("sys.actionData",kPtr);            // points to the data used by the action.

const char *SDKOfxEffect::shakeEffectProperty = "shakeEffect";

NRiImplementClassName(SDKOfxEffect, "OfxEffect_", "NRiNadic");


SDKOfxEffect::SDKOfxEffect()
  : _bPixelRender(0)
{
#ifdef _DEBUG
    printf("debug: SDKOfxEffect 0x%X properySet = 0x%X  paramSetHandle = 0x%X\n", (int)getOfxInfo(), (int)getOfxInfo()->getPropertySetHandle(), (int)getParamSetHandle());
#endif    
    m_inNotify = 0;

    OfxPropertySetHandle effectProps = m_ofxInfo.getPropertySetHandle();
    SDKOfxPluginHost::createProp(effectProps,kOfxPropType,kString);
    SDKOfxPluginHost::propSetString(effectProps,kOfxPropType,0,kOfxTypeImageEffectInstance);

    SDKOfxPluginHost::createProp(effectProps,kOfxPropInstanceData,kPtr);
    SDKOfxPluginHost::propSetPointer(effectProps,kOfxPropInstanceData,0,0);
    
    SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectPropProjectExtent,kDouble,1,2,1);
    SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectExtent,0, globalDefWidth);
    SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectExtent,1, globalDefHeight);    
    
    SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectPropProjectSize,kDouble,1,2,1);
    SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectSize,0, globalDefWidth);
    SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectSize,1, globalDefHeight);
    
    SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectPropProjectOffset,kDouble,1,2);
    SDKOfxPluginHost::propSetDouble(effectProps,kOfxImageEffectPropProjectOffset,0,0.0);
    SDKOfxPluginHost::propSetDouble(effectProps,kOfxImageEffectPropProjectOffset,1,0.0);
    
    SDKOfxPluginHost::createProp(effectProps,kOfxPropEffectInstance,kPtr);
    SDKOfxPluginHost::propSetPointer(effectProps,kOfxPropEffectInstance,0,((void *) getOfxInfo()));
   
    SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectPropProjectPixelAspectRatio,kDouble,1,1,1);
    SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectPixelAspectRatio,0,globalDefAspect);
    
    m_duration = addPlug("_duration", kFloat, NRiPlug::kIn, 1);
    m_duration->addDependency(out->timeRange());
    SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectInstancePropEffectDuration,kDouble,1,1,1);
    SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectInstancePropEffectDuration,0, m_duration);

    SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectInstancePropSequentialRender,kInt);
    SDKOfxPluginHost::propSetInt(effectProps,kOfxImageEffectInstancePropSequentialRender,0,0);

    SDKOfxPluginHost::createProp(effectProps,kOfxPropIsInteractive,kInt);
    SDKOfxPluginHost::propSetInt(effectProps,kOfxPropIsInteractive,0,globalRunlevel->asInt());    
    
    effectMode = kInPlace;
    
    m_doneInitializing = addPlug("_doneInitializing", kInt, NRiPlug::kIn, 1);
    m_doneInitializing->set(0);
    m_doneInitializing->setNotify(1);

    for(size_t i = 0; i < kMaxSupportedDepths; ++i) m_supportedDepths[i] = false;

    pOut->active()->addDependency(pIBytes);
    
#ifdef RENDER_BPIXEL    
    pOut->bPixel()->setFlag(NRiPlug::kIgnoreConnect, 1);
    pOut->bPixel()->addDependency(pOut->cacheId());
#endif    
}

static int delParmInfo(SDKOfxEffect::NRxPlugProperty *prop, void *)
{
    delete prop;
    return 0;
}

SDKOfxEffect::~SDKOfxEffect()
{
    m_iBuffers.deleteAllEntries();
    m_iRois.deleteAllEntries();
    m_ctrl->mainEntry(kOfxActionDestroyInstance, (OfxImageEffectHandle)getOfxInfo(), NULL, NULL);
    m_plugParms.enumerate(delParmInfo, (void *)0);    
}

void SDKOfxEffect::setPluginCtrl(SDKOfxPluginCtrl *pluginCtrl)
{
    m_ctrl = pluginCtrl;
    
    char *theContextWeAreGoingToUse;
    SDKOfxPluginHost::propGetString((OfxPropertySetHandle)pluginCtrl->getPropertySetHandle(), kOfxImageEffectPropContext, 0, &theContextWeAreGoingToUse);

    SDKOfxProperty *prop  = new SDKOfxProperty(kString, 0);
    *(prop->getNamePtr(0)) = theContextWeAreGoingToUse;
    m_ofxInfo.addProperty(kOfxImageEffectPropContext, prop);          
   
    //Determine if tiled rendering is supported
    // support tiles?
    int supportsTiles;
    SDKOfxPluginHost::propGetInt((OfxPropertySetHandle)pluginCtrl->getPropertySetHandle(),kOfxImageEffectPropSupportsTiles,0,&supportsTiles);
    effectMode = kInPlace;
    if (supportsTiles == 0) {
      effectMode = kFullFrame;
      setMode(kFullFrame);
    }        
//effectMode = kFullFrame;             // TODO -- remove -- for testing only
//setMode(kFullFrame);
    
    
    definePlugs();
    defineClips();

    
    // make the the various property sets, the values are set _before_ we call the get clip preferences action
    OfxPropertySetHandle clipPrefs = getPluginClipPreferenceProperties();
    SDKOfxPluginHost::createProp(clipPrefs, kOfxImageEffectPropFrameRate, kDouble);
    SDKOfxPluginHost::createProp(clipPrefs, kOfxImagePropPixelAspectRatio, kDouble);
    SDKOfxPluginHost::propSetDouble(clipPrefs,kOfxImagePropPixelAspectRatio,0,globalDefAspect->asDouble());
    SDKOfxPluginHost::createProp(clipPrefs, kOfxImageEffectPropPreMultiplication, kString);
    SDKOfxPluginHost::createProp(clipPrefs, kOfxImageClipPropContinuousSamples, kInt);
    SDKOfxPluginHost::createProp(clipPrefs, kOfxImageEffectFrameVarying, kInt);

    SDKOfxPluginHost::createProp(clipPrefs, "OfxImageClipPropDepth_Output", kString);
    SDKOfxPluginHost::createProp(clipPrefs, "OfxImageClipPropComponents_Output", kString);
    SDKOfxPluginHost::createProp(clipPrefs, "OfxImageClipPropPAR_Output", kString);

    OfxPropertySetHandle roiProps = getPluginRegionsOfInterestProperties();
    SDKOfxPluginHost::createProp(roiProps,kOfxPropTime,kDouble);
    SDKOfxPluginHost::createProp(roiProps,kOfxImageEffectPropRenderScale,kDouble);
    SDKOfxPluginHost::createProp(roiProps,kOfxImageEffectPropRegionOfInterest,kDouble);

    for (size_t i=0; i < pIn.getLength(); ++i) {
      // the name of the clip
      const char *clipName = pIn[i]->getName().getString();
      char *actualClipName = strstr(clipName, "ofx_");
      if (!actualClipName) actualClipName = (char *)clipName;
      else actualClipName += 4;

      NRiString propName = "OfxImageClipPropDepth_";
      propName += actualClipName;
      SDKOfxPluginHost::createProp(clipPrefs, propName.toCStr(), kString);

      propName = "OfxImageClipPropComponents_";
      propName += actualClipName;
      SDKOfxPluginHost::createProp(clipPrefs, propName.toCStr(), kString);

      propName = "OfxImageClipPropPAR_";
      propName += actualClipName;
      SDKOfxPluginHost::createProp(clipPrefs, propName.toCStr(), kDouble);
      SDKOfxPluginHost::propSetDouble(clipPrefs, propName.toCStr(), 0, 1.0f);

      // create a prop for the input ROI
      propName = "OfxImageClipPropRoI_";
      propName += actualClipName;      
      SDKOfxPluginHost::createProp(roiProps, propName.toCStr(),kDouble);
    }

    // initialise the array of supported depths
    OfxPropertySetHandle effectProps = (OfxPropertySetHandle)pluginCtrl->getPropertySetHandle();
    int nSupportedDepths;
    memset(m_supportedDepths, 0, sizeof(m_supportedDepths));
    if(SDKOfxPluginHost::propGetDimension(effectProps, kOfxImageEffectPropSupportedPixelDepths, &nSupportedDepths) == kOfxStatOK) {
      for(int d = 0; d < nSupportedDepths; d++) {
        char *cstr;
        if(SDKOfxPluginHost::propGetString(effectProps, kOfxImageEffectPropSupportedPixelDepths, d, &cstr) == kOfxStatOK) {
          NRiString depthStr = cstr;
          m_supportedDepths[SDKOfx::mapFromOfxDepth(depthStr)] = true;
        }
      }
    }
    
}

// For each of the parameters, define the corresponding plugs in our node
void SDKOfxEffect::definePlugs()
{
    SDKOfxParameterList   *ctrlParmList = m_ctrl->getParameterList();
    SDKOfxParameterList   *parmList = getParameterList();    
    
    *parmList = *ctrlParmList;                                  // copy the parameter list from the plug-in control to the instance
    
    int numParms = parmList->getNbItems();
    int i;
    
    for (i = 0; i < numParms; i++)
    {
        SDKOfxParm *parm = (*parmList)[i];
        
        int j;
        for (j = 0; j < parm->getPlugCount(); j++)
        {
            NRiName plugName;
            parm->getPlugName(plugName, j);
            
            NRiPlug* plug  = addPlug(plugName, parm->getPlugType(), NRiPlug::kIn);
            parm->addPlug(plug);
            plug->setNotify(1);
            plug->setFlag(NRiPlug::kMonitor, 1);
            plug->setFlag(NRiPlug::kPassThrough, 1);
            m_plugs.append(plug);
            NRxPlugProperty *pp = new NRxPlugProperty(parm, j);
            m_plugParms.addEntry(plug->getName(), pp);
            
            int cacheIdDependsOn = 0;
            SDKOfxPluginHost::propGetInt((OfxPropertySetHandle)parm->getPropertySetHandle(), kOfxParamPropEvaluateOnChange, 0, &cacheIdDependsOn);
            
            if (cacheIdDependsOn) pOut->cacheId()->addDependency(plug);
            
            NRiHashTable<SDKOfxProperty*> *propTable = parm->getPropertyTable();
            
            SDKOfxProperty *newProperty = new SDKOfxProperty(kPtr);
            propTable->addEntry(kOfxParamPropDataPtr, newProperty);
            
            // For the following properties that are in both the parameter descriptor as well as the instance, change the r/w permissions as necessary
            newProperty = propTable->getValue(kOfxParamPropAnimates);               if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropCacheInvalidation);      if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropCanUndo);                if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropCustomInterpCallbackV1); if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropCustomValue);            if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropDimensionLabel);         if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropDoubleType);             if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropInteractMinimumSize);    if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropInteractPreferedSize);   if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropInteractSizeAspect);     if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropInteractV1);             if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropPageChild);              if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropParent);                 if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropPersistant);             if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropPluginMayWrite);         if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropScriptName);             if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropStringFilePathExists);   if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxParamPropStringMode);             if (newProperty) newProperty->setReadWritePermission(0);
            newProperty = propTable->getValue(kOfxPluginPropParamPageOrder);        if (newProperty) newProperty->setReadWritePermission(0);
        }
        
        NRiName plugName;
        parm->getPlugName(plugName);
        NRiPlug *enabled = addPlug((plugName + NRiName("_enabled")).getString(), kInt, NRiPlug::kIn, 1);
        enabled->set(-1);
        m_enabledPlugs.append(enabled);        
        m_enabledParms.append(parm);
        enabled->setNotify(1);
        enabled->set(parm->getEnabledProp()->getValuePtr(0)->i);
        parm->getEnabledProp()->setPlug(0, enabled);
    }
}

// For each of the clips, define the corresponding clip in our node
void SDKOfxEffect::defineClips()
{
    NRiPArray<SDKOfxClip> *ctrlClips = m_ctrl->getClips();
    size_t i;
    for (i=0;i<ctrlClips->getLength();i++) {
        SDKOfxClip *ctrlClip = (*ctrlClips)[i];
        NRiString clipName = ctrlClip->getName();
        if (clipName == "ofx_Output") {
            createOutput(clipName.toCStr());
            if (isGenerator()) {
                createInput(clipName.toCStr());
            }
        } else {
            createInput(clipName.toCStr());
        }
        // Copy all the clip properties to the new clip
        SDKOfxClip *newClip = m_ofxInfo.getClipByName(clipName);
        // let the clip class copy all the parameters into the instance
        *newClip = *ctrlClip;
        newClip->createInstanceProperties(clipName);
    }
}

int SDKOfxEffect::setClipProperties(NRiIPlug *ip, int connected, int inputBytes)
{
    // Set the clip instance properties that can change. The host
    // currently doesn't support changing the following properties.
    // kOfxImageEffectPropSupportedComponents - (always RGBA)
    // kOfxImageEffectPropTemporalClipAccess - (always 0)
    // kOfxImageClipPropFieldExtraction - (always kOfxImageFieldBoth)
    // kOfxImageEffectPropComponents - (always RGBA)
    // kOfxImageClipPropUnmappedComponents - (always RGBA)
    // kOfxImageEffectPropPreMultiplication - (always kOfxImageUnPreMultiplied)
    // kOfxImagePropPixelAspectRatio - (linked to script.defaultAspect)
    // kOfxImageEffectPropFrameRate - (always global value)
    // kOfxImageClipPropFieldOrder - (always kOfxImageFieldNone)
    // kOfxImageClipPropContinuousSamples - (always 0)

    int success = 0;
    if (ip) {
        OfxImageClipHandle clipHandle;
        OfxPropertySetHandle clipProps;
        OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(&m_ofxInfo);

        // make the name nice
        NRiString clipName = SDKOfx::mapClipName(ip->getName());
        SDKOfxPluginHost::clipGetHandle(thisHandle,clipName.toCStr(),&clipHandle,&clipProps);

        NRiIRect inputDod;
        inputDod = ip->getDod();
        int bytes = inputBytes;
        if (inputBytes == -1) bytes = ip->getBytes();

        const char *clipDepthStr = SDKOfx::mapToOfxDepth(getClipBitDepth(ip->getName().getString()));

        // allow the input to override the current value to let the getClipPref action to change
        NRiTimeRange tr = ip->getTimeRange();
        SDKOfxPluginHost::propSetInt(clipProps,kOfxImagePropRegionOfDefinition,0,inputDod.X1);
        SDKOfxPluginHost::propSetInt(clipProps,kOfxImagePropRegionOfDefinition,1,inputDod.Y1);
        SDKOfxPluginHost::propSetInt(clipProps,kOfxImagePropRegionOfDefinition,2,inputDod.X2);
        SDKOfxPluginHost::propSetInt(clipProps,kOfxImagePropRegionOfDefinition,3,inputDod.Y2);
        SDKOfxPluginHost::propSetString(clipProps,kOfxImageEffectPropPixelDepth,0, clipDepthStr);
        SDKOfxPluginHost::propSetString(clipProps,kOfxImageClipPropUnmappedPixelDepth,0,clipDepthStr);

        const char *clipComponentsStr = SDKOfx::mapToOfxComponents(getClipComponents(ip->getName().getString()));
        SDKOfxPluginHost::propSetString(clipProps,kOfxImageClipPropUnmappedComponents,0,kOfxImageComponentRGBA);
        SDKOfxPluginHost::propSetString(clipProps,kOfxImageEffectPropComponents,0,clipComponentsStr);

        SDKOfxPluginHost::propSetInt(clipProps,kOfxImageClipPropConnected,0,connected);
        SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropUnmappedFrameRange,0,tr.iPoint);
        SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropUnmappedFrameRange,1,tr.oPoint);
        SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropFrameRange,0,tr.iPoint);
        SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropFrameRange,1,tr.oPoint);
        SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropFrameRate,0,globalFramesPerSecond->asDouble());
        SDKOfxPluginHost::propSetDouble(clipProps,kOfxImageEffectPropUnmappedFrameRate,0,globalFramesPerSecond->asDouble());   
        SDKOfxPluginHost::propSetPlug(clipProps,kOfxImagePropPixelAspectRatio,0,globalDefAspect);    
        SDKOfxPluginHost::propSetPlug(clipProps,  kOfxImagePropPixelAspectRatio,0,globalDefAspect);
        
        success = 1;
    }
    return success;
}

int SDKOfxEffect::setImageProperties(NRiIPlug *ip, int bytes, int active, const NRiIRect &dataRect, const NRiIRect &fullImageRect, void *dataPtr, int stride)
{
  // Set the clip instance properties that can change. The host
  // currently doesn't support changing the following properties.
  // kOfxImageEffectPropComponents - (always RGBA)
  // kOfxImageEffectPropPreMultiplication - (always kOfxImageUnPreMultiplied)
  // kOfxImageEffectPropRenderScale - (always 1.0)
  // kOfxImagePropPixelAspectRatio - (always 1?)
  // kOfxImagePropField  - (always kOfxImageFieldNone)
  if(ip) {
    OfxImageClipHandle clipHandle;
    OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(&m_ofxInfo);
    OfxPropertySetHandle clipProps;
    SDKOfxPluginHost::clipGetHandle(thisHandle,ip->getName().getString(),&clipHandle,&clipProps);
    assert(clipHandle);
    
    return ((SDKOfxClip*)clipHandle)->setImageProperties(bytes, dataRect, fullImageRect, stride, ip->getCacheId().getString(), dataPtr, active);
  }
  return 0;
}


void SDKOfxEffect::addInput(const NRiName &iName)
{
    NRiNadic::addInput(iName);
    
    int nbInputs = pIn.getLength();
    if (nbInputs) 
    {
#ifdef RENDER_BPIXEL    
        NRiIPlug *ip = getImageInput(nbInputs - 1);
        pOut->bPixel()->addDependency(ip->bPixel());
#endif        
    }
}

NRiIPlug *SDKOfxEffect::createInput(const char *name)
{
    int nInputs = pIn.getLength();
    NRiIPlug *createdIPlug = 0;
    addInput(name);
    if (pIn.getNbItems() == (nInputs+1)) {
        createdIPlug = getImageInput(nInputs);
        createdIPlug->setName(name);
        
        NRiName cIdStr = NRiName("_cacheId")+NRiName(name);
        NRiPlug *cacheIdWatch = addPlug(cIdStr, kString, NRiPlug::kIn, 1);
        cacheIdWatch->connect(createdIPlug->cacheId());
        m_cIdWatchers.append(cacheIdWatch);
        m_clipNames.append(name);
        m_rawClipNames.append(SDKOfx::mapClipName(name).toCStr());
        cacheIdWatch->setNotify(1);
        
        SDKOfxClip *newClip = new SDKOfxClip;
        newClip->setName(name);
        // Set the name properties in the clip
        OfxPropertySetHandle clipProps = newClip->getPropertySetHandle();
        SDKOfxPluginHost::propSetString(clipProps,kOfxPropLabel,0,name);
        SDKOfxPluginHost::propSetString(clipProps,kOfxPropShortLabel,0,name);
        SDKOfxPluginHost::propSetString(clipProps,kOfxPropLongLabel,0,name);
        SDKOfxPluginHost::createProp(clipProps, shakeEffectProperty, kPtr);
        SDKOfxPluginHost::propSetPointer(clipProps, shakeEffectProperty, 0, (void *)this);
        
        m_ofxInfo.addClip(newClip);
        newClip->setInputPlug(createdIPlug);
    }
    m_iBuffers.append(new NRiBuffer);
    m_iRois.append(new NRiIRect(NRiIRect::nullIRect));
    return createdIPlug;
}
NRiIPlug *SDKOfxEffect::createOutput(const char *name)
{
    SDKOfxClip *newClip = new SDKOfxClip;
    newClip->setName(name);
    OfxPropertySetHandle clipProps = newClip->getPropertySetHandle();
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropLabel,0,name);
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropShortLabel,0,name);
    SDKOfxPluginHost::propSetString(clipProps,kOfxPropLongLabel,0,name);
    SDKOfxPluginHost::createProp(clipProps, shakeEffectProperty, kPtr);
    SDKOfxPluginHost::propSetPointer(clipProps, shakeEffectProperty, 0, (void *)this);
    
    m_ofxInfo.addClip(newClip);
    pOut->setName(name);
    newClip->setInputPlug(pOut);
    return pOut;
}

int SDKOfxEffect::notify(NRiPlug *p)
{
    m_inNotify++;
    int idx;
    
    if (m_plugs.index(p) > -1)
    {
        int ignoreChange = 0;
        NRxPlugProperty *pp = m_plugParms.getValue(p->getName());
        SDKOfxParm *ofxParameter = pp->m_property;
        assert(ofxParameter);
        
        NRiPlug *grpPlug = getParent() ? getParent()->getPlug(p->getName()) : 0;
        NRiCurve *curve = NRiCurveManager::getCurve(grpPlug->getLogicalUpdater());        
        
        // Set properties defining param animation and keying   
        if (curve) {
            SDKOfxPluginHost::propSetInt((OfxPropertySetHandle)ofxParameter->getPropertySetHandle(), kOfxParamPropIsAnimating, 0, curve->isCurve());
            SDKOfxPluginHost::propSetInt((OfxPropertySetHandle)ofxParameter->getPropertySetHandle(), kOfxParamPropIsAutoKeying, 0, curve->autoKeyFrame());
        }
        if (grpPlug)
        {
            NRiString parmType = ofxParameter->getType();
            
            if (parmType == kOfxParamTypeDouble ||
                parmType == kOfxParamTypeDouble2D ||
                parmType == kOfxParamTypeRGB ||
                parmType == kOfxParamTypeDouble3D ||
                parmType == kOfxParamTypeRGBA )
            {
                
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                double divisor = 1.f;  // Accommodate params with up to three dimensions.
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(p->getOwner());
                
                if(pOwnerNode)
                {
                    SDKOfxProperty* pDoubleType = ofxParameter->getProperty(kOfxParamPropDoubleType);
                    NRiValue* pVal;
                    NRiString theType;
                    
                    if(pDoubleType)
                    {
                        pVal = pDoubleType->getValuePtr(0);
                        theType = pVal->n;
                    }
                            
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        divisor = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        divisor = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        divisor = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                }
                
                double extent;
                SDKOfxPluginHost::propGetDouble((OfxPropertySetHandle)ofxParameter->getPropertySetHandle(), kOfxParamPropMin, pp->m_index, &extent); 
                if ((extent != -DBL_MAX) && ((p->asDouble()/divisor) < extent)) 
                {
                    NRiSys::error("%WValue for %s is out of range, setting it to %g\n", p->getName().getString(), extent*divisor);
                    grpPlug->set(extent*divisor);
                }
                SDKOfxPluginHost::propGetDouble((OfxPropertySetHandle)ofxParameter->getPropertySetHandle(), kOfxParamPropMax, pp->m_index, &extent);
                if ((extent != DBL_MAX) && ((p->asDouble()/divisor) > extent)) 
                {
                    NRiSys::error("%WValue for %s is out of range, setting it to %g\n", p->getName().getString(), extent*divisor);
                    grpPlug->set(extent*divisor);
                }
            }
            else if (parmType == kOfxParamTypeInteger ||
                     parmType == kOfxParamTypeInteger2D ||
                     parmType == kOfxParamTypeInteger3D ||
                     parmType == kOfxParamTypeBoolean)
            {
                
                // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                // a broader range.  See if this is one.
                double divisor = 1.f;  // Accommodate params with up to three dimensions.
                SDKOfxEffect* pOwnerNode = dynamic_cast<SDKOfxEffect*>(p->getOwner());
                
                if(pOwnerNode)
                {
                    SDKOfxProperty* pDoubleType = ofxParameter->getProperty(kOfxParamPropDoubleType);
                    NRiValue* pVal;
                    NRiString theType;
                    
                    if(pDoubleType)
                    {
                        pVal = pDoubleType->getValuePtr(0);
                        theType = pVal->n;
                    }
                    
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        divisor = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        divisor = pOwnerNode->out->height()->asFloat()/globalYProxyScale->asFloat();
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                    {
                        divisor = pOwnerNode->out->width()->asFloat()/globalXProxyScale->asFloat();
                    }
                }
                                
                int extent;
                SDKOfxPluginHost::propGetInt((OfxPropertySetHandle)ofxParameter->getPropertySetHandle(), kOfxParamPropMin, pp->m_index, &extent);
                if ((extent != INT_MIN) && ((p->asInt()/divisor) < extent)) 
                {
                    NRiSys::error("%WValue for %s is out of range, setting it to %d\n", p->getName().getString(), extent*divisor);
                    grpPlug->set(extent*divisor);
                }
                SDKOfxPluginHost::propGetInt((OfxPropertySetHandle)ofxParameter->getPropertySetHandle(), kOfxParamPropMax, pp->m_index, &extent);
                if ((extent != INT_MAX) && ((p->asInt()/divisor) > extent)) 
                {
                    NRiSys::error("%WValue for %s is out of range, setting it to %d\n", p->getName().getString(), extent*divisor);
                    grpPlug->set(extent*divisor);
                }
            }
            else if (parmType == kOfxParamTypePushButton)
            {
                if (grpPlug->asInt())
                {
                    p->setNotify(0);                                               // temporarily disable notify so we don't come here again when we set the plug back to 0
                    grpPlug->set(0);
                    p->setNotify(1);                                               // re-enable notify
                }
                else
                {
                    ignoreChange = 1;
                }
            }
        }
        
        if (m_doneInitializing->asInt() && ignoreChange == 0)
        {
            SDKOfxProperty      *prop = ofxParameter->getRenderScaleProp();
            prop->getValuePtr(0)->d = globalXProxyScale->asDouble();
            prop->getValuePtr(1)->d = globalYProxyScale->asDouble();
            
            prop = ofxParameter->getTimeProp();
            prop->getValuePtr(0)->d = globalTime->asDouble();        
            
            prop = ofxParameter->getChangeReasonProp();
            *(prop->getNamePtr(0)) = m_inNotify > 1 ? kOfxChangePluginEdited : kOfxChangeUserEdited;        // If a recursive call to notify, probably coming from the plugin.
            
            // We don't have any good way to group all parameter changes into one begin/end group.  The only place we which might be considered for this behavior is when
            // parameters are set during deserialization.

            int numSlaves;
            OfxPropertySetHandle effectProps = m_ofxInfo.getPropertySetHandle();
            if(SDKOfxPluginHost::propGetDimension(effectProps, kOfxImageEffectPropClipPreferencesSlaveParam, &numSlaves) == kOfxStatOK) 
            {
                int i;
                for (i = 0; i < numSlaves; i++)
                {
                    char *slave = 0;
                    if (SDKOfxPluginHost::propGetString(effectProps, kOfxImageEffectPropClipPreferencesSlaveParam, i, &slave) == kOfxStatOK)
                    {
                        if (ofxParameter->getName() == NRiString(slave))
                        {
                            doOfxClipPreferences();
                        }
                    }
                }
            }
            
            if (!p->isDefined())  // Don't notify the plugin if we are evaluating an expression - This causes problems for Primatte.
            {
                SDKOfxActionData actionData(SDKOfxActionData::kTweaker);
                SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();
                globalActionData->set(&actionData);
            
                m_ctrl->mainEntry(kOfxActionBeginInstanceChanged, (OfxImageEffectHandle)getOfxInfo(), ofxParameter->getPropertySetHandle(), NULL);
                m_ctrl->mainEntry(kOfxActionInstanceChanged, (OfxImageEffectHandle)getOfxInfo(), ofxParameter->getPropertySetHandle(), NULL);
                m_ctrl->mainEntry(kOfxActionEndInstanceChanged,   (OfxImageEffectHandle)getOfxInfo(), ofxParameter->getPropertySetHandle(), NULL);
                cleanUpFetchedImages();
                globalActionData->set(prevActionData);
            }
        }
    }
    else if ((idx = m_enabledPlugs.index(p)) > -1)
    {
        // Someone enabled or disabled a parameter.
        // Find the parameter assiciated with the enabled plug that was just set.
        SDKOfxParm *ofxParameter = m_enabledParms[idx];
        
        // loop though this parameter's plugs to set the locked state
        int j;
        for (j = 0; j < ofxParameter->getPlugCount(); j++)
        {
            NRiName plugName;
            ofxParameter->getPlugName(plugName, j);
            NRiPlug* plug  = getPlug(plugName);
            plug->getLogicalUpdater()->setFlag(NRiPlug::kLocked, !p->asInt());
        }
            
        forceTwkRebuild();
    }
    else if ((idx = m_cIdWatchers.index(p)) > -1)
    {
        if (m_doneInitializing->asInt())
        {
            NRiName inputName = m_clipNames[idx];
            NRiString nameString(inputName.getString());
            SDKOfxClip *clip = m_ofxInfo.getClipByName(nameString);
            assert(clip);
            
            OfxPropertySetHandle clipProps = clip->getPropertySetHandle();
            SDKOfxPluginHost::propSetString(clipProps, kOfxPropChangeReason,0, kOfxChangeUserEdited);
            SDKOfxPluginHost::propSetDouble(clipProps, kOfxPropTime, 0, globalTime->asDouble());
            SDKOfxPluginHost::propSetDouble(clipProps, kOfxImageEffectPropRenderScale, 0, globalXProxyScale->asDouble());
            SDKOfxPluginHost::propSetDouble(clipProps, kOfxImageEffectPropRenderScale, 1, globalYProxyScale->asDouble());
            
            SDKOfxActionData actionData(SDKOfxActionData::kTweaker);
            SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();        
            globalActionData->set(&actionData);
            m_ctrl->mainEntry(kOfxActionBeginInstanceChanged, (OfxImageEffectHandle)getOfxInfo(), clipProps, NULL);
            m_ctrl->mainEntry(kOfxActionInstanceChanged,      (OfxImageEffectHandle)getOfxInfo(), clipProps, NULL);
            m_ctrl->mainEntry(kOfxActionEndInstanceChanged,   (OfxImageEffectHandle)getOfxInfo(), clipProps, NULL);
            cleanUpFetchedImages();
            
            globalActionData->set(prevActionData);
        }
    }
    else if (p == m_doneInitializing)
    {
        if ((m_inNotify == 1) && (p->asInt() == 1))
        {
/*          This section is causing recursive evaluations during deserialization when input to an OFX node is a file sequence.  I haven't found a case where this is needed.
            m_doneInitializing->set(0);
            // Force all parameters to evaluate themselves once to avoid notifies when parameters haven't actually changed, they've just been computed the first time.
            SDKOfxParameterList   *parmList = getParameterList();    
            int numParms = parmList->getNbItems();
            int i;
            for (i = 0; i < numParms; i++)
            {
                SDKOfxParm *parm = (*parmList)[i];
                NRiString parmType = parm->getType();
                if (parmType == kOfxParamTypeGroup ||
                    parmType == kOfxParamTypePage || 
                    parmType == kOfxParamTypePushButton)
                {
                    continue;
                }
                
                NRiPArray<NRiPlug> *plugs = parm->getPlugList();
                int j;
                for (j = 0; j < plugs->getNbItems(); j++)
                {
                    NRiPlug *plug  = (*plugs)[j];
                    plug->update();
                }
            }
            m_doneInitializing->set(1);            
*/            
           
            // Tell the plugin that the instance has been created
            OfxStatus err;
            if ((err = m_ctrl->mainEntry(kOfxActionCreateInstance, (OfxImageEffectHandle)getOfxInfo(), NULL, NULL)) != kOfxStatOK && err != kOfxStatReplyDefault)
            {
                NRiSys::error("%EkOfxActionCreateInstance failed : %d\n", err);
            }
        }
    }

    m_inNotify--;
    return NRiNadic::notify(p);
}


// This is where the effect node gets housed within the NRiGrpNode.  This is where we need to internalize any secret parameters
void SDKOfxEffect::setParent(NRiNode *node)
{
    NRiNadic::setParent(node);

    SDKOfxParameterList   *parmList = getParameterList();    
    int numParms = parmList->getNbItems();
    int i;
    for (i = 0; i < numParms; i++)
    {
        SDKOfxParm *parm = (*parmList)[i];
        
        int j;
        for (j = 0; j < parm->getPlugCount(); j++)
        {
            NRiName plugName;
            parm->getPlugName(plugName, j);
            NRiPlug* plug  = getPlug(plugName);
            assert(plug);
            NRiPlug *grpPlug = getParent()->getPlug(plug->getName());
            
            if (grpPlug) 
            {
                grpPlug->setFlag(NRiPlug::kLocked, plug->getFlag(NRiPlug::kLocked));
                plug->connect(grpPlug);
            }

            if (plug->getName() == SDKOfxPluginHost::hostGeneratorWidthName) {
                pOut->width()->connect(grpPlug);
                OfxPropertySetHandle effectProps = m_ofxInfo.getPropertySetHandle();
                SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectExtent,0,grpPlug);
                SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectSize,0,grpPlug);
            }
            if (plug->getName() == SDKOfxPluginHost::hostGeneratorHeightName) {
                pOut->height()->connect(grpPlug);
                OfxPropertySetHandle effectProps = m_ofxInfo.getPropertySetHandle();
                SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectExtent,1,grpPlug);
                SDKOfxPluginHost::propSetPlug(effectProps,kOfxImageEffectPropProjectSize,1,grpPlug);
            }
            if (plug->getName() == SDKOfxPluginHost::hostGeneratorBytesName) {
                pIn[0]->bytes()->disconnect();
                pIn[0]->bytes()->connect(grpPlug);
                pIBytes->addDependency(grpPlug);
            }
        }
    }
}


int SDKOfxEffect::serialize(NRiFile& file, int mask)
{
    SDKOfxParameterList* pParams = getParameterList();
    SDKOfxProperty* pProperty;
    NRiPArray<NRiPlug>* pParamPlugs;

    // Go through all parameters and hide the plugs of any that aren't persistent.
    int i, x;
    for(i = 0; i < pParams->getNbItems(); i++)
    {
        pProperty = (*pParams)[i]->getProperty(kOfxParamPropPersistant);
        if(pProperty)
        {
            if(pProperty->getValuePtr(0)->i != 1)
            {
                // Not persistent, so go through all the param's plugs and make them internal.
                pParamPlugs = (*pParams)[i]->getPlugList();
                for(x = 0; x < pParamPlugs->getNbItems(); x++)
                {
                    NRiPlug::setFlag(NRiPlug::kInternal, 1, 0, (*pParamPlugs)[x], 0);
                }
            }
        }
    }

    int retVal = NRiNode::serialize(file, mask);

    // Reverse the plug-hiding process.
    for(i = 0; i < pParams->getNbItems(); i++)
    {
        // Don't see any need to check persistence; just go through all the param's plugs and re-expose them.
        pParamPlugs = (*pParams)[i]->getPlugList();
        for(x = 0; x < pParamPlugs->getNbItems(); x++)
        {
            NRiPlug::setFlag(NRiPlug::kInternal, 0, 0, (*pParamPlugs)[x], 0);
        }
    }

    return (retVal);
}


/// map the depth to the nearest supported depth that doesn't lose precision
/// 0 implies there is no mapping, and therefore no output possible
int
SDKOfxEffect::mapToSupportedDepth(int depthToUse)
{  
  // does the effect support that depth directly
  if(depthToUse && !m_supportedDepths[depthToUse]) {
    size_t d;
    // not directly supported, in which case find a deeper one to use
    for(d = depthToUse + 1; d < kMaxSupportedDepths; d++) if(m_supportedDepths[d]) break;

    // if not there, get a shallower one
    if(d == kMaxSupportedDepths) {
      for(d = depthToUse - 1; d >= 0; d--) if(m_supportedDepths[d]) break;
    }
    depthToUse = d;
  }

  return depthToUse;
  
}

/// run the OFX clip preferences command
bool SDKOfxEffect::doOfxClipPreferences()
{
  OfxPropertySetHandle clipPrefProps = getPluginClipPreferenceProperties();
  size_t i;

  // find the deepest input we have on the input plugs
  int depthToUse = 0;
  for (i=0; i<pIn.getLength(); ++i) { 
    NRiIPlug *ip = getImageInput(i);
    NRiIPlug *grpNodeIp = ip?(NRiIPlug *)ip->getLogicalInput():0;
    NRiIPlug *externalIp = grpNodeIp?(NRiIPlug *)grpNodeIp->getLogicalInput():0;
    int thisActive = ip  ? (ip->getActive() & ~kZ):0;
    int connected = (externalIp!=0) && thisActive != kBlack;
    int generator = isGenerator();
    if(connected || generator) {
      int depth = ip->getBytes();    
      if(depth > depthToUse)
        depthToUse = depth;
    }
  }

  // turn that into a depth the effect does support
  depthToUse = mapToSupportedDepth(depthToUse);

  // was it bad
  if(depthToUse == 0)
    return false;

  int active = 0;
  // now set up the clip pref properties
  for (i=0; i < pIn.getLength(); ++i) {
    // Get the max returned clip depth
    //First make sure the clip properties are up to date
    NRiIPlug *ip = getImageInput(i);
    NRiIPlug *grpNodeIp = ip?(NRiIPlug *)ip->getLogicalInput():0;
    NRiIPlug *externalIp = grpNodeIp?(NRiIPlug *)grpNodeIp->getLogicalInput():0;
    int thisActive = ip ? (ip->getActive() & ~kZ) : 0;
    int connected = (externalIp!=0) && thisActive != kBlack;

    SDKOfxClip *clip = m_ofxInfo.getClipByName(m_clipNames[i].getString());
          
    // set the bit depth
    NRiString propName = "OfxImageClipPropDepth_";
    propName += m_rawClipNames[i].getString();
    if (connected) {      
      SDKOfxPluginHost::propSetString(clipPrefProps, propName.toCStr(), 0, SDKOfx::mapToOfxDepth(depthToUse));
    }
    else {
      SDKOfxPluginHost::propSetString(clipPrefProps, propName.toCStr(), 0, kOfxBitDepthNone);
    }

    // set the component on this clip at the moment
    propName = "OfxImageClipPropComponents_" ;
    propName +=  m_rawClipNames[i].getString();
    if (connected && thisActive) {      
      active = thisActive > active ? thisActive : active; // find the most components
      SDKOfxPluginHost::propSetString(clipPrefProps, propName.toCStr(), 0, SDKOfx::mapToOfxComponents(clip->mapToSupportedActive(thisActive)));
    }
    else {
      SDKOfxPluginHost::propSetString(clipPrefProps, propName.toCStr(), 0, kOfxImageComponentNone);
    }

    // set the PAR, always what it is
    propName = "OfxImageClipPropPAR_" ;
    propName +=  m_rawClipNames[i].getString();
    SDKOfxPluginHost::propSetDouble(clipPrefProps, propName.toCStr(), 0, globalDefAspect->asDouble());  // BJN, where to get this?

    // and reset the corresponding clip's properties
    setClipProperties(ip, connected);
  }

  // do the same for the output
  if (isGenerator()) {
      // If inside a generator,  set the initial val to be RGBA and let the clip prefs modify that
      active = kRGBA;
  }
  
  SDKOfxPluginHost::propSetString(clipPrefProps, "OfxImageClipPropDepth_Output", 0, SDKOfx::mapToOfxDepth(depthToUse));

  
  SDKOfxClip *clip = m_ofxInfo.getClipByName("ofx_Output");
  SDKOfxPluginHost::propSetString(clipPrefProps, "OfxImageClipPropComponents_Output", 0, SDKOfx::mapToOfxComponents(clip->mapToSupportedActive(active)));
  SDKOfxPluginHost::propSetDouble(clipPrefProps, "OfxImageClipPropPAR_Output", 0, globalDefAspect->asDouble());  

  // set the other clip preference bits
  SDKOfxPluginHost::propSetDouble(clipPrefProps,  kOfxImageEffectPropFrameRate, 0, globalFramesPerSecond->asDouble());
  SDKOfxPluginHost::propSetDouble(clipPrefProps, kOfxImagePropPixelAspectRatio, 0, globalDefAspect->asDouble());
  SDKOfxPluginHost::propSetString(clipPrefProps, kOfxImageEffectPropPreMultiplication, 0, kOfxImageUnPreMultiplied);
  SDKOfxPluginHost::propSetInt(clipPrefProps,    kOfxImageClipPropContinuousSamples, 0, 0);
  SDKOfxPluginHost::propSetInt(clipPrefProps,    kOfxImageEffectFrameVarying, 0, 0);

  // now call it
  OfxStatus stat =  m_ctrl->mainEntry(kOfxImageEffectActionGetClipPreferences,(OfxImageEffectHandle)getOfxInfo(), 0, clipPrefProps);
  
  // reset the bits on the clip now that we have called the effect's clip preferences
  for (i=0; i < pIn.getLength(); ++i) {
    // Get the max returned clip depth
    //First make sure the clip properties are up to date
    NRiIPlug *ip = getImageInput(i);
    NRiIPlug *grpNodeIp = ip?(NRiIPlug *)ip->getLogicalInput():0;
    NRiIPlug *externalIp = grpNodeIp?(NRiIPlug *)grpNodeIp->getLogicalInput():0;
    int thisActive = ip ? (ip->getActive() & ~kZ) : 0;
    int connected = (externalIp!=0) && thisActive != kBlack;
    // and reset the corresponding clip's properties
    // should just set the clip's components/depth, make a call?
    setClipProperties(ip, connected);
  }


  return (stat == kOfxStatOK) || (stat == kOfxStatReplyDefault);
}

/// evaluate the RoI of the effect on the given rect
void
SDKOfxEffect::doOfxGetRegionsOfInterest(const NRiIRect &roi)
{
  double xs, ys;
  SDKOfx::getCoordinateScaleFactors(xs, ys);

  OfxPropertySetHandle roiProps = getPluginRegionsOfInterestProperties();
  double rTime = pOut->time()->asDouble();
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxPropTime,0,rTime);
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxImageEffectPropRenderScale,0,globalXProxyScale->asDouble());
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxImageEffectPropRenderScale,1,globalYProxyScale->asDouble());
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxImageEffectPropRegionOfInterest,0,roi.X1/xs);
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxImageEffectPropRegionOfInterest,1,roi.Y1/ys);
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxImageEffectPropRegionOfInterest,2,roi.X2/xs);
  SDKOfxPluginHost::propSetDouble(roiProps,kOfxImageEffectPropRegionOfInterest,3,roi.Y2/ys);

  // set up out clips
  for (size_t i=0; i<pIn.getLength(); ++i) {
    NRiIPlug *ip = getImageInput(i);
    NRiIPlug *grpNodeIp = ip?(NRiIPlug *)ip->getLogicalInput():0;
    NRiIPlug *externalIp = grpNodeIp?(NRiIPlug *)grpNodeIp->getLogicalInput():0;
    int connected = (externalIp!=0);
    if (connected) {
      // the name of me clip
        NRiString roiPropName = "OfxImageClipPropRoI_";
        roiPropName += m_rawClipNames[i].getString();

      SDKOfxPluginHost::propSetDouble(roiProps,roiPropName.toCStr(),0,roi.X1/xs);
      SDKOfxPluginHost::propSetDouble(roiProps,roiPropName.toCStr(),1,roi.Y1/ys);
      SDKOfxPluginHost::propSetDouble(roiProps,roiPropName.toCStr(),2,roi.X2/xs);
      SDKOfxPluginHost::propSetDouble(roiProps,roiPropName.toCStr(),3,roi.Y2/ys);
    }
  }

  // call the ofx action
  OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(&m_ofxInfo);
  OfxStatus err = m_ctrl->mainEntry(kOfxImageEffectActionGetRegionsOfInterest, thisHandle, roiProps, roiProps);
  
}

/// get the roi the effect requested on the named clip
NRiIRect SDKOfxEffect::getClipRoI(const char *name)
{
  NRiIRect rect = NRiIRect::nullIRect;
  // get it from the prop set
  OfxPropertySetHandle roiProps = getPluginRegionsOfInterestProperties();
  if(roiProps) { 
    // make up the name of the clip depth in the property set
    NRiString clipName = SDKOfx::mapClipName(name);
    NRiString propName = "OfxImageClipPropRoI_";
    propName += clipName;
    
    double xS, yS;
    SDKOfx::getCoordinateScaleFactors(xS, yS);
    double returnVal = 0;
    SDKOfxPluginHost::propGetDouble(roiProps,propName.toCStr(),0,&returnVal);
    rect.X1 = (int)(returnVal * xS);
    SDKOfxPluginHost::propGetDouble(roiProps,propName.toCStr(),1,&returnVal);
    rect.Y1 = (int)(returnVal * yS);
    SDKOfxPluginHost::propGetDouble(roiProps,propName.toCStr(),2,&returnVal);
    rect.X2 = (int)(returnVal * xS);
    SDKOfxPluginHost::propGetDouble(roiProps,propName.toCStr(),3,&returnVal);
    rect.Y2 = (int)(returnVal * yS);
  }
  return rect;
}

/// get the roi the effect requested on the ith clip
NRiIRect SDKOfxEffect::getClipRoI(int i)
{
  return getClipRoI(m_rawClipNames[i].getString());
}

void SDKOfxEffect::internalizeSecretStuff(NRiPArray<NRiPlug> &internalPlugs)
{
    SDKOfxParameterList   *parmList = getParameterList();    
    int numParms = parmList->getNbItems();
    int i;
    for (i = 0; i < numParms; i++)
    {
        SDKOfxParm *parm = (*parmList)[i];
        
        int j;
        for (j = 0; j < parm->getPlugCount(); j++)
        {
            NRiName plugName;
            parm->getPlugName(plugName, j);            
            NRiPlug* plug  = getPlug(plugName);
            assert(plug);
            NRiPlug *grpPlug = getParent()->getPlug(plug->getName());
            
            int secret = 0;
            SDKOfxPluginHost::propGetInt((OfxPropertySetHandle)parm->getPropertySetHandle(), kOfxParamPropSecret, 0, &secret);
            if (secret == 0) secret = (parm->getType() == kOfxParamTypeCustom);
            if (secret == 1) 
            {
                plug->setFlag(NRiPlug::kInternal, 1);
                internalPlugs.append(plug);
                if (grpPlug) 
                {
                    grpPlug->setFlag(NRiPlug::kInternal, 1);
                    internalPlugs.append(grpPlug);                
                }
            }
        }
    }
}

void SDKOfxEffect::externalizeSecretStuff(NRiPArray<NRiPlug> &internalPlugs)
{
    int numPlugs = internalPlugs.getNbItems();
    int i;
    for (i = 0; i < numPlugs; i++)
    {
        NRiPlug *plug = internalPlugs[i];
        plug->setFlag(NRiPlug::kInternal, 0);
    }
}

int SDKOfxEffect::eval(NRiPlug *p)
{
#ifdef RENDER_BPIXEL    
    if (p == pOut->bPixel())
    {
        pOut->getCacheId();
        
        _bPixelRender = 1;

        NRiPArray<NRiBPixel> bPixels;
       
        int supporteBytes = mapToSupportedDepth(4);
        
        int i;
        for (i=0; i<pIn.getNbItems(); ++i) 
        {
            NRiIPlug *ip = getImageInput(i);
            NRiBPixel *inBPix = new NRiBPixel;
            bPixels.append(inBPix);
            ip->getBPixel(*inBPix);
            
            // Convert the input buffers to the appropriate bit depth.
            // This will pretty much trash the NRiBPixel struct but that's ok since the render treats it as a packed buffer
            // We convert it back to float at the end to resture the struct.
            if (supporteBytes == 2)
            {
                uint16_t *buf = (uint16_t *)&(inBPix->a);
                buf[0] = NRiF2Sn(inBPix->a);
                buf[1] = NRiF2Sn(inBPix->b);
                buf[2] = NRiF2Sn(inBPix->g);
                buf[3] = NRiF2Sn(inBPix->r);                
            }
            else if (supporteBytes == 1)
            {
                uint8_t *buf = (uint8_t *)&(inBPix->a);            
                buf[0] = NRiF2Bn(inBPix->a);
                buf[1] = NRiF2Bn(inBPix->b);
                buf[2] = NRiF2Bn(inBPix->g);
                buf[3] = NRiF2Bn(inBPix->r);                
            }
            
            iBufs[i]->cPtr = (void*)&(inBPix->a);
            iBufs[i]->zPtr = &(inBPix->z);
            iBufs[i]->cStride = 0;
            iBufs[i]->zStride = 0;
            iBufs[i]->line = 0;
            iBufs[i]->nLine = 1;
        }            
        
        fillOutputBuffer();
        
        _bPixelRender = 0;

        // Restore the bpixel to float
        if (supporteBytes == 2)
        {
            uint16_t *buf = (uint16_t *)&(m_bPixel.a);
            uint16_t a = buf[0];
            uint16_t b = buf[1];
            uint16_t g = buf[2];
            uint16_t r = buf[3];
                
            m_bPixel.a = NRiS2F(a);
            m_bPixel.b = NRiS2F(b);
            m_bPixel.g = NRiS2F(g);
            m_bPixel.r = NRiS2F(r);
        }
        else if (supporteBytes == 1)
        {
            uint8_t *buf = (uint8_t *)&(m_bPixel.a);            
            uint8_t a = buf[0];
            uint8_t b = buf[1];
            uint8_t g = buf[2];
            uint8_t r = buf[3];
            
            m_bPixel.a = float(a)/255.f;
            m_bPixel.b = float(b)/255.f;
            m_bPixel.g = float(g)/255.f;
            m_bPixel.r = float(r)/255.f;
        }
        
        m_bPixel.quantize(pOut->getBytes());        
        bPixels.deleteAllEntries();        
	p->set(&m_bPixel);
    }
    else
#endif 
    
    if (p == pOut->cacheId()) {
	NRiName cId;	
	
	cId = cId.sprintf("%s(", className().getString());

        int i;
        int nbInputs = pIn.getNbItems();
        for (i = 0; i < nbInputs; i++)
        {
            if (i != 0) cId += NRiName(",");                                        
	    cId += pIn[i]->getCacheId();
        }
        
        SDKOfxParameterList   *parmList = getParameterList();    
        
        int numParms = parmList->getNbItems();
        for (i = 0; i < numParms; i++)
        {
            SDKOfxParm *parm = (*parmList)[i];
            NRiString parmType = parm->getType();
            if (parmType == kOfxParamTypeGroup ||
                parmType == kOfxParamTypePage || 
                parmType == kOfxParamTypePushButton)
            {
                continue;
            }
            
            NRiPArray<NRiPlug> *plugs = parm->getPlugList();
            int j;
            for (j = 0; j < plugs->getNbItems(); j++)
            {
                cId += NRiName(",");                
                NRiPlug *plug  = (*plugs)[j];
                int saveNotify = plug->getFlag(NRiPlug::kNotify);
                plug->setNotify(0);
                switch (parm->getPlugType())
                {
                    case kInt:
                    {
                        cId += NRiName(plug->asInt());
                        break;
                    }
                    case kFloat:
                    {
                        cId += NRiName(plug->asFloat());
                        break;
                    }
                    case kString:
                    {
                        cId += plug->asString();
                        break;
                    }
                    default:
                        break;
                }
                plug->setNotify(saveNotify);
            }
        }
        cId += NRiName(")");
        OfxPropertySetHandle effectProps = m_ofxInfo.getPropertySetHandle();
        SDKOfxPluginHost::createProp(effectProps,"cacheID",kString);
        SDKOfxPluginHost::propSetString(effectProps,"cacheID",0,cId.getString());
	cId = cId.compress();
	p->set(cId);    
    } else if (p == m_duration) {
        NRiTimeRange tr = out->getTimeRange();
        if (tr.isInfinite() || 
            tr.isIInfinite() || 
            tr.isOInfinite()) 
        {
            m_duration->set(tr.largeFrameNumber);
        }
        else 
        {
            m_duration->set(tr.oPoint - tr.iPoint);
        }
    } else if (!p->isValid()) {
        if (p == pOut->oBuf()) 
        {
          NRiIBuf * oBuf = pOut->getIBuf();
          int mask = pOut->getMask();
          if (oBuf && mask) 
            {
              if (fillInputs()) 
                {
                  oBuf = 0;
                } else 
                {
                  fillOutputBuffer();
                }

              // deallocate the input buffers, no longer needed
              size_t i,n = pIn.getLength();
              for (i=0; i<n; ++i) 
                m_iBuffers[i]->deallocate();
            }
          p->set(oBuf);
          return NRiMonadic::eval(p);   // don't let NRiNadic handle this since we are dealing with it here 
        }
        // TODO update the output depth or components based on the inputs
	else if (p == pOut->dod()) {
	    pOut->getCacheId();
	    oDod = NRiIRect::nullIRect;
            double rTime = pOut->time()->asDouble();
            double xs, ys;
            SDKOfx::getCoordinateScaleFactors(xs, ys);
            
            // if the effect is a generator provide some default values for width and height
            if (isGenerator()) {
                oDod.X2 = globalDefWidth->asInt();
                oDod.Y2 = globalDefHeight->asInt();
            }
            
            // Set the Dod properties for the input clips
            for (size_t i=0; i<pIn.getLength(); ++i) {
                NRiIRect inputDod;
                inputDod = pIn[i]->getDod();
                oDod = oDod.unite(pIn[i]->getDod());
            }
                
            
            // use the default NAdic method to set the DOD to pass into the DOD action
            if (effectMode == kFullFrame) {
                NRiIRect oWindow;
                oWindow.X1 = 0;
                oWindow.Y1 = 0;
                oWindow.X2 = pOut->getWidth();
                oWindow.Y2 = pOut->getHeight();
                
                oDod = oDod.intersect(oWindow);
            }
            if (effectMode == kSuperFullFrame) {
                NRiIRect oWindow;
                oWindow.X1 = 0;
                oWindow.Y1 = 0;
                oWindow.X2 = pMaxWidth->asInt();
                oWindow.Y2 = pMaxHeight->asInt();
                
                oDod = oDod.intersect(oWindow);
            }                
            OfxPropertySetHandle effectProps = m_ofxInfo.getPropertySetHandle();
            SDKOfxPluginHost::createProp(effectProps,kOfxImageEffectPropRegionOfDefinition,kDouble);
            SDKOfxPluginHost::propSetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,0,oDod.X1/xs);
            SDKOfxPluginHost::propSetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,1,oDod.Y1/ys);
            SDKOfxPluginHost::propSetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,2,oDod.X2/xs);
            SDKOfxPluginHost::propSetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,3,oDod.Y2/ys);
            
            NRiHashTable<SDKOfxProperty*> renderPropertiesHash;
            OfxPropertySetHandle renderProperties = (OfxPropertySetHandle)(&renderPropertiesHash);
            SDKOfxPluginHost::createProp(renderProperties,kOfxPropTime,kDouble);
            SDKOfxPluginHost::propSetDouble(renderProperties,kOfxPropTime,0,rTime);
            SDKOfxPluginHost::createProp(renderProperties,kOfxImageEffectPropRenderScale,kDouble);
            SDKOfxPluginHost::propSetDouble(renderProperties,kOfxImageEffectPropRenderScale,0,globalXProxyScale->asDouble());
            SDKOfxPluginHost::propSetDouble(renderProperties,kOfxImageEffectPropRenderScale,1,globalYProxyScale->asDouble());
            OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(&m_ofxInfo);
            OfxStatus err = m_ctrl->mainEntry(kOfxImageEffectActionGetRegionOfDefinition, thisHandle, renderProperties, effectProps);
            if (err == kOfxStatOK) {
                // Plugin has modified the DOD,  set to the new value
                double returnVal;
                err = SDKOfxPluginHost::propGetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,0,&returnVal);
                oDod.X1 = (int)(returnVal*xs);
                err = SDKOfxPluginHost::propGetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,1,&returnVal);
                oDod.Y1 = (int)(returnVal*ys);
                err = SDKOfxPluginHost::propGetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,2,&returnVal);
                oDod.X2 = (int)(returnVal*xs);
                err = SDKOfxPluginHost::propGetDouble(effectProps,kOfxImageEffectPropRegionOfDefinition,3,&returnVal);
                oDod.Y2 = (int)(returnVal*ys);
            }
	    p->set(&oDod);
	} else if (p == pMaxWidth) {
	    int x = -1;
	    size_t i;
	    for (i=0; i<pIn.getLength(); ++i) {
		if(hasInput(i)) { x = NRiMax(x, ((NRiIPlug *)getInput(i))->getWidth()); }
	    }
	    if(x == -1)
	    {
		if(pIn.getLength() >= 1) { x = pIn[0]->getWidth(); }
		else { x = 0; }
	    }
	    p->set(x);
	} else if (p == pMaxHeight) {
	    int x = -1;
	    size_t i;
	    for (i=0; i<pIn.getLength(); ++i) {
		if(hasInput(i)) { x = NRiMax(x, ((NRiIPlug *)getInput(i))->getHeight()); }
	    }
	    if(x == -1)
	    {
		if(pIn.getLength() >= 1) { x = pIn[0]->getHeight(); }
		else { x = 0; }
	    }
	    p->set(x);
        } else if (p == pIBytes) {
            int iBytes = 0;
            
            // call the clip preferences action
            if(doOfxClipPreferences()) {
                // got it, so fetch it
                iBytes = getClipBitDepth("Output");
            }
            // if we are in a generator node,  let the defined plug drive the byte depth
            if (isGenerator()) {
                NRiPlug *bp = getPlug(SDKOfxPluginHost::hostGeneratorBytesName);
                if (bp) {
                    iBytes = bp->asInt();
                }
            }
            iBytes = NRiMax(iBytes, 1);
	    p->set(iBytes);            

	} else if (p == pOut->active()) {
	    pOut->getCacheId();
            doOfxClipPreferences();
	    int oActive = 0;
              // previously specified in clip prefernecs call in ibytes
            oActive = getClipComponents("Output");
            
	    p->set(oActive);
	} else {
	    for (size_t i=0; i<pIn.getLength(); ++i) {
                if (p == pIn[i]->mask()) {
                    pOut->getCacheId();
                    int iActive = pIn[i]->getActive();
                    int oActive = pOut->getActive();
                    int oMask = pOut->getMask();
                    int iMask;
                    iMask = getClipComponents(i);
                    p->set(iMask);
                    return NRiMonadic::eval(p);   // don't let NRiNadic handle this since we are dealing with it here 
                }
    else
        if (p == pIn[i]->roi()) {
          pOut->getCacheId();

          // does this clip tile?
          SDKOfxClip *clip = m_ofxInfo.getClipByName(m_clipNames[i].getString());
          OfxPropertySetHandle clipProps = clip->getPropertySetHandle();
          int clipSupportsTiles = 1;
          SDKOfxPluginHost::propGetInt(clipProps,kOfxImageEffectPropSupportsTiles,0, &clipSupportsTiles);
      
          // holds what we want from the input clip
          NRiIRect iRoi;

          // get the input's DoD
          NRiIRect iDod = pIn[i]->getDod();

          // if the clip doesn't support tiles, or the effect doesn't support tiles, ignore the RoI call and fetch full DoD input
          if(clipSupportsTiles && effectMode != kFullFrame) {
            // get what needs to be rendered
            NRiIRect roi = pOut->getRoi();

            // call the OFX ROI action
            doOfxGetRegionsOfInterest(roi);
            iRoi = getClipRoI(i);
            iRoi = iRoi.intersect(iDod);
          }
          else {
            iRoi = iDod;
          }

          // and set it in our member and pass it back
          *m_iRois[i] = iRoi;
          p->set(m_iRois[i]);
          break;
        }
	    }
	}
    }        
    return NRiNadic::eval(p);
}

namespace SDKOfx {
    
    /// templated function that turns a buffer of BGRABGRA.... int AAAAA...
    template <class COMP>
    void convertToSingleAlpha(COMP *buf, int width, int height, int endStride)
    {
        for(int y = 0; y < height; y++) {
            COMP *dst = buf;
            COMP *src = buf;
            int n = width;
            while(n--) {
                *dst = *src;
                dst++; 
                src+=4;
            }
            buf = buf + 4 * (width + endStride);
        }
    }
    

    void convertToSingleAlpha(NRiIBuf *src, int bytes, int width)
    {
        switch(bytes) {
            case 4 : convertToSingleAlpha((float *)src->cPtr, width, src->nLine, src->cStride); break;
            case 2 : convertToSingleAlpha((uint16_t *)src->cPtr, width, src->nLine, src->cStride); break;                
            case 1 : convertToSingleAlpha((uint8_t *)src->cPtr, width, src->nLine, src->cStride); break;

        }
    }

    /// templated function that turns a buffer of AAAA... into ABGRABGRABGR...
    template <class COMP>
    void convertFromSingleAlpha(COMP *buf, int width, int height, int endStride)
    {
      for(int y = 0; y < height; y++) {
        COMP *dst = buf + 4 * (width - 1); // position it at the end of the RGBA scan line
        COMP *src = buf + width - 1; // position it at the end of the packed alphas
        int n = width;
        while(n--) {
          dst[0] = *src; // alpha
          dst[1] = dst[2] = dst[3] = 0; // BGR
          dst -= 4;
          src--;
        }
        // move on a scan line
        buf = buf + 4 * (width + endStride);
      }
    }
    

    void convertFromSingleAlpha(NRiIBuf *src, int bytes, int width)
    {
        switch(bytes) {
            case 4 : convertFromSingleAlpha((float *)src->cPtr, width, src->nLine, src->cStride); break;
            case 2 : convertFromSingleAlpha((uint16_t *)src->cPtr, width, src->nLine, src->cStride); break;                
            case 1 : convertFromSingleAlpha((uint8_t *)src->cPtr, width, src->nLine, src->cStride); break;

        }
    }
}

int SDKOfxEffect::endExec()
{
    m_oBuffer.deallocate();
    
    return NRiNadic::endExec();
}

int SDKOfxEffect::fillInputs()
{
  // force the RoIs to be valid
  size_t i,n = pIn.getLength();  
  for(i=0;i<n;i++) {
    pIn[i]->getRoi(); 
  }
 
  NRiIBuf *oBuf = pOut->getIBuf();
  NRiIRect oRoi = pOut->getRoi();  
  int currentLine = oBuf->line;

  // we are rendering full output RoI images
  if (currentLine == oRoi.Y1) {
    /// compile time flag for little-endian-ness
    static const int isLittleEndian	= ( NRiBE( (uint32_t)1) == (uint32_t)1 ? 0 : 1 );   

    // what we have to fill in
    NRiIBuf * oBuf = pOut->getIBuf();
    NRiIRect oDod = pOut->getDod();
    int oMask = pOut->getMask();

    // call the OFX clip preferences to get the bounds for the input image
    doOfxGetRegionsOfInterest(oRoi);

    int iBytes = pIBytes->asInt();
  
    for (i=0; i<n; ++i) {
      // are we connected ?
      NRiIPlug *ip = getImageInput(i);
      NRiIPlug *grpNodeIp = ip?(NRiIPlug *)ip->getLogicalInput():0;
      NRiIPlug *externalIp = grpNodeIp?(NRiIPlug *)grpNodeIp->getLogicalInput():0;
      int connected = (externalIp!=0);

      if(connected) {
        // does this clip tile?
        SDKOfxClip *clip = m_ofxInfo.getClipByName(m_clipNames[i].getString());
        OfxPropertySetHandle clipProps = clip->getPropertySetHandle();
        int clipSupportsTiles = 1;
        SDKOfxPluginHost::propGetInt(clipProps,kOfxImageEffectPropSupportsTiles,0, &clipSupportsTiles);
      
        int components = 0;

        // holds what we want from the input clip
        NRiIRect iRoi;

        // get the input's DoD
        NRiIRect iDod = pIn[i]->getDod();

        // if the clip doesn't support tiles, or the effect doesn't support tiles, ignore the RoI call and fetch full DoD input
        if(clipSupportsTiles && effectMode != kFullFrame) {
          iRoi = getClipRoI(i);
          iRoi = iRoi.intersect(iDod);
        }
        else {
          iRoi = iDod;
        }

        // how big is it
        int roiW = iRoi.X2 - iRoi.X1;
        int roiH = iRoi.Y2 - iRoi.Y1;
        size_t lcSize = roiW * iBytes * 4;
      
        // make some room (Soylent Green is people!)
        if (pIn[i]->getMask() & kRGBA && lcSize > 0) {
          m_iBuffers[i]->allocate(roiH * lcSize);
        } else {
          m_iBuffers[i]->deallocate();
        }

        if(m_iBuffers[i]->data.v) {

          // get the optimal height for this tile
          int tileHeight =  pIn[i]->getBestTileHeight();
          int nTiles = roiH/tileHeight;

          // tile the input into our buffer in stripes
          for(int t = 0; t <= nTiles; t++ ) {                             
            // doing the last tile? Check for remnants
            int thisTileHeight = t < nTiles ? tileHeight : (roiH % tileHeight);
            if(thisTileHeight == 0)
              break;

            // set up our buffer for this tile
            NRiIBuf *iBuf = iBufs[i];
            iBuf->cPtr = m_iBuffers[i]->data.b + (t * tileHeight * lcSize);
            iBuf->zPtr = 0;
            iBuf->cStride = 0;
            iBuf->zStride = 0;
          
            iBuf->line = iRoi.Y1 + t * tileHeight;
            iBuf->nLine = thisTileHeight;
          
            pIn[i]->iBuf()->set(iBuf);
            if (pIn[i]->getOBuf() == 0) {
              return -1;
            }
          
            // what was the output buffer composed of?
            components = getClipComponents(m_rawClipNames[i].getString());
          
            // if the OFX effect wants single component, then make it so
            if(components == kA) {
              SDKOfx::convertToSingleAlpha(iBuf, iBytes, iRoi.X2 - iRoi.X1);
            }
            else {
              // otherwise expand and convert to RGBA
              iBuf->expand(iRoi.X2 - iRoi.X1, 
                           iBytes,
                           pIn[i]->getBPixel(), 
                           pIn[i]->getActive(), 
                           oMask & ~kG
                           );
              iBuf->toRGBA(iBuf,iRoi.X2 - iRoi.X1,iBytes,isLittleEndian);  
            }
          }
        }

        // set up our OFX image which is the accumulated set of tiled renders
        setImageProperties(pIn[i], iBytes, components, iRoi, iDod, m_iBuffers[i]->data.v, 0);
      }    
      else {
        setImageProperties(pIn[i], 0, 0, NRiIRect::nullIRect, NRiIRect::nullIRect, 0, 0);// empty bugger
      }
    }
  }
  return 0;
}

void SDKOfxEffect::fillOutputBuffer()
{
    // compile time flag for little-endian-ness
    static const int isLittleEndian	= ( NRiBE( (uint32_t)1) == (uint32_t)1 ? 0 : 1 );   

    NRiIBuf * oBuf;
    NRiIRect oRoi;
    int oMask;
    NRiName cId;
    int oBytes;
    NRiIRect oDod = pOut->getDod();
    
#ifdef RENDER_BPIXEL    
    if (_bPixelRender)
    {
        oRoi.X1 = 0;
        oRoi.X2 = 1;
        oRoi.Y1 = 0;        
        oRoi.Y2 = 1;
        oMask = kRGBA;
        
        NRiIBuf bPixBuf;        
        oBuf = &bPixBuf;
        oBuf->cPtr = (void*)&(m_bPixel.a);
        oBuf->zPtr = &m_bPixel.z;
        oBuf->cStride = oBuf->zStride = 0;
        oBuf->line = 0;
        oBuf->nLine = 1;
        oBytes = mapToSupportedDepth(4);       // bPixel buffer should be float but the plugin may not be able to handle that, do the best we can and conver to float after render
    }
    else
    {
#endif
        oBuf = pOut->getIBuf();
        oRoi = pOut->getRoi();  
        oMask = pOut->getMask();
        cId = pOut->getCacheId();
        oBytes = pOut->getBytes();
#ifdef RENDER_BPIXEL    
    }
#endif
    
//    assert(oBuf->cStride == 0);
    
    int currentLine = oBuf->line;
    double time = pOut->time()->asDouble();
    OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(&m_ofxInfo);
    
    if (currentLine == oRoi.Y1)
    {    
        int bottom, top, stride;
        int nLines;
        
        bottom = oRoi.Y1; 
        top = oRoi.Y2;
        stride = 0;
        nLines = top-bottom;
            
        m_oBuffer.allocate(nLines*(oRoi.X2-oRoi.X1)*4*oBytes);   // TODO -- sizes need to be rethought,  should render the full DOD in full frame            
        
        // Set the image properties to allow rendering
        NRiIRect oDataRect = oRoi;
        oDataRect.Y1 = bottom;
        oDataRect.Y2 = top;
        setImageProperties(pOut, oBytes, getClipComponents("Output"), oDataRect, oDod, m_oBuffer.data.v, stride);
        setClipProperties(pOut,1);
        
        OfxStatus err;
        NRiHashTable<SDKOfxProperty*> renderPropertiesHash;
        OfxPropertySetHandle renderProperties = (OfxPropertySetHandle)(&renderPropertiesHash);
        SDKOfxPluginHost::createProp(renderProperties, kOfxPropTime, kDouble);
        SDKOfxPluginHost::createProp(renderProperties, kOfxImageEffectPropRenderWindow, kInt, 0, 4);    
        SDKOfxPluginHost::createProp(renderProperties, kOfxImageEffectPropFrameRange, kDouble, 0, 2);
        SDKOfxPluginHost::createProp(renderProperties, kOfxImageEffectPropFrameStep, kDouble);    
        SDKOfxPluginHost::createProp(renderProperties, kOfxPropIsInteractive, kInt);        
        SDKOfxPluginHost::createProp(renderProperties, kOfxImageEffectPropRenderScale, kDouble, 0, 2);    
        SDKOfxPluginHost::createProp(renderProperties, kOfxImageEffectPropFieldToRender, kString);            
        
        NRiTimeRange tr = pOut->getTimeRange();
        
        SDKOfxPluginHost::propSetDouble(renderProperties,kOfxPropTime,0,time);
        SDKOfxPluginHost::propSetInt(renderProperties,kOfxImageEffectPropRenderWindow,0,oRoi.X1);
        SDKOfxPluginHost::propSetInt(renderProperties,kOfxImageEffectPropRenderWindow,1,currentLine);
        SDKOfxPluginHost::propSetInt(renderProperties,kOfxImageEffectPropRenderWindow,2,oRoi.X2);
        SDKOfxPluginHost::propSetInt(renderProperties,kOfxImageEffectPropRenderWindow,3,currentLine+nLines);
        SDKOfxPluginHost::propSetDouble(renderProperties,kOfxImageEffectPropFrameRange,0,tr.iPoint);    
        SDKOfxPluginHost::propSetDouble(renderProperties,kOfxImageEffectPropFrameRange,1,tr.oPoint);
        SDKOfxPluginHost::propSetDouble(renderProperties,kOfxImageEffectPropFrameStep,0,1.);            
        SDKOfxPluginHost::propSetInt(renderProperties,kOfxPropIsInteractive,0,globalRunlevel->asInt());
        SDKOfxPluginHost::propSetDouble(renderProperties, kOfxImageEffectPropRenderScale, 0, globalXProxyScale->asDouble());
        SDKOfxPluginHost::propSetDouble(renderProperties, kOfxImageEffectPropRenderScale, 1, globalYProxyScale->asDouble());
        SDKOfxPluginHost::propSetString(renderProperties, kOfxImageEffectPropFieldToRender, 0, kOfxImageFieldNone);
        
        err = m_ctrl->mainEntry(kOfxImageEffectActionBeginSequenceRender, thisHandle, renderProperties, NULL);
        err = m_ctrl->mainEntry(kOfxImageEffectActionRender, thisHandle, renderProperties, NULL);
        err = m_ctrl->mainEntry(kOfxImageEffectActionEndSequenceRender, thisHandle, renderProperties, NULL);

        cleanUpFetchedImages();
        NRiIBuf imgBuf;
        imgBuf.cPtr = m_oBuffer.data.v;
        imgBuf.line = bottom;
        imgBuf.nLine = nLines;
        imgBuf.cStride = stride;
        int outComps = getClipComponents("Output");
        if(outComps == kA) {
          // have to unpack AAA... into ABGRABGR....
            SDKOfx::convertFromSingleAlpha(&imgBuf, oBytes, oRoi.X2 - oRoi.X1);
        }
        else {
          // other wise flip RGBA to ABGR
          imgBuf.fromRGBA(&imgBuf, oRoi.X2 - oRoi.X1, oBytes, isLittleEndian);   
        }
    }

    if(m_oBuffer.data.v) {
        int lineDataWidth = (oRoi.X2 - oRoi.X1)*oBytes*4;
        uint8_t *srcPtr = (uint8_t *)(m_oBuffer.data.v) + lineDataWidth*(currentLine-oRoi.Y1);
        uint8_t *dstPtr = (uint8_t *)oBuf->cPtr;
        
        int y;
        for (y = 0; y < oBuf->nLine; y++)
        {
            memcpy(dstPtr, srcPtr, lineDataWidth);                        // copy from our buffered full image to the oBuf tile        
            dstPtr += lineDataWidth + oBuf->cStride*oBytes;        
            srcPtr += lineDataWidth;
        }
        
        
    }
}

void SDKOfxEffect::forceTwkRebuild()
{
    NRiPlug *tmp = addPlug("_dummy", kFloat, NRiPlug::kIn);
    delete(tmp);
    tmp = getParent()->addPlug("_dummy", kFloat, NRiPlug::kIn);    
    delete(tmp);
}

/// look in the clip preferences and get the bit depth for the named clip
int SDKOfxEffect::getClipBitDepth(const char *name)
{
  // make up the name of the clip depth in the property set
  NRiString clipName = SDKOfx::mapClipName(name);
  NRiString depthPropName = "OfxImageClipPropDepth_";
  depthPropName += clipName;

  // get it from the prop set
  OfxPropertySetHandle clipPrefsProps = getPluginClipPreferenceProperties();  
  char *cstr = 0;
  if(clipPrefsProps && (SDKOfxPluginHost::propGetString(clipPrefsProps, depthPropName.toCStr(), 0,&cstr) == kOfxStatOK)) {
    // and turn that into a shape bit depth
    return SDKOfx::mapFromOfxDepth(cstr);
  }
  else {
    return 0;
  }
}

/// look in the clip preferences and get the active bit mask for the ith clip
int SDKOfxEffect::getClipBitDepth(int i)
{
  return getClipBitDepth(m_rawClipNames[i].getString());
}

/// look in the clip preferences and get the active bit mask for the named clip
int SDKOfxEffect::getClipComponents(const char *name)
{
  // make up the name of the clip depth in the property set
  NRiString clipName = SDKOfx::mapClipName(name);
  NRiString depthPropName = "OfxImageClipPropComponents_";
  depthPropName += clipName;

  // get it from the prop set
  OfxPropertySetHandle clipPrefsProps = getPluginClipPreferenceProperties();  
  char *cstr = 0;
  if(clipPrefsProps && (SDKOfxPluginHost::propGetString(clipPrefsProps, depthPropName.toCStr(), 0,&cstr) == kOfxStatOK)) {
    // and turn that into a shape bit depth
    return SDKOfx::mapFromOfxComponents(cstr);
  }
  else {
    return 0;
  }
}

/// look in the clip preferences and get the active bit mask for the ith clip
int SDKOfxEffect::getClipComponents(int i)
{
  return getClipComponents(m_rawClipNames[i].getString());
}



/// map from shake's pixel coordinates to OFX canonical coords
void SDKOfxEffect::mapToOFXCoords(double &x, double &y)
{
  double xS, yS;
  SDKOfx::getCoordinateScaleFactors(xS, yS);
  x = x / xS;
  y = y / yS;
}

/// map to shake's pixel coordinates from OFX canonical coords
void SDKOfxEffect::mapFromOfxCoords(double &x, double &y)
{
  double xS, yS;
  SDKOfx::getCoordinateScaleFactors(xS, yS);
  x = x * xS;
  y = y * yS;
}

/// clean up all fetched images that were not released during the action
void SDKOfxEffect::cleanUpFetchedImages()
{
  for (int i = 0; i < pIn.getLength(); ++i) {
    // does this clip tile?
    SDKOfxClip *clip = m_ofxInfo.getClipByName(m_clipNames[i].getString());
    clip->cleanUpFetchedImages();
  }
}

namespace SDKOfx {

  /// get the render scale factors 
  void getRenderScaleFactors(double &x, double &y)
  {
    x = globalXProxyScale->asDouble();
    y = globalYProxyScale->asDouble();
  }

  /// get the pixel aspect ratio
  void getPixelAspectRatio(double &par)
  {
    par = globalDefAspect->asDouble();
  }

  /// get the scale factors that map from OFX canonical coords to Shake's pixel coordinates
  void getCoordinateScaleFactors(double &x, double &y)
  {                                                                 
    getRenderScaleFactors(x, y);
    double par = 1;
    getPixelAspectRatio(par);
    x /= par;
  }
};

/// map to shake's pixel coordinates from OFX canonical coords
int SDKOfxEffect::isGenerator()
{
    char *choice;
    SDKOfxPluginHost::propGetString(m_ctrl->getPropertySetHandle(), kOfxImageEffectPropContext, 0, &choice);
    return (strcmp(choice,kOfxImageEffectContextGenerator) == 0);
} 
    
    
