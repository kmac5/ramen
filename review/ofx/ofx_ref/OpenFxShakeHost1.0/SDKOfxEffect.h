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
* $Header: /cvsroot/openfxshakehost/OFX/SDKOfxEffect.h,v 1.2 2006/09/12 19:51:07 dcandela Exp $
*
*
*/

#ifndef __SDKOfxEffect_H__
#define __SDKOfxEffect_H__

#include <NRiHashTable.h>
#include <NRiNadic.h>
#include <SDKOfxPluginHost.h>


class SDKOfxEffect : public NRiNadic {
    
public:

    //--------------------------------------------------------------------------
    /// Shake image processing nodes should setup input plugs, internal nodes,
    /// and the connection logic between the input image plug(s) and the output
    /// image plug in the constructor.  In addition, for any plugs that are
    /// evaluated by this node (i.e. output plugs), the input plugs that they
    /// depend upon for evaluation should be registered with Shake.
    //--------------------------------------------------------------------------
    SDKOfxEffect();
    virtual ~SDKOfxEffect();
    
    
    SDKOfxPluginCtrl            *getPluginCtrl()                           { return m_ctrl;                        }
    SDKOfxBase                  *getOfxInfo()                              { return &m_ofxInfo;                    }
    OfxParamSetHandle            getParamSetHandle()                       { return m_ofxInfo.getParamSetHandle(); }    
    SDKOfxParameterList         *getParameterList()                        { return m_ofxInfo.getParameterList();  }
    OfxPropertySetHandle         getPluginClipPreferenceProperties()       { return (OfxPropertySetHandle)(&m_pluginClipPreferenceProperties); }
    OfxPropertySetHandle         getPluginRegionsOfInterestProperties()       { return (OfxPropertySetHandle)(&m_pluginGetRegionsOfInterestProperties); }

    void                         setPluginCtrl(SDKOfxPluginCtrl *);
    NRiIPlug                    *createInput(const char *name);           
    NRiIPlug                    *createOutput(const char *name);           
    
    virtual int	 eval(NRiPlug *p);
    virtual int	 notify(NRiPlug *p);    
    virtual void setParent(NRiNode *node);    
    virtual void internalizeSecretStuff(NRiPArray<NRiPlug> &);
    virtual void externalizeSecretStuff(NRiPArray<NRiPlug> &);
    virtual int serialize(NRiFile& fp, int mask);
    virtual int endExec();
    virtual void fillOutputBuffer();
    virtual int fillInputs();
    
    class NRxPlugProperty               // for a parameter plug, maintain the necessary information so that we can get back to its associated ofxProperty
    {
        public:
            NRxPlugProperty(SDKOfxParm *prop, int idx) { m_property = prop; m_index = idx; }
            SDKOfxParm *m_property;
            int         m_index;           
    };
    
  /// map from shake's pixel coordinates to OFX canonical coords
  void mapToOFXCoords(double &x, double &y);

  /// map to shake's pixel coordinates from OFX canonical coords
  void mapFromOfxCoords(double &x, double &y);

  /// look in the clip preferences and get the bit depth for the named clip
  int getClipBitDepth(const char *name);

   // get the bit depth for the i'th clip
  int getClipBitDepth(int i);

  /// look in the clip preferences and get the active components bit mask for the named clip
  int getClipComponents(const char *name);

  /// get the components on the i'th clip
  int getClipComponents(int i);

  /// get the roi last returned from the get regions of interest action on the ith clip
  NRiIRect getClipRoI(const char *name);
  
  /// get the roi last returned from the get regions of interest action on the ith clip
  NRiIRect getClipRoI(int i);
  
  // Determine if the node is a generator
  // if we are in a generator node,  let the defined plug drive the byte depth
  int isGenerator();

  static const char *shakeEffectProperty;
  NRiPlug *m_doneInitializing;
    
protected:
        
    virtual void addInput(const NRiName &iName);        
    
private:
        
        struct NRxBInfo {
            
            NRxBInfo() : waterLine(0), active(0) { iRoi = NRiIRect::nullIRect; }
            NRiName	    bCacheId;
            NRiBuffer   zBuf;
            NRiIRect    iRoi;
            int	    waterLine;
            int	    active;
        };
    
        
    SDKOfxPlugin      m_ofxInfo;
    SDKOfxPluginCtrl *m_ctrl;
    NRiPlug          *m_duration;
    NRiBPixel	      m_bPixel;    

  /// An array indicating which depths this effect is willing to support
  /// if m_supportedDepths[X] is set then the equivalant 'getBytes' depth
  /// is supported by the effect.
  static const size_t kMaxSupportedDepths = 5;
  bool m_supportedDepths[kMaxSupportedDepths]; 

  /// clean up all fetched images that were not released during the action
  void cleanUpFetchedImages();
  
  /// map the depth to one that this effect supports
  int mapToSupportedDepth(int depth);
  
  /// run the OFX clip preferences command
  bool doOfxClipPreferences();  

  /// evaluate the RoI of the effect on the given rect
  void doOfxGetRegionsOfInterest(const NRiIRect &roi);

    void forceTwkRebuild();         // force the tweaker to rebuild itself.
    void definePlugs();
    void defineClips();
    int  setClipProperties(NRiIPlug *ip, int connected, int inputBytes = -1);
    int  setImageProperties(NRiIPlug *ip, int bytes, int active, const NRiIRect &dataRect, const NRiIRect &fullImageRect, void *dataPtr, int stride);
    int  effectMode;
    int  m_inNotify;
    int  _bPixelRender;
    NRiPArray<NRiIRect>	  m_iRois; ///< the input ROIs calculated for the eval call
    NRiPArray<NRiBuffer>  m_iBuffers; ///< the buffers filled in the for the eval of the output buf
    NRiBuffer             m_oBuffer;
    NRiPArray<NRiPlug>    m_plugs;
    NRiPArray<NRiPlug>    m_enabledPlugs;
    NRiPArray<NRiPlug>    m_cIdWatchers;    
    NRiPArray<SDKOfxParm> m_enabledParms;    
    NRiArray<NRiName>     m_clipNames; // will have ofx_ prepended
    NRiArray<NRiName>     m_rawClipNames; // will not have ofx_ prepended
    NRiHashTable<NRxPlugProperty*> m_plugParms;           // Hash table to get the OFX parameter given the plug name
  NRiHashTable<SDKOfxProperty*> m_pluginClipPreferenceProperties;  ///< hash tabel of the clip preference properties
  NRiHashTable<SDKOfxProperty*> m_pluginGetRegionsOfInterestProperties; ///< hash table of the ROI values last returned
        
    NRiDeclareNodeName(SDKOfxEffect);
};

#endif
