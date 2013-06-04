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
 * $Header: /cvsroot/openfxshakehost/OFX/SDKOfxOverlay.cpp,v 1.2 2006/09/12 19:51:07 dcandela Exp $
 *
 *
 */

#include <NRiGlobals.h>
#include <NRiSysWin.h>
#include <NRiViewer.h>
#include <NRiWin.h>

#include <ofxCore.h>
#include <ofxProperty.h>
#include <ofxImageEffect.h>
#include <ofxKeySyms.h>

#include "SDKOfxOverlay.h"
#include "SDKOfxEffect.h"

static NRiPlug * globalTime	     = NRiGlobals::ref("script.time", kFloat);
static NRiPlug * globalXProxyScale   = NRiGlobals::ref("script.xProxyScale", kFloat);
static NRiPlug * globalYProxyScale   = NRiGlobals::ref("script.yProxyScale", kFloat);
static NRiPlug * globalActionData    = NRiGlobals::ref("sys.actionData",kPtr);            // points to the data used by the action.

static int  registerOFXOverlay = NRiOverlayControl::registerControl(SDKOfxOverlay::createControl, 1);

int SDKOfxOverlay::createControl(NRiUIOverlay *overlay, NRiPArray<NRiOverlayControl>& list)
{
    SDKOfxOverlay * rc = 0;
    SDKOfxEffect *ofxNode = 0;
    
    NRiNode *twkNode = overlay->iNode;
    if (twkNode)
    {
        NRiGrpNode *grp  = dynamic_cast<NRiGrpNode *>(twkNode);
        if (grp) 
        {
            grp->buildBody();
            int nChildren = grp->getNbChildren();
            int i;
            for (i = 0; i < nChildren; i++)
            {
                ofxNode = dynamic_cast<SDKOfxEffect *>(grp->getNthChild(i));
                if (ofxNode)
                {
                    void * overlayMain;
                    SDKOfxPluginHost::propGetPointer(ofxNode->getPluginCtrl()->getPropertySetHandle(), kOfxImageEffectPluginPropOverlayInteractV1, 0,  (void **) &overlayMain);
                    if (!overlayMain) ofxNode = 0; // we have no overlayMain in the plugin, don't create the control
                    break;
                }
            }
            
        }
    }
    
    if (ofxNode)
    {
        rc = new SDKOfxOverlay(overlay, ofxNode);
    }
    
    if (rc) list.append(rc);
    return list.getLength();
}

