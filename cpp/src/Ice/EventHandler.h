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

    InstancePtr instance() const;
    
    //
    // Return true if the handler is for a datagram transport, false otherwise.
    //
    virtual bool datagram() const = 0;

    //
    // Return true if read() must be called before calling message().
    //
    virtual bool readable() const = 0;

    //
    // Read data via the event handler. May only be called if
    // readable() returns true.
    //
    virtual bool read(BasicStream&) = 0;

#if defined(ICE_APPLE_CFNETWORK)
    //
    // It's possible that the transceiver reads more data than what
    // was really asked. If this is the case, hasMoreData() returns
    // true and the handler read() method should be called again
    // (without doing a select()). This is handled by the Selector
    // class (it adds the handler to a separate list of handlers if
    // this method returns true.)
    //
    virtual bool hasMoreData() { return false; }
#endif

    //
    // A complete message has been received.
    //
    virtual void message(BasicStream&, const ThreadPoolPtr&) = 0;

    //
    // Will be called if the event handler is finally
    // unregistered. (Calling unregister() does not unregister
    // immediately.)
    //
    virtual void finished(const ThreadPoolPtr&) = 0;

    //
    // Propagate an exception to the event handler.
    //
    virtual void exception(const ::Ice::LocalException&) = 0;
    
    //
    // Get the file descriptor of the event handler.
    //
    virtual SOCKET fd() const = 0;

    //
    // Get a textual representation of the event handler.
    //
    virtual std::string toString() const = 0;

protected:
    
    EventHandler(const InstancePtr&);
    ICE_API virtual ~EventHandler();

    const InstancePtr _instance;

    //
    // The _stream data member is for use by ThreadPool or by the
    // connection for connection validation only.
    //
    BasicStream _stream;
    bool _serializing;
    bool _registered;
    friend class ThreadPool;
    friend class Selector<EventHandler>;
};

class ThreadPoolWorkItem : virtual public IceUtil::Shared
{
public:
    
    virtual void execute(const ThreadPoolPtr&) = 0;
};

}

#endif
