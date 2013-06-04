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
#ifdef macos
#include <unistd.h>
#include "CoreServices/CoreServices.h"
#else
#include <unistd.h>
#include <pthread.h>
#endif

#include "NRiThread.h"
#include "NRiLock.h"

#include <ofxCore.h>
#include <ofxMultiThread.h>
namespace SDKOfx {

#ifdef macos
  typedef ThreadID OFXThreadID;
#else
  typedef unsigned long  OFXThreadID;
#endif

  /// the ids of the currently executing threads
  static OFXThreadID gThreadIds[NRiTHREAD_MAX];

  /// are we currently forked
  static bool     gIsThreading = false;

  /// get an id for this thread
  static OFXThreadID getRawThreadID(void)
  {
    OFXThreadID thisThread;
#ifdef macos
    GetCurrentThread(&thisThread);
#else
    thisThread = pthread_self();
#endif
    return thisThread;
  }

  /// are we currently threading
  int multiThreadIsSpawnedThread()
  {
    return int(gIsThreading); // no mutex
  }

  /// get the number of processors that exist
  OfxStatus multiThreadNumCPUs(unsigned int *n)
  {
    *n =  NRiThread::m_nproc();
    return kOfxStatOK;
  }

  /// get the id of the current thread we are on, 0..GetNProcs-1
  OfxStatus multiThreadIndex(unsigned int *idx)
  {
    if(gIsThreading) {

      int nProcs = NRiThread::m_nproc();
      OFXThreadID tid = getRawThreadID();
      for(int i=0; i<nProcs; i++) {
        if (tid == gThreadIds[i]) {
          *idx = i;
          break;
        }
      }
    }
    else
      *idx = 0;
    return kOfxStatOK;
  }

  /// holds guff to pass into the shake threading call
  struct ThreadData {
    OfxThreadFunctionV1 *func;
    void *arg;
    unsigned int requestedThreads;
  };

  /// what is run in each thread
  static void RunThread(NRiThread* nri_thread_ptr, ThreadData* data)
  {
    int id = nri_thread_ptr->threadId;
    gThreadIds[id] = getRawThreadID();

    if(id < data->requestedThreads) 
      data->func(id, nri_thread_ptr->nThread, data->arg);
  
  }
  
  //void FnMpFork(FnMpForkFunc Func, int N, ...)
  OfxStatus multiThread(OfxThreadFunctionV1 func, unsigned int requestedThreads, void *customArg)
  {
    /// really should guard this
    if(gIsThreading)
      return kOfxStatErrExists;
    
#ifndef macos
    void (*conthandler)(int) = signal(SIGCONT,SIG_IGN);
    void (*stophandler)(int) = signal(SIGSTOP,SIG_IGN);
#endif
    
    // set up threading guff
    ThreadData threadData;
    threadData.func = func;
    threadData.arg  = customArg;
    threadData.requestedThreads = requestedThreads;

    gIsThreading = true;

    NRiThread::m_fork((NRiThreadFunc)RunThread, &threadData);

    gIsThreading = false;

#ifndef macos
    signal(SIGCONT,conthandler);
    signal(SIGSTOP,stophandler);
#endif

    return kOfxStatOK;
  }


}
