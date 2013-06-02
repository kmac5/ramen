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
 * $Header: /cvsroot/openfxshakehost/OFX/SDKOfxUtility.h,v 1.2 2006/09/12 19:51:07 dcandela Exp $
 *
 *
 */

#ifndef __SDKOfxClip_H__
#define __SDKOfxClip_H__

#include <NRiCurve.h>
#include <NRiCurveManager.h>
#include <NRiIPlug.h>
#include <NRiHashTable.h>
#include <NRiMath.h>
#include <NRiPArray.h>
#include <NRiPlug.h>
#include <NRiString.h>
#include <NRiThread.h>
#include <NRiValue.h>

#include <ofxCore.h>
#include <ofxProperty.h>
#include <ofxImageEffect.h>

class  NRiViewer;
class  NRiIBuffer;

/// for utility functions
namespace SDKOfx {
  enum ComponentEnum {eComponentRGBA,
                      eComponentAlpha,
                      kComponentEnumMax};

  /// turns the kOfxBitDepthX strings into a shake depth
  int mapFromOfxDepth(const NRiString &s);

  /// turns the shake bitdepth into a kOfxBitDepth value
  const char *mapToOfxDepth(int d);

  /// turns the kOfxImageComponentX strings into a shake 'active' bitmask
  int mapFromOfxComponents(const NRiString &s);

  /// turns the shake 'active' bitmask into a kOfxImageComponentX string
  const char *mapToOfxComponents(int d);

  /// turns our SDK clip's names into OFX clip names
  NRiString mapClipName(const char *n);

  /// turns our SDK clip's names into OFX clip names
  NRiString mapClipName(const NRiName &n);

  /// get the scale factors that map from OFX canonical coords to Shake's pixel coordinates
  void getCoordinateScaleFactors(double &x, double &y);

  /// get the render scale factors 
  void getRenderScaleFactors(double &x, double &y);

  /// get the pixel aspect ratio
  void getPixelAspectRatio(double &par);
};

class SDKOfxProperty
{
    
public:
    SDKOfxProperty(int type, int rw=1, int dimension=1, int hasPlug=0);
    ~SDKOfxProperty();
    int getDimension() {return _dimension;}
    int getType()      {return _type;}
    
    NRiValue *getValuePtr(int valueIndex);
    NRiName  *getNamePtr(int valueIndex)  {return (_stringValue + valueIndex);}
    NRiPlug  *getPlug(int valueIndex)     {return (_plugListPtr ? _plugListPtr[valueIndex] : 0);}
    void     setPlug(int idx, NRiPlug *p) {if (_plugListPtr) _plugListPtr[idx] = p; else assert(0); }    
    void resize(int newDimension = 1);
    int  getReadWritePermission()     {return _readWrite;}
    void setReadWritePermission(int rw) {_readWrite=rw;}    
    
    SDKOfxProperty& operator=( const SDKOfxProperty & );    
    
private:
	
    int _dimension;
    int _type;
    NRiName *_stringValue;
    NRiValue *_valuePtr;
    NRiPlug  **_plugListPtr;
    int   _readWrite;
};

class SDKOfxBase
{
public:
    SDKOfxBase()          {}
    virtual ~SDKOfxBase(); 
    
    const NRiString&    getName() {return m_name;}
    void                setName(const NRiString& name) {m_name = name;}
    
    void                      addProperty(const char *name, SDKOfxProperty *newProperty) { m_properties.addEntry(NRiName(name),  newProperty); }
    OfxPropertySetHandle      getPropertySetHandle()                                     { return (OfxPropertySetHandle)(&m_properties);       }
    
    NRiHashTable<SDKOfxProperty*>*  getPropertyTable() { return &m_properties; }
    SDKOfxProperty*                 getProperty(const NRiString& propName) {return m_properties.getValue(propName.toCStr());}
  
private:    
    
    NRiString m_name;
    NRiHashTable<SDKOfxProperty*> m_properties;
};


class SDKOfxPlugin;
class SDKOfxOverlay;
class SDKOfxParm;
class SDKOfxParameterList
{
public:
    SDKOfxParameterList();
    SDKOfxParameterList(SDKOfxPlugin* owner);
    ~SDKOfxParameterList();
    
    // Property groups have names.
    const NRiString&    getName() {return m_name;}
    void                setName(const NRiString& name) {m_name = name;}
    const NRiString&    getParentName() {return m_parentName;}
    void                setParentName(const NRiString& name) {m_parentName = name;}
    
    bool    addParameter(SDKOfxParm* newParam) { m_parameters.append(newParam); return true; }
    bool    addParameterList(SDKOfxParameterList* pNewList) { m_childLists.append(pNewList); return true; }   
    
    SDKOfxParm*             getParamByName(const char *name);
    SDKOfxParameterList*    getChildListByName(const NRiString& name);
    SDKOfxPlugin*           getOwner() { return m_owner; }

    const NRiPArray<SDKOfxParameterList>* getChildLists() const {return &m_childLists;}
    
    int getNbItems() { return m_parameters.getNbItems(); }
    
    SDKOfxParm *operator [] (int i) 
    { 
        return m_parameters[i];
    }
    
    SDKOfxParameterList& operator=( const SDKOfxParameterList& parmList );
    
private:    
        
