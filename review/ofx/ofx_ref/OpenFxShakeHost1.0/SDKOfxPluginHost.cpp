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
* $Header: /cvsroot/openfxshakehost/OFX/SDKOfxPluginHost.cpp,v 1.2 2006/09/12 19:51:07 dcandela Exp $
*
*
*/

#include <NRiCrop.h>
#include <NRiCurve.h>
#include <NRiCurveManager.h>
#include <NRiDir.h>
#include <NRiDoBox.h>
#include <NRiEval.h>
#include <NRiGlobals.h>
#include <NRiMath.h>
#include <NRiMenu.h>
#include <NRiNadic.h>
#include <NRiNodeUI.h>
#include <NRiScript.h>
#include <NRiTweekReg.h>
#include <NRiUpdater.h>
#include <NRiColorToggle.h>
#include <NRiModalWin.h>
#include <NRiEvSrc.h>
#include <NRiFile.h>

#include <ofxCore.h>
#include <ofxProperty.h>
#include <ofxImageEffect.h>

#include <SDKOfxEffect.h>
#include <SDKOfxPluginHost.h>
#include <SDKOfxUtility.h>
#include <SDKOfxOverlay.h>
#include <SDKOfxThreading.h>

static NRiPlug *gp_loadedOfxPluginFiles = NRiGlobals::ref("sys.loadedOfxPluginFiles",kPtr);     // list of Ofx plugin libs
static NRiPlug *currentScript		= NRiGlobals::ref("script.currentScript",   kPtr);
static NRiPlug *globalTime              = NRiGlobals::ref("script.time", kFloat);
static NRiPlug *ofxIsAvailable          = NRiGlobals::ref("sys.ofxAvailable", kInt);
static NRiPlug *globalRunlevel          = NRiGlobals::ref("sys.runLevel", kInt);
static NRiPlug *gIScrubRefCnt           = NRiGlobals::ref("gui.inputScrubRefCnt", kInt);
static NRiPlug *globalInfiniteOPoint    = NRiGlobals::ref("timeRange.infiniteOPoint", kString);
static NRiPlug *globalInfiniteIPoint    = NRiGlobals::ref("timeRange.infiniteIPoint", kString);



static NRiPlug *gp_ofxPlugins	       =  NRiGlobals::def("sys.ofxPlugins", 0, kPtr);           // list of plugins

static NRiPArray<SDKOfxPluginCtrl> ofxPluginList;

extern "C"
{
    void nuiDefSlider(const char* s, const char* lo, const char* hi, const char* granularity = "0", const char* notchSpacing= "-1",
                         const char* virtualSlider = "-1", int ignored = 0);
    void nuiDefTweakerNumCol(const char* name, const char* numCol);
    void nuiDefPCtrlLoc(const char* s, const char* columnNum = "-1");

    void nuxDefButtonCtrl(const char *name, int useLabel=0, int align=1 , const char *icons = 0);
    void nuxDefMultiChoice(const char *name, const char *choices, int close=0, int multicolumn=1, int displayItems=35);
    void nuiPushControlGroup(const char* groupName);
    void nuiPushControlWidget(const char* groupName, const char* cmd);
    void nuiGroupControl(const char* controlName);
    void nuiPopControlGroup();
    void nuiConnectColorTriplet(int colorSpace = NRiColorToggle::kRGBToggle, int stat = NRiColorToggle::kCurrentColor, int useSourceBuf = 0);
    void nuxDefExprToggle(int n, const char *name, const char *state = 0, ...);
    void nuxDefRadioBtnOCtrl(int n,const char *name, int useIcon, int useLabel, int animatable, const char *state0, ...);
    void nriDefNoKeyPCtrl(const char*);
}

//==============================================================================
/// Implements a loader of Ofx plugins
///
/// This example demonstrates how to load and execute Ofx plugins.
//==============================================================================
static OfxHost SDKOfxHost;
static SDKOfxPluginHost *thePluginHost = 0;

class EnableOFX
{
public :
    EnableOFX() { ofxIsAvailable->set(1); }
};

EnableOFX g_enable;


NRiName SDKOfxPluginHost::hostPropName = NRiName( "com.apple.shake");
NRiName SDKOfxPluginHost::hostGeneratorWidthName  = NRiName( "ofx_width");
NRiName SDKOfxPluginHost::hostGeneratorHeightName = NRiName( "ofx_height");
NRiName SDKOfxPluginHost::hostGeneratorBytesName  = NRiName( "ofx_bytes");

void *hostFetchSuite(OfxPropertySetHandle host, const char *suiteName, int suiteVersion)
{
    void *returnSuite = 0;
    if (suiteVersion == 1) {
	if (NRiName(suiteName) == NRiName(kOfxPropertySuite)) {
	    returnSuite = (void*)(&(thePluginHost->hostPropertySuite));
	}
	else if (NRiName(suiteName) == NRiName(kOfxImageEffectSuite)) {
	    returnSuite = (void*)(&(thePluginHost->hostImageEffectSuite));
	}
	else if (NRiName(suiteName) == NRiName(kOfxParameterSuite)) {
	    returnSuite = (void*)(&(thePluginHost->hostParameterSuite));
	}
	else if (NRiName(suiteName) == NRiName(kOfxMultiThreadSuite)) {
	    returnSuite = (void*)(&(thePluginHost->hostThreadSuite));
	}
	else if (NRiName(suiteName) == NRiName(kOfxMemorySuite)) {
	    returnSuite = (void*)(&(thePluginHost->hostMemorySuite));
	}
	else if (NRiName(suiteName) == NRiName(kOfxInteractSuite)) {
	    returnSuite = (void*)(&(thePluginHost->hostInteractSuite));
	}
	else if (NRiName(suiteName) == NRiName(kOfxMessageSuite))
    {
	    returnSuite = (void*)(&(thePluginHost->hostMessageSuite));
	}
        else
        {
            NRiSys::error("%EOFX plug-in is requesting an unknown suite: %s.\n", suiteName);
        }
    }
    return returnSuite;
}

SDKOfxPluginHost::SDKOfxPluginHost()
{
    hostPropertySetHandle = getPropertySetHandle();

#ifdef _DEBUG
    printf("debug: SDKOfxPluginHost 0x%X properySet = 0x%X\n", (int)this, (int)getPropertySetHandle());
#endif    
    
    // Setup values for all of the SDK host properties    
    // Setup all the host data plugins will need to access
    SDKOfxHost.host = hostPropertySetHandle;
    SDKOfxHost.fetchSuite = hostFetchSuite;
    
    // create the host properties
    // Type is a host
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxPropType,kString);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxPropType,0,kOfxTypeImageEffectHost);
    // Unique host name
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxPropName,kString);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxPropName,0,hostPropName.getString());
    // Externally visible host name
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxPropLabel,kString);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxPropLabel,0, "Shake");
    // background renderer
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectHostPropIsBackground,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectHostPropIsBackground,0, 0);//globalRunlevel->asInt() ? 0 : 1);
    // support overlays?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportsOverlays,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSupportsOverlays,0,1);
    // support multi-res?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportsMultiResolution,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSupportsMultiResolution,0,1);
   // support tiles?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportsTiles,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSupportsTiles,0,1);
    // support temporal access to images?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropTemporalClipAccess,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropTemporalClipAccess,0,0);
    // supported component types
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportedComponents,kString);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedComponents,0,kOfxImageComponentNone);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedComponents,1,kOfxImageComponentRGBA);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedComponents,2,kOfxImageComponentAlpha);
    // supported contexts
    // Filter will be suppported first,  add support for others later
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportedContexts,kString);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedContexts,0,kOfxImageEffectContextFilter);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedContexts,1,kOfxImageEffectContextGenerator);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedContexts,2,kOfxImageEffectContextGeneral);
    // support multiple clip depths?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportsMultipleClipDepths,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSupportsMultipleClipDepths,0,0);
    // support multiple pixel aspect ratios?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportsMultipleClipPARs,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSupportsMultipleClipPARs,0,0);
    // support effect changing the output frame rate?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSetableFrameRate,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSetableFrameRate,0,0);
    // support effect changing the fielding? (not yet)
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSetableFielding,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPropSetableFielding,0,0);
    // support custom interactions? (not yet)
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropSupportsCustomInteract,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropSupportsCustomInteract,0,0);
    // support string animations?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropSupportsStringAnimation,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropSupportsStringAnimation,0,0);
    // support choice animations?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropSupportsChoiceAnimation,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropSupportsChoiceAnimation,0,0);
    // support boolean animations?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropSupportsBooleanAnimation,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropSupportsBooleanAnimation,0,0);
    // support custom animations?
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropSupportsCustomAnimation,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropSupportsCustomAnimation,0,0);
    // unlimited number of params
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropMaxParameters,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropMaxParameters,0,0);
    // no pages support yet
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropMaxPages,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropMaxPages,0,0);
    // no pages support yet
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxParamHostPropPageRowColumnCount,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropPageRowColumnCount,0,1);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxParamHostPropPageRowColumnCount,1,1);
    // no field rendering yet
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPluginPropFieldRenderTwiceAlways,kInt);
    SDKOfxPluginHost::propSetInt(hostPropertySetHandle,kOfxImageEffectPluginPropFieldRenderTwiceAlways,0,0);
    // Supported bit depth
    SDKOfxPluginHost::createProp(hostPropertySetHandle,kOfxImageEffectPropSupportedPixelDepths,kString);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedPixelDepths,0,kOfxBitDepthNone);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedPixelDepths,1,kOfxBitDepthByte);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedPixelDepths,2,kOfxBitDepthShort);
    SDKOfxPluginHost::propSetString(hostPropertySetHandle,kOfxImageEffectPropSupportedPixelDepths,3,kOfxBitDepthFloat);    
    
    // set all the suite functions to the host implementation
    // property suite
    hostPropertySuite.propSetPointer            = clientSetPointer;
    hostPropertySuite.propSetString             = clientSetString;
    hostPropertySuite.propSetDouble             = clientSetDouble;
    hostPropertySuite.propSetInt                = clientSetInt;
    hostPropertySuite.propSetPointerN           = clientSetPointerN;
    hostPropertySuite.propSetStringN            = clientSetStringN;
    hostPropertySuite.propSetDoubleN            = clientSetDoubleN;
    hostPropertySuite.propSetIntN               = clientSetIntN;
    hostPropertySuite.propGetPointer            = propGetPointer;
    hostPropertySuite.propGetString             = propGetString;
    hostPropertySuite.propGetDouble             = propGetDouble;
    hostPropertySuite.propGetInt                = propGetInt;
    hostPropertySuite.propGetPointerN           = propGetPointerN;
    hostPropertySuite.propGetStringN            = propGetStringN;
    hostPropertySuite.propGetDoubleN            = propGetDoubleN ;
    hostPropertySuite.propGetIntN               = propGetIntN;
    hostPropertySuite.propReset                 = propReset; 
    hostPropertySuite.propGetDimension          = propGetDimension;

    // parameter suite
    hostParameterSuite.paramDefine              = paramDefine;
    hostParameterSuite.paramGetHandle           = paramGetHandle;
    hostParameterSuite.paramSetGetPropertySet   = paramSetGetPropertySet;
    hostParameterSuite.paramGetPropertySet      = paramGetPropertySet;
    hostParameterSuite.paramGetValue            = paramGetValue;
    hostParameterSuite.paramGetValueAtTime      = paramGetValueAtTime;
    hostParameterSuite.paramGetDerivative       = paramGetDerivative;
    hostParameterSuite.paramGetIntegral         = paramGetIntegral;
    hostParameterSuite.paramSetValue            = paramSetValue;
    hostParameterSuite.paramSetValueAtTime      = paramSetValueAtTime;
    hostParameterSuite.paramGetNumKeys          = paramGetNumKeys;
    hostParameterSuite.paramGetKeyTime          = paramGetKeyTime;
    hostParameterSuite.paramGetKeyIndex         = paramGetKeyIndex;
    hostParameterSuite.paramDeleteKey           = paramDeleteKey;
    hostParameterSuite.paramDeleteAllKeys       = paramDeleteAllKeys;
    hostParameterSuite.paramCopy                = paramCopy;
    hostParameterSuite.paramEditBegin           = paramEditBegin;
    hostParameterSuite.paramEditEnd             = paramEditEnd;

    // Image effect suite
    hostImageEffectSuite.getPropertySet            = getPropertySet;
    hostImageEffectSuite.getParamSet               = getParamSet;
    hostImageEffectSuite.clipDefine                = clipDefine;
    hostImageEffectSuite.clipGetHandle             = clipGetHandle;
    hostImageEffectSuite.clipGetPropertySet        = clipGetPropertySet;
    hostImageEffectSuite.clipGetImage              = clipGetImage;
    hostImageEffectSuite.clipReleaseImage          = clipReleaseImage;
    hostImageEffectSuite.clipGetRegionOfDefinition = clipGetRegionOfDefinition;
    hostImageEffectSuite.abort                     = abort;
    hostImageEffectSuite.imageMemoryAlloc          = imageMemoryAlloc;
    hostImageEffectSuite.imageMemoryFree           = imageMemoryFree;
    hostImageEffectSuite.imageMemoryLock           = imageMemoryLock;
    hostImageEffectSuite.imageMemoryUnlock         = imageMemoryUnlock;

    // Memory suite
    hostMemorySuite.memoryAlloc = memoryAlloc;
    hostMemorySuite.memoryFree = memoryFree;
    
    // Thread suite
    hostThreadSuite.multiThread = SDKOfx::multiThread;
    hostThreadSuite.multiThreadNumCPUs = SDKOfx::multiThreadNumCPUs;
    hostThreadSuite.multiThreadIndex = SDKOfx::multiThreadIndex;    
    hostThreadSuite.multiThreadIsSpawnedThread = SDKOfx::multiThreadIsSpawnedThread;
    hostThreadSuite.mutexCreate = mutexCreate;
    hostThreadSuite.mutexDestroy = mutexDestroy;
    hostThreadSuite.mutexLock = mutexLock;
    hostThreadSuite.mutexUnLock = mutexUnLock;
    hostThreadSuite.mutexTryLock = mutexTryLock;    
    
    // Interact suite
    hostInteractSuite.interactSwapBuffers = interactSwapBuffers;
    hostInteractSuite.interactRedraw = interactRedraw;
    hostInteractSuite.interactGetPropertySet = interactGetPropertySet;

    // Message suite
    hostMessageSuite.message = message;
} 