SDKOfxOverlay::SDKOfxOverlay(NRiUIOverlay* overlay, SDKOfxEffect *fxNode) :
    NRiOverlayControl(),
    m_mouseDown(0)
{
#ifdef _DEBUG
    printf("debug: SDKOfxOverlay 0x%X properySet = 0x%X\n", (int)getOfxInfo(), (int)getOfxInfo()->getPropertySetHandle());
#endif    
        
    ofxNode = fxNode;

    // kOfxPropEffectInstance    
    m_propInstance = new SDKOfxProperty(kPtr);
    getOfxInfo()->addProperty(kOfxPropEffectInstance, m_propInstance);
    m_propInstance->getValuePtr(0)->p = (void *) getOfxInstance()->getOfxInfo();    
    
    m_propPropSlaveToParam = new SDKOfxProperty(kString,1,0);
    getOfxInfo()->addProperty(kOfxInteractPropSlaveToParam, m_propPropSlaveToParam);    
    
    SDKOfxProperty *prop  = new SDKOfxProperty(kPtr, 1);
    getOfxInfo()->addProperty(kOfxPropInstanceData, prop);   

    prop  = new SDKOfxProperty(kPtr, 0);
    prop->getValuePtr(0)->p = this;
    getOfxInfo()->addProperty("ShakeOverlayPtr", prop);   
    
    // set the property that is the overlay's main entry point for the plugin
    SDKOfxPluginHost::propGetPointer(ofxNode->getPluginCtrl()->getPropertySetHandle(), kOfxImageEffectPluginPropOverlayInteractV1, 0,  (void **) &overlayMain);
    if (overlayMain) 
    {
	overlayMain(kOfxActionDescribe,       (const void *)getOfxInfo(), NULL, NULL);
	overlayMain(kOfxActionCreateInstance, (const void *)getOfxInfo(), NULL, NULL);    
    }
    
    int nSlaveParms = m_propPropSlaveToParam->getDimension();
    SDKOfxParameterList *nodeParams = fxNode->getParameterList();
    int i;
    for (i = 0; i < nSlaveParms; i++)
    {
        NRiName *slave = m_propPropSlaveToParam->getNamePtr(i);
		SDKOfxParm *param = nodeParams->getParamByName(slave->getString());
		if(param) {
			NRiPArray<NRiPlug> *plugs = param->getPlugList();
			
			int j;
			for (j = 0; j < param->getPlugCount(); j++)
			{
				NRiPlug *plug = (*plugs)[j];
                                NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
				NRiName plugName;
				param->getPlugName(plugName, j);
				NRiPlug* slave = addPlug(plugName, param->getPlugType(), NRiPlug::kIn);            
				slave->connect(grpPlug);
				slave->setNotify(1);
				m_slaves.append(slave);
			}
		}
    }

    // OFX does not tell us which parameters will be affected by OSC.  We have to assume any of them can, register them all...
    SDKOfxParameterList *parmList = fxNode->getParameterList();
    int numParms = parmList->getNbItems();
    for (i = 0; i < numParms; i++)
    {
        SDKOfxParm *parm = (*parmList)[i];
        
        int animates, secret;
        SDKOfxPluginHost::propGetInt(parm->getPropertySetHandle(), kOfxParamPropAnimates, 0, &animates);
        SDKOfxPluginHost::propGetInt(parm->getPropertySetHandle(), kOfxParamPropSecret, 0, &secret);        
        
        NRiString paramType = parm->getType();
        if (animates && !secret && 
            (paramType == kOfxParamTypeInteger ||
             paramType == kOfxParamTypeDouble ||
             paramType == kOfxParamTypeBoolean ||
             paramType == kOfxParamTypeDouble2D ||
             paramType == kOfxParamTypeDouble3D ||
             paramType == kOfxParamTypeInteger3D))
        {
            NRiPArray<NRiPlug> *plugs = parm->getPlugList();
            
            int j;
            for (j = 0; j < parm->getPlugCount(); j++)
            {
                NRiPlug *plug = (*plugs)[j];
                NRiPlug *grpPlug = plug->getOwner()->getParent()->getPlug(plug->getName());
                if (grpPlug) registerPlug(grpPlug);
            }
        }
    }
    
    // kOfxInteractPropViewportSize
    m_propViewportSize = new SDKOfxProperty(kInt, 0, 2);
    getOfxInfo()->addProperty(kOfxInteractPropViewportSize, m_propViewportSize);

    // kOfxInteractPropPixelScale 
    m_propPixScale = new SDKOfxProperty(kDouble, 0, 2);
    getOfxInfo()->addProperty(kOfxInteractPropPixelScale, m_propPixScale);
    m_propPixScale->getValuePtr(0)->d = 1.;
    m_propPixScale->getValuePtr(1)->d = 1.;    
    
    // kOfxInteractPropBackgroundColour
    m_propBgColor = new SDKOfxProperty(kDouble, 0, 3);
    getOfxInfo()->addProperty(kOfxInteractPropBackgroundColour, m_propBgColor);
    m_propBgColor->getValuePtr(0)->d = 0.;
    m_propBgColor->getValuePtr(1)->d = 0.;    
    m_propBgColor->getValuePtr(2)->d = 0.;        
    
    // kOfxPropTime
    m_propTime = new SDKOfxProperty(kDouble);
    getOfxInfo()->addProperty(kOfxPropTime, m_propTime);
    m_propTime->getValuePtr(0)->d = double(globalTime->asFloat());
    
    // kOfxImageEffectPropRenderScale
    m_propProxyScale = new SDKOfxProperty(kDouble, 0, 2);
    getOfxInfo()->addProperty(kOfxImageEffectPropRenderScale, m_propProxyScale);
    m_propProxyScale->getValuePtr(0)->d = globalXProxyScale->asFloat();
    m_propProxyScale->getValuePtr(1)->d = globalYProxyScale->asFloat();    
    
    m_propCursorPos = new SDKOfxProperty(kDouble, 0, 2);
    getOfxInfo()->addProperty(kOfxInteractPropPenPosition, m_propCursorPos);    

    m_propPropViewportCursorPos = new SDKOfxProperty(kInt, 0, 2);
    getOfxInfo()->addProperty("OfxInteractPropPenViewportPosition", m_propPropViewportCursorPos);    

    m_propPenPressure = new SDKOfxProperty(kDouble, 0);
    getOfxInfo()->addProperty(kOfxInteractPropPenPressure, m_propPenPressure);    

    prop = new SDKOfxProperty(kInt, 0);
    getOfxInfo()->addProperty(kOfxPropKeySym, prop);

    prop = new SDKOfxProperty(kString, 0);
    getOfxInfo()->addProperty(kOfxPropKeyString, prop);

    setViewportAndPixelScale();
    
    overlayMain(kOfxInteractActionGainFocus, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);        
    resetViewport();            
}