    NRiPArray<SDKOfxParm>           m_parameters;
    SDKOfxPlugin*                   m_owner;
    NRiString                       m_name;
    NRiString                       m_parentName;
    NRiPArray<SDKOfxParameterList>  m_childLists;
    
};

class SDKOfxParm : public SDKOfxBase
{
    // We may need Get & Set methods for the name to protect against garbage characters or spaces in it
    // when we use it to generate factory-method parameters.
    // Not to mention methods to support the more complicated parameter types like "choice",
    // which is a list of labeled integer values.
public:
    SDKOfxParm(const char *name, const char *type);
    SDKOfxParm(SDKOfxParm *parm);
    virtual ~SDKOfxParm();        
    
    NRiString getParentGroupName();   // A param may be part of a group of them.
    
    const NRiString&    getType()                          { return m_parmType;             }
    const NRiString&    getLabel()                         { return m_Label;                }
    const NRiString&    getSuffix(int idx)                 { return (*m_parmSuffixes[idx]); }    
    const int           getPlugCount()                     { return m_plugCount;            }    
    const NRiId         getPlugType()                      { return m_plugType;             }    
    void                getPlugName(NRiName &plugName, int index = -1);
    void                getValue(va_list);
    void                setValue(va_list);    
    void                setValueAtTime(float, va_list);
    static SDKOfxOverlay *m_OverlayInUse;

    void                getValueAtTime(float, va_list);    
    void                getDerivativeAtTime(float, va_list);
    void                getIntegral(float, float, va_list);
    void                getNumKeys(unsigned int* numberOfKeys);
    void                getKeyTime(unsigned int, float *);    
    OfxStatus           getKeyIndex(float, int, int *);
    OfxStatus           deleteKey(float);    
    void                deleteAllKeys();     
    void                copy(SDKOfxParm *from, double timeOffset, OfxRangeD* frameRange);
    void                addPlug(NRiPlug *p)                { m_plugs.append(p); }
    NRiPArray<NRiPlug> *getPlugList()                      { return &m_plugs;   }      
    
    int                 getNumValues();     // return the number of values corresponding to this parameter type
    SDKOfxProperty      *getRenderScaleProp()              { return m_renderScaleProp;  }
    SDKOfxProperty      *getTimeProp()                     { return m_timeProp;         }
    SDKOfxProperty      *getChangeReasonProp()             { return m_changeReasonProp; }       
    SDKOfxProperty      *getEnabledProp()                  { return m_enabledProp; }           
    
private:
        
    void        initialize();
    void        initializeProperties();
    void        getParmKeyList(NRiArray<float> &);
    
    NRiString            m_Label;
    NRiString            m_parmType;
    NRiPArray<NRiString> m_parmSuffixes;
    NRiPArray<NRiPlug>   m_plugs;    
    int                  m_plugCount;
    NRiId                m_plugType;
    
    static NRiString     strEmpty, strX, strY, strZ;
    static NRiString     strR, strG, strB, strA;    
    
    SDKOfxParameterList* m_pParentGroup;
    
    SDKOfxProperty      *m_renderScaleProp;
    SDKOfxProperty      *m_timeProp;    
    SDKOfxProperty      *m_changeReasonProp;        
    SDKOfxProperty      *m_nameProp;
    SDKOfxProperty      *m_enabledProp;    
    char                *m_allocatedString;
};

class SDKOfxClip : public SDKOfxBase
{
public:

    SDKOfxClip();
    virtual ~SDKOfxClip() {};
    void setInputPlug(NRiIPlug *ip) {inputPlug = ip;}
    NRiIPlug *getInputPlug()        {return inputPlug;}
    NRiHashTable<SDKOfxProperty*>  *getImagePropertyTable() { return &m_imageProperties; }
    SDKOfxClip& operator=( SDKOfxClip& clip );
    void    createInstanceProperties(const NRiString &clipName);
    OfxStatus   getImage(OfxTime time, OfxRectD *region, OfxPropertySetHandle *imageHandle);
    static OfxStatus releaseImage(OfxPropertySetHandle imageHandle);    
  int  setImageProperties(int oBytes, const NRiIRect &dataRect, const NRiIRect &fullImageRect, int stride, const char *uniqueId, void *dataPtr, int components);

  // called after any effect action which may have fetched images and not freed them all
  void cleanUpFetchedImages();

  /// maps an Shake active mask into one that this clips supports
  int mapToSupportedActive(int active);
  
    static int _interrupted;
private:
  /// An array indicating the components the effect is willing to accept for the output clip
  bool m_supportedComponents[SDKOfx::kComponentEnumMax];

    void    createBaseProperties();
    
    NRiBuffer _tmpBuf;
    NRiIPlug *inputPlug;
    NRiHashTable<SDKOfxProperty*> m_imageProperties;
};

// This is used by the Shake OFX host implementation only and is not used to implement anything specific to OFX.
// More specifically, this is used to pass inforation to the clipGetImage function to prepare the image buffers for the OFX client.
class SDKOfxActionData
{
public:
    SDKOfxActionData(int context)
    {
        _actionContext = context;
    }
    
    SDKOfxActionData(NRiViewer *viewer)
    {
        _viewer = viewer;
        _actionContext = kOverlay;
    }
    
    virtual ~SDKOfxActionData() {}
    
    enum
    {
        kGeneral = 0,
        kOverlay,
        kTweaker
    };

    NRiViewer *_viewer;
    int        _actionContext;
    
    
private:    
    
};

#endif
