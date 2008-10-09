// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_SOCKET_READY_CALLBACK_H
#define ICE_SOCKET_READY_CALLBACK_H

#include <IceUtil/Timer.h>
#include <IceUtil/Handle.h>
#include <Ice/Network.h>
#include <Ice/SelectorF.h>

namespace IceInternal
{

class SocketReadyCallback : public IceUtil::TimerTask
{
public:
    
    SocketReadyCallback() : _status(Finished), _timeout(0)
    {
    }
    
    virtual SocketStatus socketReady() = 0;

    virtual void socketFinished() = 0;

    //
    // The selector thread doesn't unregister the callback when sockectTimeout is called; socketTimeout
    // must unregister the callback either explicitly with unregister() or by shutting down the socket 
    // (if necessary).
    //
    virtual void socketTimeout() = 0;

    virtual SOCKET fd() const = 0;
#ifdef ICE_APPLE_CFNETWORK
    virtual void* stream() const = 0;
#endif

private:

    void
    runTimerTask()
    {
        this->socketTimeout();
    }
    
    friend class SelectorThread;
    friend class Selector<SocketReadyCallback>;

    SocketStatus _status;
    int _timeout;
};
typedef IceUtil::Handle<SocketReadyCallback> SocketReadyCallbackPtr;

}

#endif
