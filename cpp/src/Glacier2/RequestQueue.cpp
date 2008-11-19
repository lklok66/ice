// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Glacier2/RequestQueue.h>
#include <Glacier2/SessionRouterI.h>
#include <set>

using namespace std;
using namespace Ice;
using namespace Glacier2;

namespace
{

//
// AMI callback class for twoway requests
//
class AMI_Array_Object_ice_invokeI : public AMI_Array_Object_ice_invoke
{
public:
    
    AMI_Array_Object_ice_invokeI(const AMD_Array_Object_ice_invokePtr& amdCB,
                                 const InstancePtr& instance,
                                 const ConnectionPtr& connection) :
        _amdCB(amdCB),
        _instance(instance),
        _connection(connection)
    {
    }
    
    virtual void
    ice_response(bool ok, const pair<const Byte*, const Byte*>& outParams)
    {
        if(_amdCB)
        {
            _amdCB->ice_response(ok, outParams);
        }
    }

    virtual void
    ice_exception(const Exception& ex)
    {
        //
        // If the connection has been lost, destroy the session.
        //
        if(_connection)
        {
            try
            {
                ex.ice_throw();
            }
            catch(const Ice::ConnectionLostException&)
            {
                try
                {
                    _instance->sessionRouter()->destroySession(_connection);
                }
                catch(const Exception&)
                {
                }
            }
            catch(const Exception&)
            {
            }
        }
     
        if(_amdCB)
        {
            _amdCB->ice_exception(ex);
        }
    }

private:

    const AMD_Array_Object_ice_invokePtr _amdCB;
    const InstancePtr _instance;
    const ConnectionPtr _connection;
};

}

Glacier2::Request::Request(const ObjectPrx& proxy, const std::pair<const Byte*, const Byte*>& inParams,
                           const Current& current, bool forwardContext, const Ice::Context& sslContext,
                           const AMD_Array_Object_ice_invokePtr& amdCB, const ConnectionPtr& connection) :
    _proxy(proxy),
    _inParams(inParams.first, inParams.second),
    _current(current),
    _forwardContext(forwardContext),
    _sslContext(sslContext),
    _amdCB(amdCB),
    _connection(connection)
{
    //
    // If this is not a twoway call, we can finish the AMD call right
    // away.
    //
    if(!_proxy->ice_isTwoway())
    {
        _amdCB->ice_response(true, pair<const Byte*, const Byte*>(0, 0));
    }

    Context::const_iterator p = current.ctx.find("_ovrd");
    if(p != current.ctx.end())
    {
        const_cast<string&>(_override) = p->second;
    }
}


bool
Glacier2::Request::invoke(const InstancePtr& instance)
{
    pair<const Byte*, const Byte*> inPair;
    if(_inParams.size() == 0)
    {
        inPair.first = inPair.second = 0;
    }
    else
    {
        inPair.first = &_inParams[0];
        inPair.second = inPair.first + _inParams.size();
    }

    if(_proxy->ice_isBatchOneway() || _proxy->ice_isBatchDatagram())
    {
        ByteSeq outParams;
        if(_forwardContext)
        { 
            if(_sslContext.size() > 0)
            {
                Ice::Context ctx = _current.ctx;
                ctx.insert(_sslContext.begin(), _sslContext.end());
                _proxy->ice_invoke(_current.operation, _current.mode, inPair, outParams, ctx);
            }
            else
            {
                _proxy->ice_invoke(_current.operation, _current.mode, inPair, outParams, _current.ctx);
            }
        }
        else
        {
            if(_sslContext.size() > 0)
            {
                _proxy->ice_invoke(_current.operation, _current.mode, inPair, outParams,  _sslContext);
            }
            else
            {
                _proxy->ice_invoke(_current.operation, _current.mode, inPair, outParams);
            }
        }
        return true; // Batch invocation.
    }
    else
    {
        AMI_Array_Object_ice_invokePtr amiCB;
        if(_proxy->ice_isTwoway())
        {
            amiCB = new AMI_Array_Object_ice_invokeI(_amdCB, instance, _connection);
        }
        else
        {
            amiCB = new AMI_Array_Object_ice_invokeI(0, instance, _connection);
        }
        
        if(_forwardContext)
        { 
            if(_sslContext.size() > 0)
            {
                Ice::Context ctx = _current.ctx;
                ctx.insert(_sslContext.begin(), _sslContext.end());
                _proxy->ice_invoke_async(amiCB, _current.operation, _current.mode, inPair, ctx);
            }
            else
            {
                _proxy->ice_invoke_async(amiCB, _current.operation, _current.mode, inPair, _current.ctx);
            }
        }
        else
        {
            if(_sslContext.size() > 0)
            {
                _proxy->ice_invoke_async(amiCB, _current.operation, _current.mode, inPair, _sslContext);
            }
            else
            {
                _proxy->ice_invoke_async(amiCB, _current.operation, _current.mode, inPair);
            }
        }
        return false; // Not a batch invocation.
    }
}