SDKOfxPluginHost::~SDKOfxPluginHost()
{
    //TODO clean up all the static host properties created and allocated in the constructor
    
}


OfxStatus SDKOfxPluginHost::getParamSet(OfxImageEffectHandle pPlugin, OfxParamSetHandle* paramSet)
{
    if(!pPlugin)
        return kOfxStatErrBadHandle;

    *paramSet = ((SDKOfxPlugin*)pPlugin)->getParamSetHandle();

    return kOfxStatOK;
}

int SDKOfxPluginHost::abort(OfxImageEffectHandle imageEffect)
{
    bool abort = false;
    
    NRiNode::InterruptState interruptState = NRiNode::getInterruptState();
    
    if ( interruptState != NRiNode::kNoInterrupt ) {
        abort = true;
    }    
    
    return abort;
}

// Add plugin to the ofxPluginList. 
void SDKOfxPluginHost::addPlugin(OfxPlugin *plugin, const char *pathToPlugin)
{
    SDKOfxPluginCtrl *ofxPI = new SDKOfxPluginCtrl(plugin, pathToPlugin);    
    
    int idx = getPluginIdx(plugin->pluginIdentifier);
    int nPIs = ofxPluginList.getNbItems();
    
    if (idx >= 0)
    {
        int needInsert = 1;

        while ((ofxPluginList[idx]->getName() == NRiString(plugin->pluginIdentifier)) && (idx < nPIs))
        {
            SDKOfxPluginCtrl *existingPlugin = ofxPluginList[idx];
            if (existingPlugin->getPluginVersionMajor() == plugin->pluginVersionMajor)
            {
                if (existingPlugin->getPluginVersionMinor() < plugin->pluginVersionMinor)
                {
                    ofxPI->setObsoleteVersion(ofxPluginList[idx]->isObsoleteVersion());
                    delete ofxPluginList[idx];
                    ofxPluginList[idx] = ofxPI;
                    needInsert = 0;
                    break;
                }
                else
                {
                    delete ofxPI;
                    return;        // we have an obsolete minor version, don't keep it.
                }
            }
            else if (existingPlugin->getPluginVersionMajor() > plugin->pluginVersionMajor)
            {
                ofxPI->setObsoleteVersion(true);
            }
            else
            {
                ofxPluginList[idx]->setObsoleteVersion(true);              
                ofxPluginList.insert(ofxPI, idx); 
                needInsert = 0;                
                break;
            }
            idx++;
        }
        
        if (needInsert)
        {
            if (idx < nPIs) ofxPluginList.insert(ofxPI, idx); 
            else ofxPluginList.append(ofxPI);
        }
    }
    else
    {
        ofxPluginList.append(ofxPI);
    }
    
#ifdef _DEBUG    
    NRiSys::error("added plugin %s \n", plugin->pluginIdentifier);
#endif    
}


int SDKOfxPluginHost::getPluginIdx(const char *piname)
{
    int nPlugins = ofxPluginList.getNbItems();
    int i;
    for (i = 0; i < nPlugins; i++)
    {
        SDKOfxPluginCtrl *pi = ofxPluginList[i];
        if (pi->getName() == NRiString(piname)) 
        {
            return i;
        }
    }
    return -1;
}



SDKOfxPluginCtrl *SDKOfxPluginHost::getPluginByFactoryName(const char *piname)
{
    int nPlugins = ofxPluginList.getNbItems();
    int i;
    for (i = 0; i < nPlugins; i++)
    {
        SDKOfxPluginCtrl *pi = ofxPluginList[i];
        if (pi->getPluginFactoryName() == piname) 
        {
            return pi;
        }
    }
    return 0;
}



// When a plug-in calls the host to define a parameter, the only means of identifying which plug-in
// it belongs to is the parameter-set handle.  That handle is just a blind pointer to whatever we're using
// as a collection to hold our descriptions of the parameters.
SDKOfxPluginCtrl* SDKOfxPluginHost::getPluginByParamSet(OfxParamSetHandle hParamSet)
{
    int nPlugins = ofxPluginList.getNbItems();
    int i;
    for (i = 0; i < nPlugins; i++)
    {
        SDKOfxPluginCtrl* pPlugin = ofxPluginList[i];
        if (pPlugin->getParamSetHandle() == hParamSet) 
        {
            return pPlugin;
        }
    }
    
    return 0;
}


// Implementation of the properties suite
//TODO add proper return errors for property suite, checking handles,  number of elements in Get/SetN methods
NRiValue *SDKOfxPluginHost::getPropertySetValue(OfxPropertySetHandle properties, const char *property, int index, int type)
{
    NRiValue *returnVal = 0;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    assert(p);
    SDKOfxProperty *ofxp = p ? p->getValue(property) : 0;
    if (!ofxp)
    {
        ofxp = new SDKOfxProperty(type);
        p->addEntry(NRiName(property),ofxp);
    }
    
    if (ofxp && (ofxp->getType() == type))
    {
	if (index == ofxp->getDimension()) {
	    //if something is requesting index+1 create it.  If the index is out of bounds and not dimension+1 fail
	    ofxp->resize(index+1);
	}
	if (index < ofxp->getDimension()) returnVal = ofxp->getValuePtr(index);
    }
    return returnVal;
}

NRiPlug *SDKOfxPluginHost::getPropertySetPlug(OfxPropertySetHandle properties, const char *property, int index, int type)
{
    NRiPlug *returnVal = 0;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    assert(p);
    SDKOfxProperty *ofxp = p ? p->getValue(property) : 0;
    
    if (ofxp && (ofxp->getType() == type))
    {
	if (index == ofxp->getDimension()) {
	    //if something is requesting index+1 create it.  If the index is out of bounds and not dimension+1 fail
	    ofxp->resize(index+1);
	}
	if (index < ofxp->getDimension()) returnVal = ofxp->getPlug(index);
    }
    return returnVal;
}

