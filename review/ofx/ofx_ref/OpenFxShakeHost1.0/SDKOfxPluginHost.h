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
* $Header: /cvsroot/openfxshakehost/OFX/SDKOfxPluginHost.h,v 1.2 2006/09/12 19:51:07 dcandela Exp $
*
*
*/

#ifndef __SDKOfxPluginHost_H__
#define __SDKOfxPluginHost_H__

#include <NRiHashTable.h>
#include <NRiPArray.h>
#include <NRiPlug.h>
#include <NRiString.h>
#include <NRiThread.h>
#include <NRiValue.h>

#include <ofxCore.h>
#include <ofxProperty.h>
#include <ofxImageEffect.h>

#include <SDKOfxUtility.h>
#include <SDKOfxOverlay.h>

#define kImage 0xFF00

class SDKOfxPlugin : public SDKOfxBase
{
public:
    SDKOfxPlugin()          { m_parameters = new SDKOfxParameterList(this);                           }
    virtual ~SDKOfxPlugin() { delete m_parameters; m_clips.deleteAllEntries();   /* TODO - delete all of the parameter instances and groups*/ }
    
    bool addParameter(SDKOfxParm *newParam) { return m_parameters->addParameter(newParam);  }
    
    OfxParamSetHandle               getParamSetHandle() { return (OfxParamSetHandle)m_parameters;  }
    SDKOfxParameterList*            getParameterList()  { return  m_parameters; }
    void                            addParameterGroup(SDKOfxParameterList* pGroup);
    NRiPArray<SDKOfxParameterList>* getParameterGroups() {return &m_parameterGroups;}
    SDKOfxParameterList*            getParameterGroupByName(const NRiString& name);
    void                            buildParameterGroups();

    const int               getNbInputs()          { return m_clips.getLength();}        
    NRiPArray<SDKOfxClip>*  getClips() {return &m_clips;}
    bool                    addClip(SDKOfxClip*);
    SDKOfxClip*             getClipByName(const NRiString&);

    
protected:
    SDKOfxParameterList *m_parameters;
    NRiPArray<SDKOfxParameterList> m_parameterGroups;   // Params can be organized into groups for display.

private:    
    NRiPArray<SDKOfxClip> m_clips;
};


// This class is instantiated once for each plug-in type supported (each version of the plugin will get one of these)
class SDKOfxPluginCtrl : public SDKOfxPlugin
{
public:
    SDKOfxPluginCtrl(OfxPlugin *, const char *);
    virtual ~SDKOfxPluginCtrl();
    
    void        registerPlugin();    
    void        registerPluginUI();
    void        buildParameterControls();
    void        buildParameterControl(SDKOfxParameterList* pParamList);
    void        buildParameterControl(SDKOfxParm* pParam);
    void        buildPluginIcon();    

    OfxPluginEntryPoint    *mainEntry;

    unsigned int    getPluginVersionMajor() {return pluginVersionMajor;}
    void            setPluginVersionMajor(unsigned int v) {pluginVersionMajor = v;}
    unsigned int    getPluginVersionMinor() {return pluginVersionMinor;}
    void            setPluginVersionMinor(unsigned int v) {pluginVersionMinor = v;}
    const NRiString&    getPluginFactoryName() {return pluginFactoryName;}
    void                setPluginFactoryName(const NRiString& name) {pluginFactoryName = name;}
    const NRiString&    getPluginFactory() {return pluginFactory;}
    void                setPluginFactory(const NRiString& factory) {pluginFactory = factory;}
    char            isObsoleteVersion() {return m_cObsoleteVersion;}
    char            getHaveIcon()       {return m_haveIcon; }
    void            setObsoleteVersion(char flag) {m_cObsoleteVersion = flag;}

private:    
        
    void            registerPluginFactory();    

    void            (*setHost)(OfxHost *host);

    unsigned int pluginVersionMajor;
    unsigned int pluginVersionMinor;    
    NRiString    pluginFactoryName;
    NRiString    pluginFactory;
    char         m_cObsoleteVersion;
    char         m_haveIcon;
};


// This is our main class for hosting plugins.  Only one instance of this is created for the appliction
class SDKOfxPluginHost : public SDKOfxBase
{
public:
    
    SDKOfxPluginHost();
    ~SDKOfxPluginHost();
    static      int pluginsInitialized;
    static      void loadOfxPlugins();

    static void addPlugin(OfxPlugin *, const char *);
    static int  getPluginIdx(const char *piname);                       // given plug-in name find its index in the list, -1 if not found
    static SDKOfxPluginCtrl* getPlugin(const char *piname, unsigned int piversion);  // given plug-in name and major version, get the SDKOfxPluginCtrl
    static SDKOfxPluginCtrl* getPluginByFactoryName(const char *);
    static SDKOfxPluginCtrl* getPluginByParamSet(OfxParamSetHandle hParamSet);