SDKOfxOverlay::~SDKOfxOverlay()
{
    setViewportAndPixelScale();
    overlayMain(kOfxInteractActionLoseFocus, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);        
    overlayMain(kOfxActionDestroyInstance, (const void *)getOfxInfo(), NULL, NULL);    
    resetViewport();            
}

void SDKOfxOverlay::init()
{
}

int SDKOfxOverlay::notify(NRiPlug *p)
{
    if (m_slaves.index(p) >= 0)
    {
        redraw();
    }
    
    return NRiOverlayControl::notify(p);
}



void SDKOfxOverlay::setViewportAndPixelScale()
{
    if(overlay && overlay->viewer) 
    {
        double zom = 1.0/overlay->viewer->zoom()->asDouble();
        m_propPixScale->getValuePtr(0)->d = zom;
        m_propPixScale->getValuePtr(1)->d = zom;
        
        m_viewportOrigin[0] = m_viewportOrigin[1] = 0;        
        overlay->viewer->inCanvas(&m_viewportOrigin[0], &m_viewportOrigin[1]);

        int widV = overlay->viewer->width()->asInt();
        int htV = overlay->viewer->height()->asInt();    
        m_propViewportSize->getValuePtr(0)->i = widV;
        m_propViewportSize->getValuePtr(1)->i = htV;
        
        GLint viewport[4];
        
        glGetDoublev(GL_PROJECTION_MATRIX, &_projectionMatrix[0][0]);
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        
        float originDeltaX = viewport[0] - m_viewportOrigin[0];
        float originDeltaY = viewport[1] - m_viewportOrigin[1];

        float sizeDeltaX = double(viewport[2])/double(widV);
        float sizeDeltaY = double(viewport[3])/double(htV);

        glMatrixMode(GL_PROJECTION);
        glScalef(sizeDeltaX, sizeDeltaY, 1.0f);
        glTranslatef(originDeltaX, originDeltaY, 0);

        glMatrixMode(GL_MODELVIEW);

        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(m_viewportOrigin[0], m_viewportOrigin[1], widV, htV);
    }
}

void SDKOfxOverlay::resetViewport()
{
    if(overlay && overlay->viewer) 
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(&_projectionMatrix[0][0]);
        glMatrixMode(GL_MODELVIEW);
        glPopAttrib();        
    }
}

void SDKOfxOverlay::draw(int state)
{
    // Draw it
    m_propTime->getValuePtr(0)->d = double(globalTime->asFloat());
    m_propProxyScale->getValuePtr(0)->d = globalXProxyScale->asFloat();
    m_propProxyScale->getValuePtr(1)->d = globalYProxyScale->asFloat();    
    
    if (overlayMain) 
    {
        setViewportAndPixelScale();
        
        SDKOfxActionData actionData(overlay->viewer);
        SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();                
        globalActionData->set(&actionData);
        overlayMain(kOfxInteractActionDraw, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);    
        globalActionData->set(prevActionData);
        resetViewport();        
    }
}

