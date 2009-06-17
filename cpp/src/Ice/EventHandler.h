// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_EVENT_HANDLER_H
#define ICE_EVENT_HANDLER_H

#include <IceUtil/Shared.h>
#include <Ice/EventHandlerF.h>
#include <Ice/InstanceF.h>
#include <Ice/ThreadPoolF.h>
#include <Ice/BasicStream.h>
#include <Ice/Network.h>
#include <Ice/SelectorF.h>

namespace Ice
{

class LocalException;

}

namespace IceInternal
{

class EventHandler : virtual public ::IceUtil::Shared
{
public:

    //
    // The handler is ready for the given operation.
    //
#ifdef ICE_USE_IOCP
    virtual bool startAsync(SocketOperation) = 0;
    virtual bool finishAsync(SocketOperation) = 0;
#endif

    virtual bool message(ThreadPoolCurrent&) = 0;

    //
    // Called if the event handler is unregistered.
    //
    virtual void finished() = 0;

    //
    // Get a textual representation of the event handler.
    //
    virtual std::string toString() const = 0;

    //
    // Get the selector information for the given status.
    //
    virtual NativeInfoPtr getNativeInfo() = 0;

protected:
    
    EventHandler();
    ICE_API virtual ~EventHandler();

#ifdef ICE_USE_IOCP
    SocketOperation _ready;
    SocketOperation _pending;
#else
    SocketOperation _disabled;
#endif
    SocketOperation _registered;

    friend class ThreadPool;
    friend class Selector;
#ifdef ICE_USE_CFSTREAM
    friend class EventHandlerWrapper;
#endif
};

class ThreadPoolWorkItem : virtual public IceUtil::Shared
{
public:
    
    virtual void execute() = 0;
};

}

#endif
