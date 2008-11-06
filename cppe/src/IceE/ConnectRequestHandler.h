// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEE_CONNECT_REQUEST_HANDLER_H
#define ICEE_CONNECT_REQUEST_HANDLER_H

#include <IceE/Monitor.h>
#include <IceE/Mutex.h>

#include <IceE/RequestHandler.h>
#include <IceE/Reference.h>
#include <IceE/LocatorInfo.h>
#include <IceE/RouterInfo.h>
#include <IceE/ProxyF.h>
#include <IceE/BasicStream.h>

#include <memory>
#include <deque>

namespace IceInternal
{

class ConnectRequestHandler : public RequestHandler, 
#ifdef ICEE_HAS_LOCATOR
                              public LocatorInfo::GetEndpointsCallback,
#endif
#ifdef ICEE_HAS_ROUTER
                              public RouterInfo::GetClientEndpointsCallback,
#endif
                              public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    ConnectRequestHandler(const ReferencePtr&, const Ice::ObjectPrx&);
    virtual ~ConnectRequestHandler();

    RequestHandlerPtr connect();

#ifdef ICEE_HAS_BATCH
    virtual void prepareBatchRequest(BasicStream*);
    virtual void finishBatchRequest(BasicStream*);
    virtual void abortBatchRequest();
#endif

    virtual Ice::Connection* sendRequest(Outgoing*, bool);
#ifdef ICEE_HAS_AMI
    virtual bool sendAsyncRequest(const OutgoingAsyncPtr&, bool);
#endif

#ifdef ICEE_HAS_BATCH
    virtual bool flushBatchRequests(BatchOutgoing*);
#ifdef ICEE_HAS_AMI
    virtual bool flushAsyncBatchRequests(const BatchOutgoingAsyncPtr&);
#endif
#endif

    virtual Ice::ConnectionPtr getConnection(bool);

    //
    // Called by OutgoingConnectionFactory.
    //
    virtual void setConnection(const Ice::ConnectionPtr&); // XXX
    virtual void setException(const Ice::LocalException&); // XXX

    void flushRequestsWithException(const Ice::LocalException&);
    void flushRequestsWithException(const LocalExceptionWrapper&);

#ifdef ICEE_HAS_ROUTER
    //
    // Called by RouterInfo.
    //
    void routerInfoEndpoints(const std::vector<EndpointPtr>&);
    void routerInfoException(const Ice::LocalException& ex)
    {
        setException(ex);
    }
    void routerInfoAddedProxy();
#endif

#ifdef ICEE_HAS_LOCATOR
    //
    // Called by LocatorInfo.
    //
    void locatorInfoEndpoints(const std::vector<EndpointPtr>&, bool);
    void locatorInfoWellKnownObjectEndpoints(const std::vector<EndpointPtr>&, bool);
    void locatorInfoException(const Ice::LocalException&);
#endif

private:

    bool initialized();
    void flushRequests();

    struct Request
    {
#ifdef ICEE_HAS_AMI
        OutgoingAsyncPtr out;
#ifdef ICEE_HAS_BATCH
        BatchOutgoingAsyncPtr batchOut;
#endif
#endif
#ifdef ICEE_HAS_BATCH
        BasicStream* os;
#endif
    };

    ReferencePtr _reference;
    Ice::ObjectPrx _proxy;
#ifdef ICEE_HAS_LOCATOR
    bool _locatorInfoEndpoints;
    bool _locatorInfoCachedEndpoints;
#endif

    const bool _response;
#ifdef ICEE_HAS_BATCH
    const bool _batchAutoFlush;
#endif

    Ice::ConnectionPtr _connection;
    std::auto_ptr<Ice::LocalException> _exception;
    bool _initialized;
    bool _flushing;

    std::deque<Request> _requests;
#ifdef ICEE_HAS_BATCH
    bool _batchRequestInProgress;
    size_t _batchRequestsSize;
    BasicStream _batchStream;
#endif
    bool _updateRequestHandler;
};
typedef IceUtil::Handle<ConnectRequestHandler> ConnectRequestHandlerPtr;

}

#endif
