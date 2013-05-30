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
        out << "trying to establish " << _protocol << " connection to " << toString();
    }

    try
    {
        TransceiverPtr del = _delegate->connect();
        return new TransceiverI(_instance, _type, del, _host, _port, _resource);
    }
    catch(const Ice::LocalException& ex)
    {
        if(_instance->networkTraceLevel() >= 2)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "failed to establish " << _protocol << " connection to " << toString() << "\n" << ex;
        }
        throw;
    }
}

Short
IceWS::ConnectorI::type() const
{
    return _type;
}

string
IceWS::ConnectorI::toString() const
{
    return _delegate->toString();
}

bool
IceWS::ConnectorI::operator==(const Connector& r) const
{
    const ConnectorI* p = dynamic_cast<const ConnectorI*>(&r);
    if(!p)
    {
        return false;
    }

    if(_delegate != p->_delegate)
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

    if(_delegate < p->_delegate)
    {
        return true;
    }
    else if(p->_delegate < _delegate)
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

    return false;
}

IceWS::ConnectorI::ConnectorI(const InstancePtr& instance, Short type, const IceInternal::ConnectorPtr& del,
                              const string& host, int port, const string& resource) :
    _instance(instance),
    _type(type),
    _delegate(del),
    _host(host),
    _port(port),
    _resource(resource),
    _protocol(type == WSEndpointType ? "ws" : "wss"),
    _logger(instance->communicator()->getLogger())
{
}

IceWS::ConnectorI::~ConnectorI()
{
}