int SDKOfxOverlay::pick(const NRiV2f &cursor, const NRiM4f &sMatrix)
{
    if (overlay->pXFormOff->asInt() || overlay->pHideXForm->asInt()) return 0;    
    m_propTime->getValuePtr(0)->d = double(globalTime->asFloat());
    m_propProxyScale->getValuePtr(0)->d = globalXProxyScale->asFloat();
    m_propProxyScale->getValuePtr(1)->d = globalYProxyScale->asFloat();    
    
    NRiV2f cursorInImgSpace;
    NRiM3f mInverted;
    sMatrix.mk3x3(mInverted);
    mInverted.invert();
    mInverted.transform( cursorInImgSpace, cursor );
    
    m_propCursorPos->getValuePtr(0)->d = cursorInImgSpace.X();
    m_propCursorPos->getValuePtr(1)->d = cursorInImgSpace.Y();
    
    // Pressure set and converted from NRiMsg range (0-255) in event()
    //    m_propPenPressure->getValuePtr(0)->d = m_pressure;
    
    // pick it
    SDKOfxParm::m_OverlayInUse = this;
    SDKOfxActionData actionData(overlay->viewer);
    SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();                    
    globalActionData->set(&actionData);
    setViewportAndPixelScale();
    m_propPropViewportCursorPos->getValuePtr(0)->i = (int)cursor.X()+m_viewportOrigin[0];
    m_propPropViewportCursorPos->getValuePtr(1)->i = (int)cursor.Y()+m_viewportOrigin[1];

    OfxStatus returnStatus = kOfxStatReplyDefault;
    overlayMain(kOfxInteractActionPenMotion, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);    
    if (m_mouseDown != 0) {
         returnStatus = overlayMain(kOfxInteractActionPenDown, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);    
    }
    globalActionData->set(prevActionData);
    SDKOfxParm::m_OverlayInUse = 0;
    resetViewport();        
            
    return (returnStatus == kOfxStatOK ? 1 : 0);
}

void SDKOfxOverlay::setKey(NRiPlug *p, float v)
{
    overlay->setKey(p ,v);
}

void SDKOfxOverlay::move(const NRiV2f &cursor, const NRiM4f &sMatrix, NRiV2f &cursorDown, int ctrl)
{
    SDKOfxParm::m_OverlayInUse = this;
    m_propTime->getValuePtr(0)->d = double(globalTime->asFloat());
    m_propProxyScale->getValuePtr(0)->d = globalXProxyScale->asFloat();
    m_propProxyScale->getValuePtr(1)->d = globalYProxyScale->asFloat();    
    
    NRiV2f cursorInImgSpace;
    NRiM3f mInverted;
    sMatrix.mk3x3(mInverted);
    mInverted.invert();
    mInverted.transform( cursorInImgSpace, cursor );
    
    m_propCursorPos->getValuePtr(0)->d = cursorInImgSpace.X();
    m_propCursorPos->getValuePtr(1)->d = cursorInImgSpace.Y();
    
    // TODO - deal with pressure
//    m_propPenPressure->getValuePtr(0)->d = ;
    
    // move it
    SDKOfxActionData actionData(overlay->viewer);
    SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();                        
    globalActionData->set(&actionData);
    
    setViewportAndPixelScale();
    m_propPropViewportCursorPos->getValuePtr(0)->i = (int)cursor.X()+m_viewportOrigin[0];
    m_propPropViewportCursorPos->getValuePtr(1)->i = (int)cursor.Y()+m_viewportOrigin[1];
    
    overlayMain(kOfxInteractActionPenMotion, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);    
    globalActionData->set(prevActionData);
    resetViewport();        
            
    SDKOfxParm::m_OverlayInUse = 0;
}