bool
Glacier2::Request::override(const RequestPtr& other) const
{
    //
    // Both override values have to be non-empty.
    //
    if(_override.empty() || other->_override.empty())
    {
        return false;
    }

    //
    // Override does not work for twoways, because a response is
    // expected for each request.
    //
    if(_proxy->ice_isTwoway() || other->_proxy->ice_isTwoway())
    {
        return false;
    }

    //
    // We cannot override if the proxies differ.
    //
    if(_proxy != other->_proxy)
    {
        return false;
    }

    return _override == other->_override;
}

Glacier2::RequestQueue::RequestQueue(const RequestQueueThreadPtr& requestQueueThread,
                                     const InstancePtr& instance) :
    _requestQueueThread(requestQueueThread),
    _instance(instance)
{
}

bool
Glacier2::RequestQueue::addRequest(const RequestPtr& request)
{
    IceUtil::Mutex::Lock lock(*this);
    if(request->hasOverride())
    {
        for(vector<RequestPtr>::iterator p = _requests.begin(); p != _requests.end(); ++p)
        {
            //
            // If the new request overrides an old one, then abort the old
            // request and replace it with the new request.
            //
            if(request->override(*p))
            {
                *p = request;
                return true;
            }
        }
    }
    
    //
    // No override, we add the new request.
    //
    if(_requests.empty())
    {
        _requestQueueThread->flushRequestQueue(this); // This might throw if the thread is destroyed.
    }
    _requests.push_back(request);
    return false;
}

void
Glacier2::RequestQueue::flushRequests(set<Ice::ObjectPrx>& batchProxies)
{
    IceUtil::Mutex::Lock lock(*this);
    for(vector<RequestPtr>::const_iterator p = _requests.begin(); p != _requests.end(); ++p)
    {
        try
        {
            if((*p)->invoke(_instance)) // If batch invocation, add the proxy to the batch proxy set.
            {
                batchProxies.insert((*p)->getProxy());
            }
        }
        catch(const Ice::LocalException&)
        {
            // Ignore, this can occur for batch requests.
        }
    }
    _requests.clear();
}

Glacier2::RequestQueueThread::RequestQueueThread(const IceUtil::Time& sleepTime) :
    _sleepTime(sleepTime),
    _destroy(false),
    _sleep(false)
{
}

Glacier2::RequestQueueThread::~RequestQueueThread()
{
    assert(_destroy);
    assert(_queues.empty());
}

void 
Glacier2::RequestQueueThread::destroy()
{
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        
        assert(!_destroy);
        _destroy = true;
        _sleep = false;
        notify();
    }

    getThreadControl().join();
}

void
Glacier2::RequestQueueThread::flushRequestQueue(const RequestQueuePtr& queue)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if(_destroy)
    {
        throw Ice::ObjectNotExistException(__FILE__, __LINE__);
    }

    if(_queues.empty() && !_sleep)
    {
        notify();
    }
    _queues.push_back(queue);
}

void
Glacier2::RequestQueueThread::run()
{
    while(true)
    {
        vector<RequestQueuePtr> queues;

        {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

            //
            // Wait indefinitely if there's no requests to
            // send. If the queue is being destroyed we still need to
            // wait until all the responses for twoway requests are
            // received.
            //
            while(!_destroy && (_queues.empty() || _sleep))
            {
                if(_sleep)
                {
                    IceUtil::Time now = IceUtil::Time::now(IceUtil::Time::Monotonic);
                    if(!timedWait(_sleepDuration))
                    {
                        _sleepDuration = IceUtil::Time();
                    }
                    else
                    {
                        _sleepDuration -= IceUtil::Time::now(IceUtil::Time::Monotonic) - now;
                    }

                    if(_sleepDuration <= IceUtil::Time())
                    {
                        _sleep = false;
                    }
                }
                else
                {
                    wait();
                }
            }

            //
            // If the queue is being destroyed and there's no requests or responses 
            // to send, we're done.
            //
            if(_destroy && _queues.empty())
            {
                return;
            }

            assert(!_queues.empty() && !_sleep);
            
            queues.swap(_queues);

            if(_sleepTime > IceUtil::Time())
            {
                _sleep = true;
                _sleepDuration = _sleepTime;
            }
        }
        
        set<Ice::ObjectPrx> flushProxySet;
        for(vector<RequestQueuePtr>::const_iterator p = queues.begin(); p != queues.end(); ++p)
        {
            (*p)->flushRequests(flushProxySet);
        }

        set<Ice::ConnectionPtr> flushConnectionSet;
        for(set<Ice::ObjectPrx>::const_iterator q = flushProxySet.begin(); q != flushProxySet.end(); ++q)
        {
            //
            // As an optimization, we only flush the proxy batch requests if we didn't 
            // already flush the requests of a proxy which is using the same connection.
            //
            Ice::ConnectionPtr connection = (*q)->ice_getCachedConnection();
            if(!connection || flushConnectionSet.find(connection) == flushConnectionSet.end())
            {
                class FlushCB : public AMI_Object_ice_flushBatchRequests
                {
                public:

                    virtual void ice_exception(const Ice::Exception&) { } // Ignore.
                };
                (*q)->ice_flushBatchRequests_async(new FlushCB());

                if(connection)
                {
                    flushConnectionSet.insert(connection);
                }
            }
        }
    }
}
