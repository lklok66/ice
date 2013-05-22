// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceWS/ConnectorI.h>
#include <IceWS/Instance.h>
#include <IceWS/TransceiverI.h>
#include <IceWS/EndpointI.h>
#include <IceWS/Util.h>
#include <Ice/Communicator.h>
#include <Ice/LocalException.h>
#include <Ice/LoggerUtil.h>

using namespace std;
using namespace Ice;
using namespace IceWS;
using namespace IceInternal;

TransceiverPtr
IceWS::ConnectorI::connect()
{
    if(_instance->networkTraceLevel() >= 2)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "trying to establish ws connection to " << toString();
    }

    try
    {
        return new TransceiverI(_instance, createSocket(false, _addr), _proxy, _host, _addr, _resource);
    }
    catch(const Ice::LocalException& ex)
    {
        if(_instance->networkTraceLevel() >= 2)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "failed to establish ws connection to " << toString() << "\n" << ex;
        }
        throw;
    }
}

Short
IceWS::ConnectorI::type() const
{
    return IceWS::EndpointType;
}

string
IceWS::ConnectorI::toString() const
{
    return addrToString(!_proxy ? _addr : _proxy->getAddress());
}

bool
IceWS::ConnectorI::operator==(const Connector& r) const
{
    const ConnectorI* p = dynamic_cast<const ConnectorI*>(&r);
    if(!p)
    {
        return false;
    }

    if(compareAddress(_addr, p->_addr) != 0)
    {
        return false;
    }

    if(_timeout != p->_timeout)
    {
        return false;
    }

    if(_connectionId != p->_connectionId)
    {
        return false;
    }

    if(_resource != p->_resource)
    {
        return false;
    }

    return true;
}

bool
IceWS::ConnectorI::operator!=(const Connector& r) const
{
    return !operator==(r);
}

bool
IceWS::ConnectorI::operator<(const Connector& r) const
{
    const ConnectorI* p = dynamic_cast<const ConnectorI*>(&r);
    if(!p)
    {
        return type() < r.type();
    }

    if(_timeout < p->_timeout)
    {
        return true;
    }
    else if(p->_timeout < _timeout)
    {
        return false;
    }

    if(_connectionId < p->_connectionId)
    {
        return true;
    }
    else if(p->_connectionId < _connectionId)
    {
        return false;
    }

    if(_resource < p->_resource)
    {
        return true;
    }
    else if(p->_resource < _resource)
    {
        return false;
    }

    return compareAddress(_addr, p->_addr) == -1;
}

IceWS::ConnectorI::ConnectorI(const InstancePtr& instance, const string& host, const Address& addr,
                              const NetworkProxyPtr& proxy, Ice::Int timeout, const string& connectionId,
                              const string& resource) :
    _instance(instance),
    _logger(instance->communicator()->getLogger()),
    _host(host),
    _addr(addr),
    _proxy(proxy),
    _timeout(timeout),
    _connectionId(connectionId),
    _resource(resource)
{
}

IceWS::ConnectorI::~ConnectorI()
{
}