    OfxPropertySetHandle  hostPropertySetHandle;
    OfxPropertySuiteV1    hostPropertySuite;
    // host property suite functions to be passed into the effects. 
    static NRiValue *getPropertySetValue(OfxPropertySetHandle properties, const char *property, int index, int type);
    static NRiPlug  *getPropertySetPlug (OfxPropertySetHandle properties, const char *property, int index, int type);    
    static OfxStatus propGetAsString (OfxPropertySetHandle properties, const char *property, int index, NRiString &value);    // get any property type as a NRiString
    static OfxStatus clientSetPointer  (OfxPropertySetHandle properties, const char *property, int index, void *value);  
    static OfxStatus clientSetString   (OfxPropertySetHandle properties, const char *property, int index, const char *value);
    static OfxStatus clientSetDouble   (OfxPropertySetHandle properties, const char *property, int index, double value);
    static OfxStatus clientSetInt      (OfxPropertySetHandle properties, const char *property, int index, int value);
    static OfxStatus clientSetPointerN (OfxPropertySetHandle properties, const char *property, int count, void **value);
    static OfxStatus clientSetStringN  (OfxPropertySetHandle properties, const char *property, int count, const char **value);
    static OfxStatus clientSetDoubleN  (OfxPropertySetHandle properties, const char *property, int count, double *value);
    static OfxStatus clientSetIntN     (OfxPropertySetHandle properties, const char *property, int count, int *value);
    static OfxStatus propGetPointer  (OfxPropertySetHandle properties, const char *property, int index, void **value);
    static OfxStatus propGetString   (OfxPropertySetHandle properties, const char *property, int index, char **value);
    static OfxStatus propGetDouble   (OfxPropertySetHandle properties, const char *property, int index, double *value);
    static OfxStatus propGetInt      (OfxPropertySetHandle properties, const char *property, int index, int *value); 
    static OfxStatus propGetPointerN (OfxPropertySetHandle properties, const char *property, int count, void **value);
    static OfxStatus propGetStringN  (OfxPropertySetHandle properties, const char *property, int count, char **value);
    static OfxStatus propGetDoubleN  (OfxPropertySetHandle properties, const char *property, int count, double *value);
    static OfxStatus propGetIntN     (OfxPropertySetHandle properties, const char *property, int count, int *value);
    static OfxStatus propReset       (OfxPropertySetHandle properties, const char *property) {return kOfxStatOK;}                // TODO -- implement it
    static OfxStatus propGetDimension(OfxPropertySetHandle properties, const char *property, int *count);
    static OfxStatus createProp      (OfxPropertySetHandle properties, const char *property, int type, int readWrite = 1, int dimension = 1,int hasPlug=0);
    static OfxStatus propSetPlug     (OfxPropertySetHandle properties, const char *property, int index, NRiPlug *p);
    static OfxStatus propSetPointer  (OfxPropertySetHandle properties, const char *property, int index, void *value, int checkRW = 0);  
    static OfxStatus propSetString   (OfxPropertySetHandle properties, const char *property, int index, const char *value, int checkRW = 0);
    static OfxStatus propSetDouble   (OfxPropertySetHandle properties, const char *property, int index, double value, int checkRW = 0);
    static OfxStatus propSetInt      (OfxPropertySetHandle properties, const char *property, int index, int value, int checkRW = 0);
    static OfxStatus propSetPointerN (OfxPropertySetHandle properties, const char *property, int count, void **value, int checkRW = 0);
    static OfxStatus propSetStringN  (OfxPropertySetHandle properties, const char *property, int count, const char **value, int checkRW = 0);
    static OfxStatus propSetDoubleN  (OfxPropertySetHandle properties, const char *property, int count, double *value, int checkRW = 0);
    static OfxStatus propSetIntN     (OfxPropertySetHandle properties, const char *property, int count, int *value, int checkRW = 0);
    
    
    OfxParameterSuiteV1 hostParameterSuite;
    // Host parameter suite functions.
    static OfxStatus paramDefine           (OfxParamSetHandle paramSet, const char* paramType, const char* paramName, OfxPropertySetHandle* propertySet);
    static OfxStatus paramGetHandle        (OfxParamSetHandle paramSet, const char* name, OfxParamHandle* param,  OfxPropertySetHandle* propertySet);
    static OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet, OfxPropertySetHandle* propHandle);
    static OfxStatus paramGetPropertySet   (OfxParamHandle    param,    OfxPropertySetHandle* propHandle);
    static OfxStatus paramGetValue         (OfxParamHandle    paramHandle, ...);
    static OfxStatus paramGetValueAtTime   (OfxParamHandle    paramHandle, OfxTime time, ...);
    static OfxStatus paramGetDerivative    (OfxParamHandle    paramHandle, OfxTime time, ...);
    static OfxStatus paramGetIntegral      (OfxParamHandle    paramHandle, OfxTime time1, OfxTime time2, ...);
    static OfxStatus paramSetValue         (OfxParamHandle    paramHandle, ...);
    static OfxStatus paramSetValueAtTime   (OfxParamHandle    paramHandle, OfxTime time, ...);
    static OfxStatus paramGetNumKeys       (OfxParamHandle    paramHandle, unsigned int* numberOfKeys);
    static OfxStatus paramGetKeyTime       (OfxParamHandle    paramHandle, unsigned int nthKey, OfxTime* time);
    static OfxStatus paramGetKeyIndex      (OfxParamHandle    paramHandle, OfxTime time, int direction, int* index);
    static OfxStatus paramDeleteKey        (OfxParamHandle    paramHandle, OfxTime time);
    static OfxStatus paramDeleteAllKeys    (OfxParamHandle    paramHandle);
    static OfxStatus paramCopy             (OfxParamHandle    paramTo,   OfxParamHandle paramFrom, OfxTime dstOffset, OfxRangeD* frameRange);
    static OfxStatus paramEditBegin        (OfxParamSetHandle paramSet,  const char* name);
    static OfxStatus paramEditEnd          (OfxParamSetHandle paramSet);


    OfxImageEffectSuiteV1 hostImageEffectSuite;
    // plugin image effect suite functions
    static OfxStatus getPropertySet            (OfxImageEffectHandle imageEffect, OfxPropertySetHandle *propHandle);                                                
    static OfxStatus getParamSet               (OfxImageEffectHandle imageEffect, OfxParamSetHandle* paramSet);
    static OfxStatus clipDefine                (OfxImageEffectHandle imageEffect, const char *name, OfxPropertySetHandle *propertySet);
    static OfxStatus clipGetHandle             (OfxImageEffectHandle imageEffect, const char *name, OfxImageClipHandle *clip, OfxPropertySetHandle *propertySet);
    static OfxStatus clipGetPropertySet        (OfxImageClipHandle clip, OfxPropertySetHandle *propHandle);                                                        
    static OfxStatus clipGetImage              (OfxImageClipHandle clip,OfxTime time, OfxRectD *region, OfxPropertySetHandle   *imageHandle);                      
    static OfxStatus clipReleaseImage          (OfxPropertySetHandle imageHandle);
    static OfxStatus clipGetRegionOfDefinition (OfxImageClipHandle clip, OfxTime time, OfxRectD *bounds);
    static int abort                           (OfxImageEffectHandle imageEffect);
    static OfxStatus imageMemoryAlloc          (OfxImageEffectHandle instanceHandle, size_t nBytes, OfxImageMemoryHandle *memoryHandle);                           
    static OfxStatus imageMemoryFree           (OfxImageMemoryHandle memoryHandle);                                                                                
    static OfxStatus imageMemoryLock           (OfxImageMemoryHandle memoryHandle, void **returnedPtr);
    static OfxStatus imageMemoryUnlock         (OfxImageMemoryHandle memoryHandle)                                                                                {return kOfxStatOK;}

    OfxMultiThreadSuiteV1 hostThreadSuite;
    static OfxStatus multiThread                (OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg);
    static OfxStatus multiThreadNumCPUs         (unsigned int *nCPUs)                                                              {*nCPUs = NRiThread::m_nproc(); return kOfxStatOK;}
    static OfxStatus multiThreadIndex           (unsigned int *threadIndex)                                                        {*threadIndex = 1; return kOfxStatOK;}
    static int       multiThreadIsSpawnedThread (void)                                                                             {return 1;}
    static OfxStatus mutexCreate                (const OfxMutexHandle *mutex, int lockCount);
    static OfxStatus mutexDestroy               (const OfxMutexHandle mutex);
    static OfxStatus mutexLock                  (const OfxMutexHandle mutex);
    static OfxStatus mutexUnLock                (const OfxMutexHandle mutex);
    static OfxStatus mutexTryLock               (const OfxMutexHandle mutex);
    
    
    OfxMemorySuiteV1 hostMemorySuite;
    static OfxStatus memoryAlloc               (void *handle, size_t nBytes, void **allocatedData);                             
    static OfxStatus memoryFree                (void *allocatedData);                                 
    
    OfxInteractSuiteV1 hostInteractSuite;
    static OfxStatus interactSwapBuffers       (OfxInteractHandle interactInstance);
    static OfxStatus interactRedraw            (OfxInteractHandle interactInstance);
    static OfxStatus interactGetPropertySet    (OfxInteractHandle interactInstance, OfxPropertySetHandle *property);
        
    OfxMessageSuiteV1 hostMessageSuite;
    static OfxStatus message(void* imageEffectHandle, const char* messageType, const char* messageID, const char* format, ...);

    static NRiName hostGeneratorWidthName;
    static NRiName hostGeneratorHeightName;
    static NRiName hostGeneratorBytesName;
    
    
private:
    static NRiName hostPropType;
    static NRiName hostPropName;
    static NRiName hostPropLabel;
    
};
#endif