void SDKOfxOverlay::endInteraction()
{
    SDKOfxParm::m_OverlayInUse = this;    
    m_propTime->getValuePtr(0)->d = double(globalTime->asFloat());
    m_propProxyScale->getValuePtr(0)->d = globalXProxyScale->asFloat();
    m_propProxyScale->getValuePtr(1)->d = globalYProxyScale->asFloat();    

    NRiM4f  sMatrix;
    overlay->getPathMatrix(sMatrix);

    NRiV2f cursor(m_upX, m_upY);
    NRiV2f cursorInImgSpace;
    NRiM3f mInverted;
    sMatrix.mk3x3(mInverted);
    mInverted.invert();
    mInverted.transform( cursorInImgSpace, cursor );

    m_propCursorPos->getValuePtr(0)->d = cursorInImgSpace.X();
    m_propCursorPos->getValuePtr(1)->d = cursorInImgSpace.Y();
    
    // TODO - deal with pressure
    //    m_propPenPressure->getValuePtr(0)->d = ;
    
    // release mouse 
    SDKOfxActionData actionData(overlay->viewer);
    SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();                            
    globalActionData->set(&actionData);
    
    setViewportAndPixelScale();
    m_propPropViewportCursorPos->getValuePtr(0)->i = (int)cursor.X()+m_viewportOrigin[0];
    m_propPropViewportCursorPos->getValuePtr(1)->i = (int)cursor.Y()+m_viewportOrigin[1];
    
    overlayMain(kOfxInteractActionPenUp, (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL);    
    globalActionData->set(prevActionData);    
    SDKOfxParm::m_OverlayInUse = 0;    
    resetViewport();            
}

void SDKOfxOverlay::event( NRiMsg *m )
{
    if ( !active() ) return;
    
    if (m->type()==m->ButtonDown &&     // Mouse button down
        !(m->state() & NRiMsg::AltDown) && // No alt - this would mean a middle mouse        
        m->val()==0   
        )
    {
        m_mouseDown = 1;

        // Normally Shake checks for pick() only on mouseMove events.  OFX wants this test done on buttonDown.  So here we fake out the viewer by passing it a mouseMove event when we
        // get a buttonDown.  Then viewer will then try the pick() test.
        NRiMsg msg( NRiMsg::MouseMove );
        msg.X(m->X());
        msg.Y(m->Y());        
        msg.scX(m->scX());
        msg.scY(m->scY());        
        overlay->viewer->event(&msg);
    }
    else if (m->type()==m->ButtonUp &&
             !(m->state() & NRiMsg::AltDown) && // No alt - this would mean a middle mouse             
             m->val()==0 )
    {
        m_mouseDown = 0;
        m_upX = m->X();
        m_upY = m->Y();        
    }
    else if ((m->type()==NRiMsg::KeyDown || m->type()==NRiMsg::KeyUp) &&
             m->used()==0 )
    {
        SDKOfxParm::m_OverlayInUse = this;            
        unsigned int keyVal = m->val();
        SDKOfxPluginHost::propSetInt(getOfxInfo()->getPropertySetHandle(), kOfxPropKeySym, 0, keyVal);
        char keyStr[2] = {0, 0};
        if (keyVal < 128)
        {
            keyStr[0] = keyVal;
        }
        SDKOfxPluginHost::propSetString(getOfxInfo()->getPropertySetHandle(), kOfxPropKeyString, 0, keyStr);
        
        SDKOfxActionData actionData(overlay->viewer);
        SDKOfxActionData *prevActionData = (SDKOfxActionData *)globalActionData->asPtr();                                    
        globalActionData->set(&actionData);
        
        setViewportAndPixelScale();
        if (kOfxStatOK == overlayMain(m->type()==NRiMsg::KeyDown ? kOfxInteractActionKeyDown : kOfxInteractActionKeyUp, 
                                      (const void *)getOfxInfo(), getOfxInfo()->getPropertySetHandle(), NULL) )
        {
            m->used(1);
        }
        globalActionData->set(prevActionData);
        SDKOfxParm::m_OverlayInUse = 0;            
        resetViewport();                
    }
    if (m->tablet() != 0) {
        m_pressure = (float)m->pres()/255.f;
    } 
    
    NRiOverlayControl::event( m );
}
    
void SDKOfxOverlay::redraw()
{
    overlay->viewer->damage();
}

void SDKOfxOverlay::swapBuffers()
{
    NRiWin *topWin = (NRiWin*)overlay->viewer->getTopParent();
    if (topWin)
    {
        NRiSysWin *sys = topWin->sysWin();
        void *gc = overlay->viewer->getCurrGC();
        if (gc && sys) sys->swap(gc); 
    }
}