OfxStatus SDKOfxPluginHost::propGetAsString(OfxPropertySetHandle properties, const char *property, int index, NRiString &value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p ? p->getValue(property) : 0;
    if (ofxp)
    {
        NRiValue *val = ofxp->getValuePtr(index);
        switch (ofxp->getType())
        {
            case kInt :
                value = val->i;
                returnStatus = kOfxStatOK;
                break;
            case kFloat :
                value = val->f;                
                returnStatus = kOfxStatOK;                
                break;
            case kDouble :
                value = val->d;                
                returnStatus = kOfxStatOK;                
                break;
            case kString :
                value = val->n;
                returnStatus = kOfxStatOK;
                break;
            default :
                break;
        }
    }
    
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propGetInt(OfxPropertySetHandle properties, const char *property, int index, int *value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiValue *v = getPropertySetValue(properties, property, index, kInt);
    if (v) {
        *value = v->i;     
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetInt(OfxPropertySetHandle properties, const char *property, int index, int value)
{
    return propSetInt(properties, property, index, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetIntN(OfxPropertySetHandle properties, const char *property, int count, int *value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
    // Nope,  step through the values and set the pointer vals one by one to the prop values
    if (ofxp && (ofxp->getType() == kInt)) {
        if (count <= ofxp->getDimension()) {
            int i;
            for (i=0;i<count;i++) {
                NRiValue *v = ofxp->getValuePtr(i);
                if (v) *value = v->i;
                value++;
            }
            returnStatus = kOfxStatOK;
        } else {
            returnStatus = kOfxStatErrBadIndex;
        }
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetIntN(OfxPropertySetHandle properties, const char *property, int count, int *value)
{
    return propSetIntN(properties, property, count, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetDouble(OfxPropertySetHandle properties, const char *property, int index, double *value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiValue *v = getPropertySetValue(properties, property, index, kDouble);
    if (v) {
        *value = v->d;    
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetDouble(OfxPropertySetHandle properties, const char *property, int index, double value)
{
    return propSetDouble(properties, property, index, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetDoubleN(OfxPropertySetHandle properties, const char *property, int count, double *value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
    if (ofxp && (ofxp->getType() == kDouble)) {
        if (count <= ofxp->getDimension()) {
            int i;
            for (i=0;i<count;i++) {
                NRiValue *v = ofxp->getValuePtr(i);
                if (v) *value = v->d;
                value++;
            }
            returnStatus = kOfxStatOK;
        } else {
            returnStatus = kOfxStatErrBadIndex;
        }
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetDoubleN(OfxPropertySetHandle properties, const char *property, int count, double *value)
{
    return propSetDoubleN(properties, property, count, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetPointer(OfxPropertySetHandle properties, const char *property, int index, void **value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiValue *v = getPropertySetValue(properties, property, index, kPtr);
    if (v) {
        *value = v->p; 
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetPointer(OfxPropertySetHandle properties, const char *property, int index, void *value)
{
    return propSetPointer(properties, property, index, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetPointerN(OfxPropertySetHandle properties, const char *property, int count, void **value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
    if (ofxp && (ofxp->getType() == kPtr)) {
        if (count <= ofxp->getDimension()) {
            int i;
            for (i=0;i<count;i++) {
                NRiValue *v = ofxp->getValuePtr(i);
                if (v) *value = v->p;
                value++;
            }
            returnStatus = kOfxStatOK;
        } else {
            returnStatus = kOfxStatErrBadIndex;
        }
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetPointerN(OfxPropertySetHandle properties, const char *property, int count, void **value)
{
    return propSetPointerN(properties, property, count, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetString(OfxPropertySetHandle properties, const char *property, int index, char **value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiValue *v = getPropertySetValue(properties, property, index, kString);
    if (v) {
        *value = (char *)(v->n); 
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetString(OfxPropertySetHandle properties, const char *property, int index, const char *value)
{
    return propSetString(properties, property, index, value, 1);
}

OfxStatus SDKOfxPluginHost::propGetStringN(OfxPropertySetHandle properties, const char *property, int count, char **value)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
    if (ofxp && (ofxp->getType() == kString)) {
	if (count <= ofxp->getDimension()) {
	    int i;
	    for (i=0;i<count;i++) {
            NRiValue *v = ofxp->getValuePtr(i);
            if (v) *value =(char*)(v->n);
            value++;
	    }
	    returnStatus = kOfxStatOK;
	} else {
	    returnStatus = kOfxStatErrBadIndex;
	}
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clientSetStringN(OfxPropertySetHandle properties, const char *property, int count, const char **value)
{
    return propSetStringN(properties, property, count, value, 1);
}

OfxStatus SDKOfxPluginHost::propSetPointer(OfxPropertySetHandle properties, const char *property, int index, void *value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *pr = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = pr->getValue(property);
    NRiValue *v = getPropertySetValue(properties, property, index, kPtr);
    NRiPlug  *p = getPropertySetPlug(properties, property, index, kPtr);
    if (!ofxp) 
    {
#ifdef _DEBUG           
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kString);
        ofxp = pr->getValue(property);        
    }
#ifdef _DEBUG   
    if (checkRW && ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    if (v && ofxp && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
        v->p = value;
        if (p) p->set(value);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetString(OfxPropertySetHandle properties, const char *property, int index, const char *value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiPlug  *plug = getPropertySetPlug(properties, property, index, kString);
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
   
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kString);
        ofxp = p->getValue(property);        
    }
#ifdef _DEBUG    
    if (ofxp && checkRW && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    if (ofxp && (ofxp->getType() == kString) && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
        NRiName *n = ofxp->getNamePtr(index);
        *n = value;
        if (plug) plug->set(value);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetDouble(OfxPropertySetHandle properties, const char *property, int index, double value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *pr = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = pr->getValue(property);
    NRiValue *v = getPropertySetValue(properties, property, index, kDouble);
    NRiPlug  *p = getPropertySetPlug(properties, property, index, kDouble);
   
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been define be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kDouble);
        ofxp = pr->getValue(property);        
    }
#ifdef _DEBUG    
    if (ofxp && checkRW && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    if (v && ofxp && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
        v->d = value;
        if (p) p->set(value);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetInt(OfxPropertySetHandle properties, const char *property, int index, int value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *pr = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = pr->getValue(property);
    NRiValue *v = getPropertySetValue(properties, property, index, kInt);
    NRiPlug  *p = getPropertySetPlug(properties, property, index, kInt);
   
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kInt);
        ofxp = pr->getValue(property);        
    }
#ifdef _DEBUG    
    if (checkRW && ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    if (v && ofxp && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
        v->i = value;
        if (p) p->set(value);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetPointerN(OfxPropertySetHandle properties, const char *property, int count, void **value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
   
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kPtr);
        ofxp = p->getValue(property);        
    }
#ifdef _DEBUG    
    if (checkRW && ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    // Resize the property if needed
    if (ofxp->getDimension() < count) ofxp->resize(count);
    
    if (ofxp && (ofxp->getType() == kPtr) && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
        if (count <= ofxp->getDimension()) {
            int i;
            for (i=0;i<count;i++) {
                NRiValue *v = ofxp->getValuePtr(i);
                if (v) v->p = *value;
                NRiPlug *plug = ofxp->getPlug(i);
                if (plug) plug->set(*value);
                value++;
            }
            returnStatus = kOfxStatOK;
        } else {
            returnStatus = kOfxStatErrBadIndex;
        }
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetStringN(OfxPropertySetHandle properties, const char *property, int count, const char **value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
   
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kString);
        ofxp = p->getValue(property);        
    }
#ifdef _DEBUG    
    if (checkRW && ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    // Resize the property if needed
    if (ofxp->getDimension() < count) ofxp->resize(count);
    if (ofxp && (ofxp->getType() == kString) && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
	if (count <= ofxp->getDimension()) {
	    int i;
	    for (i=0;i<count;i++) {
                NRiName *n = ofxp->getNamePtr(i);
                if (n) *n = *value;
                NRiPlug *plug = ofxp->getPlug(i);
                if (plug) plug->set(*value);
                value++;
	    }
	    returnStatus = kOfxStatOK;
	} else {
	    returnStatus = kOfxStatErrBadIndex;
	}
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetDoubleN(OfxPropertySetHandle properties, const char *property, int count, double *value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
   
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kDouble);
        ofxp = p->getValue(property);        
    }
#ifdef _DEBUG    
    if (checkRW && ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    // Resize the property if needed
    if (ofxp->getDimension() < count) ofxp->resize(count);
    if (ofxp && (ofxp->getType() == kDouble)) {
        if (count <= ofxp->getDimension() && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
            int i;
            for (i=0;i<count;i++) {
                NRiValue *v = ofxp->getValuePtr(i);
                if (v) v->d = *value;
                NRiPlug *plug = ofxp->getPlug(i);
                if (plug) plug->set(*value);
                value++;
            }
            returnStatus = kOfxStatOK;
        } else {
            returnStatus = kOfxStatErrBadIndex;
        }
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propSetIntN(OfxPropertySetHandle properties, const char *property, int count, int *value, int checkRW)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
    
    if (!ofxp) 
    {
#ifdef _DEBUG        
        NRiSys::error("%EProperty %s has not been defined be we are setting it\n", property);
#endif        
        SDKOfxPluginHost::createProp(properties, property, kInt);
        ofxp = p->getValue(property);        
    }
#ifdef _DEBUG
    if (checkRW && ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif     
    // Resize the property if needed
    if (ofxp->getDimension() < count) ofxp->resize(count);
    
    if (ofxp && (ofxp->getType() == kInt) && (!checkRW || (checkRW && ofxp->getReadWritePermission()==1) )) {
        if (count <= ofxp->getDimension()) {
            int i;
            for (i=0;i<count;i++) {
                NRiValue *v = ofxp->getValuePtr(i);
                if (v) v->i = *value;
                NRiPlug *plug = ofxp->getPlug(i);
                if (plug) plug->set(*value);
                value++;
            }
            returnStatus = kOfxStatOK;
        } else {
            returnStatus = kOfxStatErrBadIndex;
        }
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::propGetDimension(OfxPropertySetHandle properties, const char *property, int *count)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = p->getValue(property);
    if (ofxp) {
        *count = ofxp->getDimension();
        returnStatus = kOfxStatOK;
    } else {
        returnStatus = kOfxStatErrBadIndex;
    }
    return returnStatus;
}     

OfxStatus SDKOfxPluginHost::createProp(OfxPropertySetHandle properties, const char *property, int type, int readWrite, int dimension, int hasPlug)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *p = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *existingProp = p->getValue(property);
    if (!existingProp) {
        SDKOfxProperty *newProp = new SDKOfxProperty(type, readWrite, dimension, hasPlug);
        if (newProp)
        {
            p->addEntry(property, newProp);
            returnStatus = kOfxStatOK;
        }
    }
    
    return returnStatus;
}
OfxStatus SDKOfxPluginHost::propSetPlug (OfxPropertySetHandle properties, const char *property, int index, NRiPlug *p)
{
    OfxStatus returnStatus = kOfxStatFailed;
    NRiHashTable<SDKOfxProperty*> *pr = (NRiHashTable<SDKOfxProperty*>*)(properties);
    SDKOfxProperty *ofxp = pr->getValue(property);
#ifdef _DEBUG   
    if (!ofxp) 
    {
        NRiSys::error("%EProperty %s has not been defined but we are setting the plug\n", property);
    }
    if (ofxp && ofxp->getReadWritePermission()!=1)
    {
        NRiSys::error("%EProperty %s : attempting to write to a read only property\n", property);
    }
#endif
    // Resize the property if needed
    if (ofxp->getDimension() < index) ofxp->resize(index);
    
    if (ofxp){
        ofxp->setPlug(index,p);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}    
    
OfxStatus SDKOfxPluginHost::paramDefine(OfxParamSetHandle paramSet, const char* paramType, const char* paramName, OfxPropertySetHandle* propertySet)
{
    SDKOfxPluginCtrl* pPlugin = getPluginByParamSet(paramSet);
    if(!pPlugin)
    {
        return kOfxStatFailed;
    }
    
    SDKOfxParm* pNewParam = new SDKOfxParm(paramName, paramType);
    if (propertySet) *propertySet = pNewParam->getPropertySetHandle();
    if(!pPlugin->addParameter(pNewParam))
    {
        return kOfxStatErrUnknown;
    }
    else
    {
        return kOfxStatOK;
    }
}

OfxStatus SDKOfxPluginHost::paramGetHandle(OfxParamSetHandle paramSet, const char* name, OfxParamHandle* param, OfxPropertySetHandle* propertySet)
{
    SDKOfxParameterList *parameters = (SDKOfxParameterList *)paramSet;
    SDKOfxParm *parameter = parameters->getParamByName(name);
    if (parameter)
    {
        *param = (OfxParamHandle)parameter;
        if (propertySet) *propertySet = parameter->getPropertySetHandle();
        return kOfxStatOK;        
    }
    
    return kOfxStatErrBadHandle;
}

OfxStatus SDKOfxPluginHost::paramSetGetPropertySet(OfxParamSetHandle paramSet, OfxPropertySetHandle* propHandle)
{
    SDKOfxParameterList *parameters = (SDKOfxParameterList *)paramSet;
    if (parameters == 0) return kOfxStatErrBadHandle;
 
    // From the OFX programming ref. : "The property handle belonging to a parameter set is the same as the property handle belonging to the plugin instance."
    *propHandle = parameters->getOwner()->getPropertySetHandle();
    
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::paramGetPropertySet(OfxParamHandle param, OfxPropertySetHandle* propHandle)
{
    SDKOfxParm *parameter = (SDKOfxParm *)param;
    if (parameter == 0) return kOfxStatErrBadHandle;
    
    *propHandle = parameter->getPropertySetHandle();
        
    return kOfxStatOK;        
}

OfxStatus SDKOfxPluginHost::paramGetValue(OfxParamHandle paramHandle, ...)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;

    va_list arg;
    va_start(arg, paramHandle);
    param->getValue(arg);
    va_end(arg);
    
    return kOfxStatOK;            
}

OfxStatus SDKOfxPluginHost::paramGetValueAtTime(OfxParamHandle paramHandle, OfxTime time, ...)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;    
    if (param == 0) return kOfxStatErrBadHandle;
    
    va_list arg;
    va_start(arg, time);
    param->getValueAtTime((float)time, arg);
    va_end(arg);
    
    return kOfxStatOK;        
}

OfxStatus SDKOfxPluginHost::paramGetDerivative(OfxParamHandle paramHandle,  OfxTime time, ...)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;    
    if (param == 0) return kOfxStatErrBadHandle;
    if (param->getPlugType() == kString) return kOfxStatErrBadHandle;
        
    va_list arg;
    va_start(arg, time);
    param->getDerivativeAtTime((float)time, arg);
    va_end(arg);
    
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::paramGetIntegral(OfxParamHandle paramHandle, OfxTime time1, OfxTime time2, ...)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;    
    if (param == 0) return kOfxStatErrBadHandle;
    if (param->getPlugType() == kString) return kOfxStatErrBadHandle;
    
    va_list arg;
    va_start(arg, time2);
    param->getIntegral((float)time1, (float)time2, arg);
    va_end(arg);
    
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::paramSetValue(OfxParamHandle paramHandle, ...)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    va_list arg;
    va_start(arg, paramHandle);
    param->setValue(arg);
    va_end(arg);
    
    return kOfxStatOK;            
}

OfxStatus SDKOfxPluginHost::paramSetValueAtTime (OfxParamHandle paramHandle, OfxTime time, ...)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    va_list arg;
    va_start(arg, time);
    param->setValueAtTime((float)time, arg);
    va_end(arg);
    
    return kOfxStatOK;            
}

OfxStatus SDKOfxPluginHost::paramGetNumKeys (OfxParamHandle paramHandle, unsigned int* numberOfKeys) 
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    param->getNumKeys(numberOfKeys);
    
    return kOfxStatOK;            
}

OfxStatus SDKOfxPluginHost::paramGetKeyTime(OfxParamHandle paramHandle, unsigned int nthKey, OfxTime* time) 
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    if (param->getPlugType() == kString)  return kOfxStatErrBadHandle;
    
    float keyTime;
    param->getKeyTime(nthKey, &keyTime);
    *time = (OfxTime)keyTime;
    
    return kOfxStatOK;            
}

OfxStatus SDKOfxPluginHost::paramGetKeyIndex(OfxParamHandle paramHandle, OfxTime time, int direction, int* index) 
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    if (param->getPlugType() == kString)  return kOfxStatErrBadHandle;
    
    return param->getKeyIndex(float(time), direction, index);
}

OfxStatus SDKOfxPluginHost::paramDeleteKey(OfxParamHandle paramHandle, OfxTime time)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    if (param->getPlugType() == kString)  return kOfxStatErrBadHandle;
    
    return param->deleteKey(float(time));
}

OfxStatus SDKOfxPluginHost::paramDeleteAllKeys(OfxParamHandle paramHandle)
{
    SDKOfxParm *param = (SDKOfxParm *)paramHandle;
    if (param == 0) return kOfxStatErrBadHandle;
    
    if (param->getPlugType() == kString)  return kOfxStatErrBadHandle;
    
    param->deleteAllKeys();
    return kOfxStatOK;            
}

OfxStatus SDKOfxPluginHost::paramCopy(OfxParamHandle paramTo, OfxParamHandle paramFrom, OfxTime dstOffset, OfxRangeD* frameRange) 
{
    SDKOfxParm *pTo = (SDKOfxParm *)paramTo;
    SDKOfxParm *pFrom = (SDKOfxParm *)paramFrom;
    
    if (pTo == 0 || pFrom == 0) return kOfxStatErrBadHandle;
    if (pTo->getType() != pFrom->getType())
    {
        NRiSys::error("%EparamCopy : attempting to copy parameters of different type");
    }
    
    pTo->copy(pFrom, double(dstOffset), frameRange);
    
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::paramEditBegin(OfxParamSetHandle paramSet,  const char* name) 
{
    NRiUpdater::beginInteraction();
    return kOfxStatOK;
} 

OfxStatus SDKOfxPluginHost::paramEditEnd(OfxParamSetHandle paramSet) 
{
    NRiUpdater::endInteraction();
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::imageMemoryLock(OfxImageMemoryHandle memoryHandle, void **returnedPtr)
{
    if(returnedPtr) {
	void *data = (void *)(memoryHandle);
	if(!data) {
	    return kOfxStatErrBadHandle;
	}
	*returnedPtr = data;
	return kOfxStatOK;
    }
    return kOfxStatErrBadHandle;
}

// Implementation of the effect suite
//TODO add proper return errors for effect suite, checking handles, number of elements in Get/SetN methods
OfxStatus SDKOfxPluginHost::getPropertySet(OfxImageEffectHandle imageEffect, OfxPropertySetHandle *propHandle)
{
    OfxStatus returnStatus = kOfxStatErrBadHandle;
    *propHandle = 0;
    if (imageEffect) 
    {
	SDKOfxBase *pOfxInfo = (SDKOfxBase *)imageEffect;
        *propHandle = pOfxInfo->getPropertySetHandle();
	if(propHandle) {
	    returnStatus = kOfxStatOK;
	}
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::clipDefine(OfxImageEffectHandle imageEffect, const char* name, OfxPropertySetHandle* propertySet)
{
    if(!imageEffect)
    {
        return kOfxStatErrBadHandle;
    }

    NRiString clipName = NRiString("ofx_") + NRiString(name);                 // prepend ofx_ to ensure that we don't use a Shake reserved work (ie. Mask)
    SDKOfxPluginCtrl* pPlugin = (SDKOfxPluginCtrl*)(imageEffect);
    SDKOfxClip* pNewClip = new SDKOfxClip;
    pNewClip->setName(clipName);
    *propertySet = pNewClip->getPropertySetHandle();
    if(!pPlugin->addClip(pNewClip))
    {
        return kOfxStatErrUnknown;
    }
    else
    {
        return kOfxStatOK;
    }

}

OfxStatus SDKOfxPluginHost::interactGetPropertySet(OfxInteractHandle interactInstance, OfxPropertySetHandle *property)
{
    SDKOfxBase *overlayBase = (SDKOfxBase *)interactInstance;    
    *property = overlayBase->getPropertySetHandle();
    return kOfxStatOK;
}


OfxStatus SDKOfxPluginHost::interactSwapBuffers(OfxInteractHandle interactInstance)
{
    SDKOfxBase *overlayBase = (SDKOfxBase *)interactInstance;    
    OfxPropertySetHandle propSet = overlayBase->getPropertySetHandle();
    
    SDKOfxOverlay *overlay;
    SDKOfxPluginHost::propGetPointer(propSet, "ShakeOverlayPtr", 0, (void **)&overlay);
    
    overlay->swapBuffers();
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::interactRedraw(OfxInteractHandle interactInstance)
{
    SDKOfxBase *overlayBase = (SDKOfxBase *)interactInstance;    
    OfxPropertySetHandle propSet = overlayBase->getPropertySetHandle();
    
    SDKOfxOverlay *overlay;
    SDKOfxPluginHost::propGetPointer(propSet, "ShakeOverlayPtr", 0, (void **)&overlay);
    
    overlay->redraw();
    return kOfxStatOK;
}


OfxStatus SDKOfxPluginHost::clipGetHandle(OfxImageEffectHandle imageEffect, const char* name, OfxImageClipHandle* clip, OfxPropertySetHandle* propertySet)
{
    if(!imageEffect)
    {
        return kOfxStatErrBadHandle;
    }
    NRiString clipName(name);
    if (clipName.substr(0, 3) != NRiString("ofx_"))
    {
        clipName = NRiString("ofx_") + clipName;                 // prepend ofx_ to ensure that we don't use a Shake reserved work (ie. Mask)
    }

    SDKOfxPluginCtrl* pPlugin = (SDKOfxPluginCtrl*)imageEffect;
    OfxImageClipHandle clipHandle = (OfxImageClipHandle)(pPlugin->getClipByName(clipName));
    *clip = clipHandle;
    // returning property set is optional
    if (propertySet) *propertySet = (OfxPropertySetHandle)(((SDKOfxClip*)clipHandle)->getPropertyTable());

    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::clipGetPropertySet(OfxImageClipHandle clip, OfxPropertySetHandle* propertySet)
{
    if(!clip)
    {
        return kOfxStatErrBadHandle;
    }    
    if (propertySet) *propertySet = (OfxPropertySetHandle)(((SDKOfxClip*)clip)->getPropertyTable());
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::clipGetImage(OfxImageClipHandle clip, OfxTime time, OfxRectD *region, OfxPropertySetHandle *imageHandle)
{
    if(!clip)
    {
        return kOfxStatErrBadHandle;
    }
    
    return ((SDKOfxClip*)clip)->getImage(time, region, imageHandle);
}

OfxStatus SDKOfxPluginHost::clipReleaseImage(OfxPropertySetHandle imageHandle)
{
    if(!imageHandle)
    {
        return kOfxStatErrBadHandle;
    }
    
    return SDKOfxClip::releaseImage(imageHandle);
}

OfxStatus SDKOfxPluginHost::clipGetRegionOfDefinition (OfxImageClipHandle clip, OfxTime time, OfxRectD *bounds) 
{
    OfxStatus returnStat = kOfxStatErrBadHandle;
    SDKOfxClip *ofxClip = (SDKOfxClip*)(clip);
    if (ofxClip) {
        double xs, ys;
        SDKOfx::getCoordinateScaleFactors(xs, ys);
        
        NRiIPlug *ip = ofxClip->getInputPlug();
        NRiIRect ir = ip->getDod();
        bounds->x1 = ir.X1/xs;
        bounds->y1 = ir.Y1/ys;
        bounds->x2 = ir.X2/xs;
        bounds->y2 = ir.Y2/ys;
        returnStat = kOfxStatOK;
    }    
    return returnStat;
}




OfxStatus SDKOfxPluginHost::imageMemoryAlloc(OfxImageEffectHandle instanceHandle, size_t nBytes, OfxImageMemoryHandle *memoryHandle)
{
    
    *memoryHandle = (OfxImageMemoryHandle)(malloc(nBytes));
    if (*memoryHandle == 0) return kOfxStatErrMemory;
    else return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::imageMemoryFree(OfxImageMemoryHandle memoryHandle)
{
    void *vhandle = (void*)(memoryHandle);
    if (vhandle) free(vhandle);
    if (vhandle == 0) return kOfxStatErrBadHandle;
    else return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::memoryAlloc(void *handle, size_t nBytes, void **allocatedData)
{
    
    *allocatedData = malloc(nBytes);
    if (*allocatedData == 0) return kOfxStatErrMemory;
    else return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::memoryFree(void *memoryHandle)
{
    if (memoryHandle) free(memoryHandle);
    if (memoryHandle == 0) return kOfxStatErrBadHandle;
    else return kOfxStatOK;
}

struct SDKOfxThreadInfo 
{
    int nThreads;
    void *customArg;
    OfxThreadFunctionV1 *ofxFunction;
    NRiIBuf 	*oBuf;
};

static void mp_ofx(NRiThread *thread, const SDKOfxThreadInfo *mInfo)
{
    unsigned int threadNum = thread->threadId;
    unsigned int numThread = thread->nThread;
    OfxThreadFunctionV1 *f2 = mInfo->ofxFunction;
    f2(threadNum,numThread,mInfo->customArg);
}

OfxStatus SDKOfxPluginHost::multiThread(OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg)
{
    SDKOfxThreadInfo ti;
    ti.nThreads = nThreads;
    ti.customArg = customArg;
    OfxThreadFunctionV1 f2;
    ti.ofxFunction = func;
    NRiThread::m_fork((NRiThreadFunc)mp_ofx,  (void *)&ti);
    return kOfxStatOK;
}    

OfxStatus SDKOfxPluginHost::mutexCreate(const OfxMutexHandle *mutex, int lockCount)
{
    uint32_t *newLock = new uint32_t;
    *newLock = lockCount;
    mutex = (OfxMutexHandle*)(&newLock);
    return kOfxStatOK;
}

OfxStatus SDKOfxPluginHost::mutexDestroy(const OfxMutexHandle mutex)
{
    OfxStatus returnStatus = kOfxStatErrBadHandle;
    if (mutex) {
        uint32_t *lockPtr = (uint32_t*)(mutex);
        delete lockPtr;
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::mutexLock(const OfxMutexHandle mutex)
{
    OfxStatus returnStatus = kOfxStatErrBadHandle;
    if (mutex) {
        uint32_t *lockPtr = (uint32_t*)(mutex);
        NRiLock::acquire(*lockPtr);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}

OfxStatus SDKOfxPluginHost::mutexUnLock(const OfxMutexHandle mutex)
{
    OfxStatus returnStatus = kOfxStatErrBadHandle;
    if (mutex) {
        uint32_t *lockPtr = (uint32_t*)(mutex);
        NRiLock::release(*lockPtr);
        returnStatus = kOfxStatOK;
    }
    return returnStatus;
}
OfxStatus SDKOfxPluginHost::mutexTryLock(const OfxMutexHandle mutex)
{
    OfxStatus returnStatus = kOfxStatErrBadHandle;
    if (mutex) {
        uint32_t *lockPtr = (uint32_t*)(mutex);
        if ((NRiLock::attempt(*lockPtr)) == 0) returnStatus =kOfxStatFailed;
        else returnStatus = kOfxStatOK;
    }
    return returnStatus;
}


OfxStatus SDKOfxPluginHost::message(void* imageEffectHandle, const char* messageType, const char* messageID, const char* format, ...)
{
    NRiString origin;
    NRiString type(messageType);

    // If the message is associated with an effect, let's label it so.
    if(imageEffectHandle)
    {
        SDKOfxBase* pOfxInfo = (SDKOfxBase*)imageEffectHandle;
        if(pOfxInfo)
        {
            origin = pOfxInfo->getName();
        }
    }

    // The message ID may be used to pull the text for this message from an XML resource table.  We're not supporting this yet.

    // Handle the printf-style arguments at the end.
    va_list args;
    va_start(args, format);
    va_arg(args, void*);    // Advance the argument pointer.

    // Allocate room for the message.
    char* msgBuffer = (char*)calloc(8192, sizeof(char));
    sprintf(msgBuffer, format, args);
    OfxStatus result = kOfxStatOK;
    va_end(args);

    NRiEvSrc* evSrc = NRiEvSrc::getDefaultSrc();

    if(type == kOfxMessageQuestion)
    {
        // We need to show a dialog box and get a response.
        int answer = 0;
        answer = NRiModalWin::simpleModal(evSrc, origin.toCStr(), msgBuffer, " Yes ", NRiMsg::Key_Return,
                                          " No ", NRiMsg::Key_n, 0);

        result = (answer == 0 ? kOfxStatReplyYes : kOfxStatReplyNo);
    }
    else if(type == kOfxMessageLog)
    {
        if(origin != "")
        {
            origin = NRiString("\nOFX message from ") + origin + ": ";
        }
        else
        {
            origin = "\nOFX message: ";
        }

        NRiSys::error((origin + msgBuffer).toCStr());
    }
    else    // All other types of messages go to an OK dialog.
    {
//        NRiModalWin::simpleModal(evSrc, origin.toCStr(), msgBuffer, "   OK   ",	NRiMsg::Key_Return, 0);
    }

    free(msgBuffer);
    return result;
}






//******************************************************************************************************
//******************************************* SDKOfxPluginCtrl *****************************************
//******************************************************************************************************

SDKOfxPluginCtrl::SDKOfxPluginCtrl(OfxPlugin *ofxInfo, const char *path) : 
    pluginVersionMajor(ofxInfo->pluginVersionMajor), 
    pluginVersionMinor(ofxInfo->pluginVersionMinor), 
    pluginFactoryName(ofxInfo->pluginIdentifier),
    m_cObsoleteVersion(0),
    m_haveIcon(0)
{
#ifdef _DEBUG
    printf("debug: SDKOfxPluginCtrl 0x%X properySet = 0x%X   paramSet = 0x%X\n", (int)this, (int)getPropertySetHandle(), (int)getParamSetHandle());
#endif    
        
    setHost = ofxInfo->setHost; 
    mainEntry = ofxInfo->mainEntry;
    
    // create the host properties
    // Type is a host
    OfxPropertySetHandle ctrlProps = getPropertySetHandle();
    SDKOfxPluginHost::createProp(ctrlProps,kOfxPropType,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxPropType,0,kOfxTypeImageEffect);
    // Supported contexts
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropSupportedContexts,kString);
//    SDKOfxPluginHost::propSetString(ctrlProps,kOfxImageEffectPropSupportedContexts,0,kOfxImageEffectContextGenerator);
//    SDKOfxPluginHost::propSetString(ctrlProps,kOfxImageEffectPropSupportedContexts,1,kOfxImageEffectContextFilter);
//    SDKOfxPluginHost::propSetString(ctrlProps,kOfxImageEffectPropSupportedContexts,2,kOfxImageEffectContextGeneral);
    // Unique host name
    SDKOfxPluginHost::createProp(ctrlProps,kOfxPropName,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxPropName,0,ofxInfo->pluginIdentifier);
    // Effect display name - created, should be set by effect
    SDKOfxPluginHost::createProp(ctrlProps,kOfxPropLabel,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxPropLabel,0,ofxInfo->pluginIdentifier);
    // Short effect name - created, should be set by effect
    SDKOfxPluginHost::createProp(ctrlProps,kOfxPropShortLabel,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxPropShortLabel,0,ofxInfo->pluginIdentifier);
    // long effect name - created, should be set by effect
    SDKOfxPluginHost::createProp(ctrlProps,kOfxPropLongLabel,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxPropLongLabel,0,ofxInfo->pluginIdentifier);
    // effect group - created, should be set by effect
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPluginPropGrouping,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxImageEffectPluginPropGrouping,0,"");
    // only single instance allowed? 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPluginPropSingleInstance,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPluginPropSingleInstance,0,0);
    // effect thread safety 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPluginRenderThreadSafety,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxImageEffectPluginRenderThreadSafety,0,kOfxImageEffectRenderFullySafe);
    // can host do SMP threading 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPluginPropHostFrameThreading,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPluginPropHostFrameThreading,0,0);
    // overlay pointer 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPluginPropOverlayInteractV1,kPtr);
    SDKOfxPluginHost::propSetPointer(ctrlProps,kOfxImageEffectPluginPropOverlayInteractV1,0,0);
    // effect supports multiResolution 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropSupportsMultiResolution,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPropSupportsMultiResolution,0,1);
    // effect supports tiles 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropSupportsTiles,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPropSupportsTiles,0,1);
    // effect supports temporal access 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropTemporalClipAccess,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPropTemporalClipAccess,0,0);
    // effects supported depths 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropSupportedPixelDepths,kString);        // It is up to the plugin to set values on this
    // effect field rendering method 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPluginPropFieldRenderTwiceAlways,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPluginPropFieldRenderTwiceAlways,0,1);
    // effect supports multiple clip depths - set to 0 for now
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropSupportsMultipleClipDepths,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPropSupportsMultipleClipDepths,0,0);
    // effect supports multiple pixel aspect ratios 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropSupportsMultipleClipPARs,kInt);
    SDKOfxPluginHost::propSetInt(ctrlProps,kOfxImageEffectPropSupportsMultipleClipPARs,0,0);
    // parameters which trigger clip preference changes 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxImageEffectPropClipPreferencesSlaveParam,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxImageEffectPropClipPreferencesSlaveParam,0,"");
    // file path to plugin 
    SDKOfxPluginHost::createProp(ctrlProps,kOfxPluginPropFilePath,kString);
    SDKOfxPluginHost::propSetString(ctrlProps,kOfxPluginPropFilePath,0, path);
}

SDKOfxPluginCtrl::~SDKOfxPluginCtrl()
{
    // NRiHashTable destructor will take care of the properties.
    if (mainEntry)
        mainEntry(kOfxActionUnload, NULL, NULL, NULL);
}

void SDKOfxPluginCtrl::registerPlugin()
{
    setHost(&SDKOfxHost);

    // Trigger the plug-in's creation of its data structures and the use of our function suites to define its params.
    OfxStatus err;
    err = mainEntry(kOfxActionLoad, NULL, NULL, NULL);
    if (err != kOfxStatOK && err != kOfxStatReplyDefault) 
    {
        NRiSys::error("%EkOfxActionLoad failed : %d\n", err);
        return;
    }

    OfxImageEffectHandle fxHandle = (OfxImageEffectHandle)this;

    err = mainEntry(kOfxActionDescribe, fxHandle, NULL, NULL);
    if (err != kOfxStatOK && err != kOfxStatReplyDefault)
    {
        NRiSys::error("%EkOfxActionDescribe failed : %d\n", err);
        return;
    }

    NRiHashTable<SDKOfxProperty*> *sdkEffectProperties = getPropertyTable();

    char *pluginName;
    thePluginHost->propGetString((OfxPropertySetHandle)sdkEffectProperties, kOfxPropLabel, 0, &pluginName);        
    NRiString fixedPIName = NRiLex::idString(pluginName).getString();
    pluginFactoryName = NRiString(fixedPIName) + NRiString("_v") + NRiString(getPluginVersionMajor()) + NRiString("_"); 
    pluginFactory = pluginFactoryName+NRiString("()");
    
    setName(pluginFactoryName);
        
    int numContexts;
    thePluginHost->propGetDimension((OfxPropertySetHandle)sdkEffectProperties, kOfxImageEffectPropSupportedContexts, &numContexts);
    char *piContext[numContexts];
    char *theContextWeAreGoingToUse = 0;
    thePluginHost->propGetStringN((OfxPropertySetHandle)sdkEffectProperties, kOfxImageEffectPropSupportedContexts, numContexts, piContext);
    int i;
    for (i = 0; i < numContexts; i++)
    {
        if (piContext[i])
        {
            // Select from the available context types in order of preference : kOfxImageEffectContextGeneral, kOfxImageEffectContextFilter, kOfxImageEffectContextGenerator
            if (!theContextWeAreGoingToUse || 
                (strcmp(piContext[i], kOfxImageEffectContextGeneral) == 0) ||
                (strcmp(piContext[i], kOfxImageEffectContextFilter) == 0 && strcmp(theContextWeAreGoingToUse, kOfxImageEffectContextGeneral) != 0) ||
                (strcmp(piContext[i], kOfxImageEffectContextGenerator) == 0 && 
                 strcmp(theContextWeAreGoingToUse, kOfxImageEffectContextGeneral) != 0 &&
                 strcmp(theContextWeAreGoingToUse, kOfxImageEffectContextFilter) != 0) )
            {
                theContextWeAreGoingToUse = piContext[i];
            }
        }
    }
    // Finally, confirm that the seleted context is one that Shake supports
    if (!theContextWeAreGoingToUse || (strcmp(theContextWeAreGoingToUse, kOfxImageEffectContextFilter) != 0 &&
                                       strcmp(theContextWeAreGoingToUse, kOfxImageEffectContextGeneral) != 0 &&
                                       strcmp(theContextWeAreGoingToUse, kOfxImageEffectContextGenerator) != 0 ))
    {
        theContextWeAreGoingToUse = 0;
    }
    
    if (theContextWeAreGoingToUse == 0)
    {
        NRiSys::error("Aborting load of OFX plug-in %s.  Unsupported plug-in context.", getName().toCStr());
        setObsoleteVersion(true);                  // Set this to avoid loading plugin in the node menu
        return;
    }
    
    SDKOfxProperty *newProperty = new SDKOfxProperty(kString);
    *(newProperty->getNamePtr(0)) = theContextWeAreGoingToUse;
    getPropertyTable()->addEntry(NRiName(kOfxImageEffectPropContext),newProperty);    
    
    char *choice;
    SDKOfxPluginHost::propGetString((OfxPropertySetHandle)sdkEffectProperties, kOfxImageEffectPropContext, 0, &choice);
    if (strcmp(choice,kOfxImageEffectContextGenerator) == 0) {
        // If the effect is a generator manually add width, height and bytes parameters
        OfxPropertySetHandle paramProps;        
        OfxParamSetHandle paramSet;
        //        OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(&m_ofxInfo);
        OfxImageEffectHandle thisHandle = (OfxImageEffectHandle)(fxHandle);
        SDKOfxPluginHost::getParamSet(thisHandle, &paramSet);
        SDKOfxPluginHost::paramDefine(paramSet,kOfxParamTypeInteger,SDKOfxPluginHost::hostGeneratorWidthName.getString(),&paramProps);
        SDKOfxPluginHost::paramDefine(paramSet,kOfxParamTypeInteger,SDKOfxPluginHost::hostGeneratorHeightName.getString(),&paramProps);
        SDKOfxPluginHost::paramDefine(paramSet,kOfxParamTypeInteger,SDKOfxPluginHost::hostGeneratorBytesName.getString(),&paramProps);
        
        //        SDKOfxPluginHost::paramDefine(m_ctrl->getPropertySetHandle(),kOfxParamTypeInteger,"height",&paramProps);
    }
    err = mainEntry(kOfxImageEffectActionDescribeInContext, fxHandle, getPropertySetHandle(), NULL);

    if (err != kOfxStatOK && err != kOfxStatReplyDefault)
    {
        NRiSys::error("%EkOfxImageEffectActionDescribeInContext failed : %d\n", err);
        setObsoleteVersion(true);                  // Set this to avoid loading plugin in the node menu        
        return;
    }

    // Hopefully the plug-in has called the necessary parameter-suite functions to define its params,
    // so we can create the factory function.
    registerPluginFactory();
}

// Generate an icon for the plugin if one is provided in Contents/Resources
void SDKOfxPluginCtrl::buildPluginIcon()
{
    OfxPropertySetHandle ctrlProps = getPropertySetHandle();

    char *piName;
    if (kOfxStatOK == SDKOfxPluginHost::propGetString(ctrlProps,kOfxPropName,0,&piName))
    {
        char *piIconName = strrchr(piName, ':');
        if (piIconName) piIconName++;
        else piIconName = piName;
        
        // First, check to see if we already have a .nri icon in the shake icons directory
        NRiString shkIconPath;
        shkIconPath = NRiString(NRiSys::installPath()) + NRiString("icons");
        char *grouping = 0;
        SDKOfxPluginHost::propGetString(ctrlProps, kOfxImageEffectPluginPropGrouping, 0, &grouping);                
        if (!grouping || grouping[0] == 0) grouping = "OFX";
        NRiString shakeIconFile = NRiString(grouping) + NRiString(".") + NRiString(piIconName) + NRiString(".nri");
        char fileNameBuf[NRiPATH_MAX];
        const char *fName= NRiFile::findFile(fileNameBuf, shakeIconFile.toCStr(), shkIconPath.toCStr());
        if (fName) 
        {
            m_haveIcon = 1;
        }
        else                     // no icon file, try to render one if the plugin provided a .png
        {
            char *path;
            if (kOfxStatOK == SDKOfxPluginHost::propGetString(ctrlProps,kOfxPluginPropFilePath,0, &path))
            {
#ifdef macos
                NRiString iconPath = NRiString(path) + NRiString("/Contents/Resources/");
#else
                NRiString iconPath(path);
                long contentsPos = iconPath.find(NRiString("/Contents"));
                if (contentsPos != -1)
                {
                    iconPath = iconPath.substr(0, contentsPos);
                }
                iconPath += NRiString("/Contents/Resources/");
#endif
                NRiString iconFile = NRiString(piIconName) + NRiString(".png");
                const char *fName= NRiFile::findFile(fileNameBuf, iconFile.toCStr(), iconPath.toCStr());
                if (fName) 
                {
                  char *label = "";   
                  SDKOfxPluginHost::propGetString(ctrlProps, kOfxPropLabel, 0, &label);  
                    if (!NRiDir::isDirectory(shkIconPath.toCStr())) NRiSys::mkdir(shkIconPath.toCStr());
                    
                    // build a command to render :  /path/shake -fi ofxIcon.png -fit 36 27 -mmult -addshadow 2.75 -2.75 12 -move2d 20 11 -label icon -color 75 40 0.219608 0.219608 0.219608 -under icon 0 -addtext "Name" "Arial" 11 11 0 2 2 0 1 1 -fo shakeIcon.nri
                    NRiString shakeNRI = shkIconPath + NRiString("/") + shakeIconFile;
                    NRiString renderCmd = NRiString(NRiSys::executableName()) + 
                                          NRiString(" -fi \"") + NRiString(fName) + 
                                          NRiString("\" -fit 36 27 -mmult -addshadow 2.75 -2.75 12 -move2d 20 11 -label icon -color 75 40 0.219608 0.219608 0.219608 -under icon 0 -addtext \"") +
                                          NRiString(label) +
                                          NRiString("\" \"Courier\" 11 11 0 2 2 0 1 1 -fo \"") +
                                          shakeNRI + NRiString("\"");
#ifdef _DEBUG                    
                    NRiSys::error("%Drendering icon: %s\n", renderCmd.toCStr());
#endif
                    system(renderCmd.toCStr());
                    m_haveIcon = 1;                    
#ifdef _DEBUG                    
                    fName= NRiFile::findFile(fileNameBuf, shakeIconFile.toCStr(), shkIconPath.toCStr());
                    if (!fName) NRiSys::error("%EIcon render failed !!!\n");
#endif                        
                }
            }
        }
    }
}

// set up tweaker UI for the plugin.  
//This is what normally what would be in nrui.h or the UI.h plugin file for normal Shake plugins
void SDKOfxPluginCtrl::registerPluginUI()
{
    buildParameterGroups();
    buildParameterControls();
    buildPluginIcon();    
}


void SDKOfxPluginCtrl::buildParameterControls()
{
    SDKOfxParm* pCurrParam;
    
    // The only order we have to go on is the order in which the plug-in declared the params.
    // Go through the collection in that order and build the controls sequentially.
    int i;
    for(i = 0; i < m_parameters->getNbItems(); i++)
    {
        pCurrParam = (*m_parameters)[i];
        if(pCurrParam->getType() == kOfxParamTypeGroup)
        {
            buildParameterControl(getParameterGroupByName(pCurrParam->getLabel()));
        }
        else
        {
            if (pCurrParam->getName() == SDKOfxPluginHost::hostGeneratorBytesName.getString())
            {
                nuxDefRadioBtnOCtrl(7, (getPluginFactoryName() + NRiString(".") + pCurrParam->getLabel()).toCStr(), 1, 1, 0,
                                    "1|ux/num.0001", "2|ux/num.0002", "4|ux/num.0004");
            }
            else
            {
                buildParameterControl(pCurrParam);
            }
        }
    }
}


// This one handles collapsible groups of params.
void SDKOfxPluginCtrl::buildParameterControl(SDKOfxParameterList* pParamList)
{
    if(!pParamList)
        return;

    nuiPushControlGroup(pParamList->getName().toCStr());

    SDKOfxParm* pCurrParam;
    int i;
    for(i = 0; i < pParamList->getNbItems(); i++)
    {
        pCurrParam = (*pParamList)[i];
        NRiName plugName;
        pCurrParam->getPlugName(plugName);
        nuiGroupControl(plugName.getString());        
    }

    nuiPopControlGroup();
}

/// get the min ui value for the given property set at the given dimension
static NRiString getMinClampValueD(OfxPropertySetHandle propSet, int dim)
{
  double min = -DBL_MAX, displayMin = -DBL_MAX;
  SDKOfxPluginHost::propGetDouble(propSet, kOfxParamPropMin, dim, &min);
  SDKOfxPluginHost::propGetDouble(propSet, kOfxParamPropDisplayMin, dim, &displayMin);
  
  // make sure display min is not less than the legal value
  if(displayMin < min) displayMin = min;
  
  NRiString sVal;
  if(displayMin != -DBL_MAX) {
    sVal = displayMin;
  }
  else {
    sVal = globalInfiniteIPoint->asString().getString();
  }
  return sVal;
}

/// get the min ui value for the given property set at the given dimension
static NRiString getMaxClampValueD(OfxPropertySetHandle propSet, int dim)
{
  double max = DBL_MAX, displayMax = DBL_MAX;
  SDKOfxPluginHost::propGetDouble(propSet, kOfxParamPropMax, dim, &max);
  SDKOfxPluginHost::propGetDouble(propSet, kOfxParamPropDisplayMax, dim, &displayMax);
  
  // make sure display value is not more than the legal value
  if(displayMax > max) displayMax = max;

  NRiString sVal;
  if(displayMax != DBL_MAX) {
    sVal = displayMax;
  }
  else {
    sVal = globalInfiniteOPoint->asString().getString();
  }
  return sVal;
}

/// get the min ui value for the given property set at the given dimension
static NRiString getMinClampValueI(OfxPropertySetHandle propSet, int dim)
{
  int min = -INT_MAX, displayMin = -INT_MAX;
  SDKOfxPluginHost::propGetInt(propSet, kOfxParamPropMin, dim, &min);
  SDKOfxPluginHost::propGetInt(propSet, kOfxParamPropDisplayMin, dim, &displayMin);
  
  // make sure display min is not less than the legal value
  if(displayMin < min) displayMin = min;
  
  NRiString sVal;
  if(displayMin != -INT_MAX) {
    sVal = displayMin;
  }
  else {
    sVal = globalInfiniteIPoint->asString().getString();
  }
  return sVal;
}

/// get the min ui value for the given property set at the given dimension
static NRiString getMaxClampValueI(OfxPropertySetHandle propSet, int dim)
{
  int max = INT_MAX, displayMax = INT_MAX;
  SDKOfxPluginHost::propGetInt(propSet, kOfxParamPropMax, dim, &max);
  SDKOfxPluginHost::propGetInt(propSet, kOfxParamPropDisplayMax, dim, &displayMax);
  
  // make sure display value is not more than the legal value
  if(displayMax > max) displayMax = max;

  NRiString sVal;
  if(displayMax != INT_MAX) {
    sVal = displayMax;
  }
  else {
    sVal = globalInfiniteOPoint->asString().getString();
  }
  return sVal;
}

// This one handles individual params.
void SDKOfxPluginCtrl::buildParameterControl(SDKOfxParm* pParam)
{
    if(!pParam)
        return;

    NRiString paramType = pParam->getType();
    NRiName plugName;
    pParam->getPlugName(plugName);
    NRiString parmLabel = plugName.getString();    

    // Suppress the keyframe control if the parameter isn't animatable.  We have to do this before the subsequent stuff,
    // or a Shake bug will cause certain controls' tweaker types to be disregarded.
    SDKOfxProperty* pAnimates = pParam->getProperty(kOfxParamPropAnimates);
    if(pAnimates)
    {
        if(pAnimates->getValuePtr(0)->i != 1)
        {
            int i;
            for (i = 0; i < pParam->getPlugCount(); i++)
            {
                nriDefNoKeyPCtrl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(i)).toCStr());
            }
        }
    }

    if(paramType == kOfxParamTypeRGB ||
       paramType == kOfxParamTypeRGBA)
    {
        // First we need to make a control group for the three color elements.
        nuiPushControlGroup(parmLabel.toCStr());
        nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr());
        nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr());
        nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr());
        nuiPopControlGroup();
        
        // Now attach the group to our three-color control.
        nuiPushControlWidget(parmLabel.toCStr(), "nuiConnectColorTriplet(1, 0, 1)");
    }
    else if (paramType == kOfxParamTypePushButton)
    {
        nuxDefButtonCtrl((getPluginFactoryName() + NRiString(".") + parmLabel).toCStr());
    }
    else if (paramType == kOfxParamTypeBoolean)
    {
        nuxDefExprToggle(2, (getPluginFactoryName() + NRiString(".") + parmLabel).toCStr());
    }
    else if (paramType == kOfxParamTypeChoice)
    {
        NRiString choices;
        
        int idx;
        int numChoices;
        thePluginHost->propGetDimension(pParam->getPropertySetHandle(), kOfxParamPropChoiceOption, &numChoices);
        for (idx = 0; idx < numChoices; idx++)
        {
            char *choice;
            thePluginHost->propGetString(pParam->getPropertySetHandle(), kOfxParamPropChoiceOption, idx, &choice);
            choices.addItem(choice);
        }
        
        nuxDefMultiChoice((getPluginFactoryName() + NRiString(".") + parmLabel).toCStr(), choices.toCStr());
    }
    else if(paramType == kOfxParamTypeDouble ||
            paramType == kOfxParamTypeDouble2D ||
            paramType == kOfxParamTypeDouble3D)
    {
        NRiString sValMin;        
        NRiString sValMax;
        sValMin = getMinClampValueD((OfxPropertySetHandle)pParam->getPropertySetHandle(), 0);
        sValMax = getMaxClampValueD((OfxPropertySetHandle)pParam->getPropertySetHandle(), 0);

        nuiDefSlider((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr(), 
                     sValMin == globalInfiniteIPoint->asString().getString() ? globalInfiniteIPoint->asString().getString() : sValMin.toCStr(),    // work around a shake bug, need the pointer to the global string if inf.
                     sValMax == globalInfiniteOPoint->asString().getString() ? globalInfiniteOPoint->asString().getString() : sValMax.toCStr());

        if(paramType == kOfxParamTypeDouble2D || paramType == kOfxParamTypeDouble3D)
        {
            sValMin = getMinClampValueD((OfxPropertySetHandle)pParam->getPropertySetHandle(), 1);
            sValMax = getMaxClampValueD((OfxPropertySetHandle)pParam->getPropertySetHandle(), 1);
            nuiDefSlider((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr(), 
                         sValMin == globalInfiniteIPoint->asString().getString() ? globalInfiniteIPoint->asString().getString() : sValMin.toCStr(),    // work around a shake bug, need the pointer to the global string if inf.
                         sValMax == globalInfiniteOPoint->asString().getString() ? globalInfiniteOPoint->asString().getString() : sValMax.toCStr());
        }

        if(paramType == kOfxParamTypeDouble3D)
        {
            sValMin = getMinClampValueD((OfxPropertySetHandle)pParam->getPropertySetHandle(), 2);
            sValMax = getMaxClampValueD((OfxPropertySetHandle)pParam->getPropertySetHandle(), 2);
            nuiDefSlider((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr(), 
                         sValMin == globalInfiniteIPoint->asString().getString() ? globalInfiniteIPoint->asString().getString() : sValMin.toCStr(),    // work around a shake bug, need the pointer to the global string if inf.
                         sValMax == globalInfiniteOPoint->asString().getString() ? globalInfiniteOPoint->asString().getString() : sValMax.toCStr());
        }

        // Build the special 2- or 3-D control if there are multiple values.
        if(paramType == kOfxParamTypeDouble2D || paramType == kOfxParamTypeDouble3D)
        {
            nuiPushControlGroup(parmLabel.toCStr());
            nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr());
            nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr());
            if(paramType == kOfxParamTypeDouble3D)
            {
                nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr());
            }
            nuiPopControlGroup();
            nuiPushControlWidget(parmLabel.toCStr(), "nuiConnectXYZTriplet()");
            
            nuiDefTweakerNumCol(parmLabel.toCStr(), NRiString(3 + (paramType == kOfxParamTypeDouble3D ? 1 : 0)).toCStr());
            nuiDefPCtrlLoc((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr(), "2");
            nuiDefPCtrlLoc((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr(), "2");
            if(paramType == kOfxParamTypeDouble3D)
            {
                nuiDefPCtrlLoc((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr(), "2");
            }
        }
    }
    else if(paramType == kOfxParamTypeInteger ||
            paramType == kOfxParamTypeInteger2D ||
            paramType == kOfxParamTypeInteger3D)
    {
        NRiString sValMin;        
        NRiString sValMax;
        sValMin = getMinClampValueI((OfxPropertySetHandle)pParam->getPropertySetHandle(), 0);
        sValMax = getMaxClampValueI((OfxPropertySetHandle)pParam->getPropertySetHandle(), 0);
        
        nuiDefSlider((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr(), 
                     sValMin == globalInfiniteIPoint->asString().getString() ? globalInfiniteIPoint->asString().getString() : sValMin.toCStr(),    // work around a shake bug, need the pointer to the global string if inf.
                     sValMax == globalInfiniteOPoint->asString().getString() ? globalInfiniteOPoint->asString().getString() : sValMax.toCStr());
        
        if(paramType == kOfxParamTypeInteger2D || paramType == kOfxParamTypeInteger3D)
        {            
            sValMin = getMinClampValueI((OfxPropertySetHandle)pParam->getPropertySetHandle(), 1);
            sValMax = getMaxClampValueI((OfxPropertySetHandle)pParam->getPropertySetHandle(), 1);
          
            nuiDefSlider((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr(), 
                         sValMin == globalInfiniteIPoint->asString().getString() ? globalInfiniteIPoint->asString().getString() : sValMin.toCStr(),    // work around a shake bug, need the pointer to the global string if inf.
                         sValMax == globalInfiniteOPoint->asString().getString() ? globalInfiniteOPoint->asString().getString() : sValMax.toCStr());
        }
        
        if(paramType == kOfxParamTypeInteger3D)
        {            
            sValMin = getMinClampValueI((OfxPropertySetHandle)pParam->getPropertySetHandle(), 1);
            sValMax = getMaxClampValueI((OfxPropertySetHandle)pParam->getPropertySetHandle(), 1);
          
            nuiDefSlider((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr(), 
                         sValMin == globalInfiniteIPoint->asString().getString() ? globalInfiniteIPoint->asString().getString() : sValMin.toCStr(),    // work around a shake bug, need the pointer to the global string if inf.
                         sValMax == globalInfiniteOPoint->asString().getString() ? globalInfiniteOPoint->asString().getString() : sValMax.toCStr());
        }

        // Build the special 2- or 3-D control if there are multiple values.
        if(paramType == kOfxParamTypeInteger2D || paramType == kOfxParamTypeInteger3D)
        {
            nuiPushControlGroup(parmLabel.toCStr());
            nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr());
            nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr());
            if(paramType == kOfxParamTypeInteger3D)
            {
                nuiGroupControl((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr());
            }
            nuiPopControlGroup();
            nuiPushControlWidget(parmLabel.toCStr(), "nuiConnectXYZTriplet()");
            
            nuiDefTweakerNumCol(parmLabel.toCStr(), NRiString(3 + (paramType == kOfxParamTypeInteger3D ? 1 : 0)).toCStr());
            nuiDefPCtrlLoc((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(0)).toCStr(), "2");
            nuiDefPCtrlLoc((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(1)).toCStr(), "2");
            if(paramType == kOfxParamTypeInteger3D)
            {
                nuiDefPCtrlLoc((getPluginFactoryName() + NRiString(".") + parmLabel + pParam->getSuffix(2)).toCStr(), "2");
            }
        }
    }
}


void SDKOfxPlugin::buildParameterGroups()
{
    SDKOfxParm* pCurrParam;
    NRiString groupName, parentName;
    SDKOfxParameterList* pParentGroup;
    SDKOfxParameterList* pCurrGroup;
    NRiPArray<SDKOfxParameterList> paramGroupsToAdd;
    
    int i;
    for(i = 0; i < m_parameters->getNbItems(); i++)
    {
        parentName = "";
        pCurrParam = (*m_parameters)[i];
        if(pCurrParam->getType() == kOfxParamTypeGroup)
        {
            // Create the parameter list that'll serve as the group.
            pCurrGroup = new SDKOfxParameterList;
            pCurrGroup->setName(pCurrParam->getName());     // NOTE: Using name here, not label, assuming that the plug-in will
                                                            // refer to groups by their names.
            
            // Find out if there's a parent group.
            parentName = pCurrParam->getParentGroupName();
            if(parentName != "")
            {
                pParentGroup = getParameterGroupByName(parentName);
                if(pParentGroup)
                {
                    pParentGroup->addParameterList(pCurrGroup);
                }
                else
                {
                    pCurrGroup->setParentName(parentName);
                    paramGroupsToAdd.append(pCurrGroup);
                }
            }
            else    // It has no parent.
            {
                m_parameterGroups.append(pCurrGroup);
            }
        }
    }
    
    // Now reconcile the groups whose parents didn't yet exist in the list.
    // This may never happen, but we have to be thorough, don't we?
    int currLoop = 0;
    
    // We should have to go through this, at most, as many times as there are entries.
    int maxLoops = paramGroupsToAdd.getNbItems();
    while(currLoop < maxLoops)
    {
        for(i = 0; i < paramGroupsToAdd.getNbItems(); i++)
        {
            pCurrGroup = paramGroupsToAdd[i];
            pParentGroup = getParameterGroupByName(pCurrGroup->getParentName());
            if(pParentGroup)
            {
                pParentGroup->addParameterList(pCurrGroup);
                paramGroupsToAdd.remove(pCurrGroup);
            }
        }
        
        currLoop++;
    }
    
    // Now go through and put parameters in their groups.
    for(i = 0; i < m_parameters->getNbItems(); i++)
    {
        parentName = "";
        pCurrParam = (*m_parameters)[i];
        if(pCurrParam->getType() != kOfxParamTypeGroup)
        {
            // Find out if there's a parent group.
            parentName = pCurrParam->getParentGroupName();
            if(parentName != "")
            {
                pParentGroup = getParameterGroupByName(parentName);
                if(pParentGroup)
                {
                    pParentGroup->addParameter(pCurrParam);
                }
                else
                {
                    NRiSys::error("Parameter %s's parent group, %s, not found.\n", pCurrParam->getName().toCStr(), parentName.toCStr());
                }
            }
        }
    }
}


SDKOfxParameterList* SDKOfxPlugin::getParameterGroupByName(const NRiString& name)
{
    int i;
    SDKOfxParameterList* pTargetGroup;
    for(i = 0; i < m_parameterGroups.getNbItems(); i++)
    {
        if(m_parameterGroups[i]->getName() == name)
        {
            return m_parameterGroups[i];
        }
        else    // Find out if the requested group is a child group of the current one.
        {
            pTargetGroup = m_parameterGroups[i]->getChildListByName(name);
            if (pTargetGroup)
            {
                return pTargetGroup;
            }
        }
    }
    
    return 0;
}


bool SDKOfxPlugin::addClip(SDKOfxClip* pClip)
{
    m_clips.append(pClip);
    return true;
}


SDKOfxClip* SDKOfxPlugin::getClipByName(const NRiString& name)
{
    int i;
    for(i = 0; i < m_clips.getNbItems(); i++)
    {
        if(m_clips[i]->getName() == name)
        {
            return m_clips[i];
        }
    }

    return 0;
}


// Create a factory function for the plug-in.
void SDKOfxPluginCtrl::registerPluginFactory()
{
    NRiString factoryStart = NRiString("image ") + getPluginFactoryName() + NRiString("(");
    NRiString parameterLine;
    SDKOfxParameterList *parameters = getParameterList();
    SDKOfxParm *parameter;
    NRiString  parmLabel;
    SDKOfxProperty* pProperty;
    
    NRiPArray<SDKOfxClip> *clips = getClips();
    SDKOfxClip *clip;
    int afterFirst = 0;
    
    int i;
    for ( i = 0; i < clips->getNbItems(); i++)
    {
        clip = (*clips)[i];
        parmLabel = clip->getName();
        if (parmLabel != "ofx_Output")
        {
            parameterLine += NRiString("image ") + parmLabel + NRiString(" = 0,");
        }
    }
    for ( i = 0; i < parameters->getNbItems(); i++)
    {
        parameter = (*parameters)[i];
        NRiName plugName;
        parameter->getPlugName(plugName);
        parmLabel = plugName.getString();        
        
        if (parameter->getPlugType() == kInvalid)
            continue;

        // Non-persistent parameters don't get serialized and thus have no place in a factory function.
        pProperty = parameter->getProperty(kOfxParamPropPersistant);
        if(pProperty)
        {
            if(pProperty->getValuePtr(0)->i != 1)
            {
                continue;
            }
        }
        
//        int notSecret;
//        SDKOfxPluginHost::propGetInt((OfxPropertySetHandle)parameter->getPropertySetHandle(), kOfxParamPropSecret, 0, &notSecret);
//        if (notSecret == 0) continue;                              // Secret parameters don't get serialized
        
        if (afterFirst)
            parameterLine += NRiString(", ");
        
        afterFirst = 1;

        OfxPropertySetHandle propertySet = parameter->getPropertySetHandle();

        if(parameter->getType() == kOfxParamTypeInteger ||
           parameter->getType() == kOfxParamTypeBoolean ||
           parameter->getType() == kOfxParamTypePushButton )
        {
            parameterLine += NRiString("int ") + parmLabel;
            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                // Override the default values for generator parameters we have added
                if (parameter->getName() == SDKOfxPluginHost::hostGeneratorWidthName.getString()) defVal = "GetDefaultWidth()";
                if (parameter->getName() == SDKOfxPluginHost::hostGeneratorHeightName.getString()) defVal = "GetDefaultHeight()";
                if (parameter->getName() == SDKOfxPluginHost::hostGeneratorBytesName.getString()) defVal = "GetDefaultBytes()";
                parameterLine += NRiString(" = ") + defVal;
            }
        }
        else if(parameter->getType() == kOfxParamTypeInteger2D)
        {
            parameterLine += NRiString("int ") + parmLabel + parameter->getSuffix(0);
            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", int ") + parmLabel + parameter->getSuffix(1);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 1, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
                    
        }
        else if(parameter->getType() == kOfxParamTypeInteger3D)
        {
            parameterLine += NRiString("int ") + parmLabel + parameter->getSuffix(0);
            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", int ") + parmLabel + parameter->getSuffix(1);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 1, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", int ") + parmLabel + parameter->getSuffix(2);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 2, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
        }
        else if(parameter->getType() == kOfxParamTypeDouble)
        {
            parameterLine += NRiString("float ") + parmLabel;

            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                // Find out whether this parameter is "normalized" to image dimensions.
                SDKOfxProperty* pDoubleType = parameter->getProperty(kOfxParamPropDoubleType);
                NRiValue* pVal;
                NRiString theType;
                
                if(pDoubleType)
                {
                    pVal = pDoubleType->getValuePtr(0);
                    theType = pVal->n;
                    if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                    {
                        parameterLine += NRiString(" = \"") + NRiString(defVal) + " * width\"";
                    }
                    else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                    {
                        parameterLine += NRiString(" = \"") + NRiString(defVal) + " * height\"";
                    }
                    else
                    {
                        parameterLine += NRiString(" = ") + defVal;
                    }
                }
            }
        }
        else if(parameter->getType() == kOfxParamTypeDouble2D)
        {
            // Find out whether this parameter is "normalized" to image dimensions.
            SDKOfxProperty* pDoubleType = parameter->getProperty(kOfxParamPropDoubleType);
            NRiValue* pVal;
            NRiString theType;
            
            if(pDoubleType)
            {
                pVal = pDoubleType->getValuePtr(0);
                theType = pVal->n;
            }

            parameterLine += NRiString("float ") + parmLabel + parameter->getSuffix(0);
            NRiString defVal;
            if( thePluginHost->propGetAsString(propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                if(pDoubleType && (theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute))
                {
                    // We're on the first value, so that should be X.
                    parameterLine += NRiString(" = \"") + NRiString(defVal) + " * width\"";
                }
                else
                {
                    parameterLine += NRiString(" = ") + defVal;
                }
            }

            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(1);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 1, defVal) == kOfxStatOK)
            {
                if(pDoubleType && (theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute))
                {
                    // We're on the second value, so that should be Y.
                    parameterLine += NRiString(" = \"") + NRiString(defVal) + " * height\"";
                }
                else
                {
                    parameterLine += NRiString(" = ") + defVal;
                }
            }
        }
        else if(parameter->getType() == kOfxParamTypeDouble3D)
        {
            parameterLine += NRiString("float ") + parmLabel + parameter->getSuffix(0);
            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(1);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 1, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(2);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 2, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
        }
        else if(parameter->getType() == kOfxParamTypeRGB)
        {
            parameterLine += NRiString("float ") + parmLabel + parameter->getSuffix(0);
            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(1);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 1, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(2);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 2, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
        }
        else if(parameter->getType() == kOfxParamTypeRGBA)
        {
            parameterLine += NRiString("float ") + parmLabel + parameter->getSuffix(0);
            NRiString defVal;
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(1);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 1, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(2);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 2, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
            parameterLine += NRiString(", float ") + parmLabel + parameter->getSuffix(3);
            if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 3, defVal) == kOfxStatOK)
            {
                parameterLine += NRiString(" = ") + defVal;
            }
        }
        else if(parameter->getType() == kOfxParamTypeString ||
                parameter->getType() == kOfxParamTypeCustom ||
                parameter->getType() == kOfxParamTypeChoice)
        {
            parameterLine += NRiString("const char* ") + parmLabel;
            if (parameter->getType() == kOfxParamTypeChoice)
            {
                int defIndex;
                if( thePluginHost->propGetInt (propertySet, kOfxParamPropDefault, 0, &defIndex) == kOfxStatOK)
                {
                    int numChoices;
                    thePluginHost->propGetDimension(propertySet, kOfxParamPropChoiceOption, &numChoices);
                    if (defIndex < numChoices)
                    {
                        char *choice;
                        thePluginHost->propGetString(propertySet, kOfxParamPropChoiceOption, defIndex, &choice);
                        parameterLine += NRiString(" = \"") + NRiString(choice) + NRiString("\"");
                    }
                }
                
            }
            else
            {
                NRiString defVal;
                if( thePluginHost->propGetAsString (propertySet, kOfxParamPropDefault, 0, defVal) == kOfxStatOK)
                {
                    parameterLine += NRiString(" = \"") + defVal + NRiString("\"");
                }
            }
        }

        // Types that we don't do anything with, because they're for display purposes only:
        // kOfxParamTypeGroup, kOfxParamTypePage, kOfxParamTypePushButton
    }

    parameterLine += NRiString(")");
    
    NRiString factorySource;
    
    factorySource = NRiString("{ fx1 = OfxPluginFactory(");
    factorySource += getPluginFactoryName();
    factorySource += NRiString(");");    
    factorySource += NRiString("return fx1; }");
    
    NRiString source = factoryStart + parameterLine + factorySource;
    
    NRiScript *script = (NRiScript *)currentScript->asPtr();
    assert(script);
    
    NRiEval * eval = new NRiEval(script->getScope(), -1);
    NRiNode::pushRoot(script);
    NRiValue val;
    eval->setType(kVoid);
    eval->setSource(source.toCStr(), "OfxPluginLoader");
    eval->eval(val);
    NRiNode::popRoot();
#ifdef _DEBUG
    NRiSys::error("Registered plug-in factory :\n%s\n", source.toCStr());
#endif    
    delete eval;
}


class OFXTweak: public NRiNodeUI
{
public:
    OFXTweak(NRiNode*,NRiName);
    virtual ~OFXTweak();    
};

OFXTweak::OFXTweak(NRiNode* node, NRiName tname) : NRiNodeUI(node, tname)
{
    gIScrubRefCnt->set(gIScrubRefCnt->asInt()+1);                           // Tells the viewer that we may be access input pixel data, buffer it
}

OFXTweak::~OFXTweak()
{
    gIScrubRefCnt->set(gIScrubRefCnt->asInt()-1);
}

// Implement a custom tweaker builder.
// The reason that we use this instead of just letting the Shake call the default NRiTweekReg::makeTweek is that the ofx plugin is housed in a NRiGrpFx node.
// We must tell the grpFx node to build its internals (which constructs the ofx plugin node) before tweaking it.
// Once that is done we can hide all secret and custom parameter so they don't get displayed.
static NRiTweek* ofxMakeTweak(
                              NRiNode* node         , 
                              const NRiName &twname , 
                              NRiTweek* vtweak      , 
                              int addName
                              )
{
    if(vtweak==0) vtweak= new OFXTweak(node,twname);

    NRiPArray<NRiPlug> internalPlugs;
    SDKOfxEffect *ofxFx = 0;

    NRiGrpFx* grpFx = dynamic_cast<NRiGrpFx *>(node);    
    if (grpFx) 
    {
        grpFx->buildBody();                                                   // create the internal SDKOfxEffect, if it doesn't already exist

        int numChildren = grpFx->getNbChildren();
        int i;
        for (i = 0; i < numChildren; i++)
        {
            ofxFx = dynamic_cast<SDKOfxEffect *> (grpFx->getNthChild(i));
            if (ofxFx) 
            {
                ofxFx->internalizeSecretStuff(internalPlugs);                   // hide secret parameters

                // Set up ranges for "normalized" controls.
                NRiString nodeName = grpFx->className().getString(); //  grpFx->getName().getString();
                NRiName plugName;
                NRiString fullPlugName;
                SDKOfxParameterList* pParams = ofxFx->getParameterList();
                SDKOfxParm* pCurrParam;
                int x;
                for(x = 0; x < pParams->getNbItems(); x++)
                {
                    pCurrParam = (*pParams)[x];
                    if(pCurrParam->getPlugType() == kFloat)
                    {
                        // OFX allows several kinds of "normalized" values, which translate 0 - 1 into
                        // a broader range.  See if this is one.
                        SDKOfxProperty* pDoubleType = pCurrParam->getProperty(kOfxParamPropDoubleType);
                        NRiValue* pVal;
                        NRiString theType;
                        
                        if(pDoubleType && ofxFx)
                        {
                            pVal = pDoubleType->getValuePtr(0);
                            theType = pVal->n;
                            if(theType == kOfxParamDoubleTypeNormalisedX || theType == kOfxParamDoubleTypeNormalisedXAbsolute)
                            {
                                pCurrParam->getPlugName(plugName, 0);   // First plug, should be X.
                                fullPlugName = nodeName + NRiString(".") + plugName.getString();
                                nuiDefSlider(fullPlugName.toCStr(), "0", NRiString(ofxFx->out->width()->asFloat()).toCStr(), "1");   // The last param is granularity.
                            }
                            else if(theType == kOfxParamDoubleTypeNormalisedY || theType == kOfxParamDoubleTypeNormalisedYAbsolute)
                            {
                                pCurrParam->getPlugName(plugName, 0);   // First plug, should be Y.
                                fullPlugName = nodeName + NRiString(".") + plugName.getString();
                                nuiDefSlider(fullPlugName.toCStr(), "0", NRiString(ofxFx->out->height()->asFloat()).toCStr(), "1");   // The last param is granularity.
                            }
                            else if(theType == kOfxParamDoubleTypeNormalisedXY || theType == kOfxParamDoubleTypeNormalisedXYAbsolute)
                            {
                                pCurrParam->getPlugName(plugName, 0);   // First plug, should be X.
                                fullPlugName = nodeName + NRiString(".") + plugName.getString();
                                nuiDefSlider(fullPlugName.toCStr(), "0", NRiString(ofxFx->out->width()->asFloat()).toCStr(), "1");   // The last param is granularity.
                                
                                pCurrParam->getPlugName(plugName, 1);   // Second plug, should be Y.
                                fullPlugName = nodeName + NRiString(".") + plugName.getString();
                                nuiDefSlider(fullPlugName.toCStr(), "0", NRiString(ofxFx->out->height()->asFloat()).toCStr(), "1");   // The last param is granularity.
                            }
                        }
                    }
                }

                break;
            }
        }
    }

    /*
     * This will find the parent class name of node
     * and build a generic Node tweaker.
     */
    vtweak = (NRiTweekReg::makeTweek(
                                   NRiClassName::findParent( node->className() ),
                                   node  ,
                                   twname,
                                   vtweak,
                                   addName ));

    OFXTweak *tweak= dynamic_cast<OFXTweak*>(vtweak);
    if(tweak==0) return vtweak;

    if (ofxFx) ofxFx->externalizeSecretStuff(internalPlugs);                   // re-expose the hidden parameters (so they get serialized)

    return tweak;
}


typedef int (*numPluginFunc)();    
typedef OfxPlugin* (*getPluginFunc)(int);    

extern "C" {
    
    NRiExport NRiIPlug *OfxPluginFactory(int n, const char* p1...)
    {
        if(p1[0] == ':' )
        {
            if (p1[1] == ' ')  p1 += 2;
            else p1++;
        }
        SDKOfxPluginCtrl *plugin = SDKOfxPluginHost::getPluginByFactoryName(p1);
        n--;
        
        if (!plugin)
        {
            NRiSys::error("%E Failed to find plug-in %s\n", p1);
            return 0;
        }
        
        SDKOfxEffect *fx = new SDKOfxEffect();
        fx->setPluginCtrl(plugin);
        NRiPArray<SDKOfxParm> lockedParms;

        SDKOfxParameterList *parms = fx->getParameterList();

        int parmIdx = 0;
        int nParams = parms->getNbItems();
/*        
        NRiPArray<SDKOfxClip> *clips = plugin->getClips();
        SDKOfxClip *clip;
        int i;
        NRiString parmLabel;
        for ( i = 0; i < clips->getNbItems(); i++)
        {
            clip = (*clips)[i];
            parmLabel = clip->getName();
            if (parmLabel != "ofx_Output") {
                NRiIPlug *createdIPlug = fx->createInput(parmLabel.toCStr());
                clip->setInputPlug(createdIPlug);
            } else {
                NRiIPlug *createdIPlug = fx->createOutput(parmLabel.toCStr());
                clip->setInputPlug(createdIPlug);
            }                
        }
*/        
        va_list    arg;
        va_start(arg, p1);

        while(n > 0)
        {
            SDKOfxParm *parm = (*parms)[parmIdx++];
            int numValsForParm = parm->getNumValues();

            int i;
            for (i = 0; i < numValsForParm; i++)
            {
                const char* plugExpr = va_arg(arg, const char*);
            
                int stripColon = 1;
                while (parm->getType() == kImage && parmIdx < nParams)
                {
                    parm = (*parms)[parmIdx++];                
                }
                if (parm->getType() == kImage)
                {
                    NRiSys::error("%E Can't find parameter to OFX plug-in given an input parameter for %s\n", p1);
                    return 0;
                }
            
                if (parm->getType() == kOfxParamTypeString ||
                    parm->getType() == kOfxParamTypeCustom ||
                    parm->getType() == kOfxParamTypeChoice )
                {
                    stripColon = 0;
                }
                
                // get each of the plugin's plugs and set it to the argument value
                if(plugExpr == 0)
                {
                    plugExpr = "0";
                }
                else if(stripColon)
                {
                    if(plugExpr[0] == ':' && plugExpr[1] == ' ')
                    {
                        plugExpr += 2;
                    }
                    else if(plugExpr[0] == ':')
                    {
                        plugExpr++;
                    }
                }

                // Find the plug corresponding to this parameter and set it to plugExpr
                NRiName plugName;
                parm->getPlugName(plugName, i);
                NRiPlug *plug = fx->getPlug(plugName);
                assert(plug);
                fx->setPlugDelayed(plug, plugExpr);
#ifdef _DEBUG                
                NRiSys::error("%DSetting parameter %s to %s\n", plugName.getString(), plugExpr);
#endif
                n--;
            }
        }

        // Connect the image inputs
        NRiNode *parent = NRiNode::getRoot();
        NRiGrpFx *grpFx = dynamic_cast<NRiGrpFx *>(parent);
	fx->setParent(parent);
        if (grpFx)
        {
            int nInputs = plugin->getNbInputs();
            int i;
            for (i = 0; i < nInputs; i++)
            {
                NRiPlug* pinput = grpFx->getInput(i);        
                NRiPlug* input = fx->getInput(i);
                if (input && pinput) input->connect(pinput);
            }
        }

        fx->setPlugDelayed(fx->m_doneInitializing, 1);
	return fx->out;
    }
    
    void OfxLoadPlugins()
    {
        if (!thePluginHost) thePluginHost = new SDKOfxPluginHost;

        NRiPArray<const char> *pluginFiles  = (NRiPArray<const char> *)gp_loadedOfxPluginFiles->asPtr();
        if (pluginFiles) {
            int i,n= pluginFiles->getLength();
            for(i=0; i<n; ++i)
            {
                const char *name = (*pluginFiles)[i];
#ifdef _DEBUG                
                NRiSys::error("loading plug-in bundle %s \n",name);
#endif                
                numPluginFunc npf;
                getPluginFunc gpf;
                NRiSys::loadDso(name);
                void *handle = NRiSys::findSymbol("OfxGetNumberOfPlugins",name);
                if (handle) {
                    npf = (numPluginFunc)(handle);
                    handle = NRiSys::findSymbol("OfxGetPlugin",name);
                    gpf = (getPluginFunc)(handle);
                    if (gpf) {
                        int i,numPlugins = npf();
                        for (i=0;i<numPlugins;i++) {
                            // TODO add testing of all plugins for supported features and duplicates
                            // this includes the generic load and query of the plugin descriptor
                            // and setting the host info for the plugin
                            OfxPlugin *op = gpf(i);
                            if (op) {	
                                SDKOfxPluginHost::addPlugin(op,name);
                            }
                        }
                    }
                }
            }
            gp_ofxPlugins->set(&ofxPluginList);
            int nPlugins = ofxPluginList.getNbItems();
            for (i = 0; i < nPlugins; i++)
            {
                SDKOfxPluginCtrl *pi = ofxPluginList[i];
                pi->registerPlugin();                
            }
        }
    }

    void OfxLoadToolbox()
    {
        NRiPArray<SDKOfxPluginCtrl> *ofxPluginList = (NRiPArray<SDKOfxPluginCtrl> *)gp_ofxPlugins->asPtr();
        int i;
        for (i = 0; i < ofxPluginList->getNbItems(); i++)
        {
            SDKOfxPluginCtrl *ofxPI = (*ofxPluginList)[i];
            if (!ofxPI->isObsoleteVersion())
            {
                ofxPI->registerPluginUI();                                                         // set up tweaker UI for the plugin.  This is what normally what would be in nrui.h or the UI.h plugin file for normal Shake plugins
  
                char *grouping = 0;              
                
                SDKOfxPluginHost::propGetString(ofxPI->getPropertySetHandle(), kOfxImageEffectPluginPropGrouping, 0, &grouping);  
                if (!grouping || grouping[0] == 0) grouping = "OFX";
                
                char piName[256];
                if (ofxPI->getHaveIcon())
                {
                    char *pluginName;
                    SDKOfxPluginHost::propGetString(ofxPI->getPropertySetHandle(), kOfxPropName, 0, &pluginName);
                    char *piIconName = strrchr(pluginName, ':');
                    if (piIconName) piIconName++;
                    else piIconName = pluginName;
                    strcpy(piName, piIconName);
                }
                else
                {
                    sprintf(piName, "@%s", ofxPI->getName().toCStr());
                }
                nuiPushMenu("Tools");
                nuiPushToolBox(grouping);
                nuiToolBoxItem(piName, ofxPI->getPluginFactory().toCStr(), 0);
                nuiPopToolBox();
                nuiPopMenu();
            }
            NRiTweekReg::registerTweekFunc( ofxPI->getPluginFactoryName().toCStr(), &ofxMakeTweak );  // register a custom tweaker for the plugin (see ofxMakeTweak above)
        }
    }
    
}

