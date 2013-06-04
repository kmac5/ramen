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
 * $Header: /cvsroot/openfxshakehost/OFX/SDKOfxOverlay.h,v 1.2 2006/09/12 19:51:07 dcandela Exp $
 *
 *
 */

#ifndef __SDKOfxOverlay_H__
#define __SDKOfxOverlay_H__

#include <SDKOfxPluginHost.h>
#include <SDKOfxUtility.h>
#include <NRiUIOverlay.h>

class SDKOfxEffect;

class SDKOfxOverlay : public NRiOverlayControl
{
public:
    SDKOfxOverlay(NRiUIOverlay* overlay, SDKOfxEffect *fxNode);
    virtual ~SDKOfxOverlay();
    
    static int createControl(NRiUIOverlay *overlay, NRiPArray<NRiOverlayControl>& list);
    
    void         redraw();
    void         swapBuffers();
    virtual void init();
    virtual void draw(int state);
    virtual int	 pick(const NRiV2f &cursor, const NRiM4f &sMatrix);
    virtual void move(const NRiV2f &cursor, const NRiM4f &sMatrix, NRiV2f &cursorDown, int ctrl);
    virtual void endInteraction();    
    virtual void event( NRiMsg *);
    virtual int	 notify(NRiPlug *p);

    SDKOfxBase   *getOfxInfo()     { return &ofxProps; }    
    
    SDKOfxEffect *getOfxInstance() { return ofxNode;    }    
    
    void resetViewport();
    
    void setKey(NRiPlug *p, float v);
private:
        
    GLdouble _projectionMatrix[4][4];
    SDKOfxBase    ofxProps;    
    SDKOfxEffect *ofxNode;
    SDKOfxProperty *m_propInstance;
    SDKOfxProperty *m_propViewportSize;  
    SDKOfxProperty *m_propPixScale;
    SDKOfxProperty *m_propBgColor;
    SDKOfxProperty *m_propTime;
    SDKOfxProperty *m_propProxyScale;
    SDKOfxProperty *m_propCursorPos;
    SDKOfxProperty *m_propPenPressure; 
    SDKOfxProperty *m_propPropSlaveToParam;
    NRiPArray<NRiPlug> m_slaves;
    SDKOfxProperty *m_propPropViewportCursorPos;

    int          m_mouseDown;
    int          m_upX, m_upY;
    int          m_viewportOrigin[2];
    double       m_pressure;
    
    OfxStatus (*overlayMain)(const char *action,  const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs);
    void setViewportAndPixelScale();
    SDKOfxOverlay(NRiUIOverlay *overlay);
    
};

#endif
