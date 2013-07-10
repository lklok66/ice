// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceWS/EndpointI.h>
#include <IceWS/AcceptorI.h>
#include <IceWS/ConnectorI.h>
#include <IceWS/TransceiverI.h>
#include <IceWS/Instance.h>
#include <Ice/BasicStream.h>
#include <Ice/LocalException.h>
#include <Ice/DefaultsAndOverrides.h>
#include <Ice/Object.h>
#include <Ice/HashUtil.h>

using namespace std;
using namespace Ice;
using namespace IceWS;

IceWS::EndpointI::EndpointI(const InstancePtr& instance, Short type, const IceInternal::EndpointIPtr& del,
                            const string& res) :
    IceInternal::EndpointI(del->connectionId()),
    _instance(instance),
    _type(type),
    _delegate(del),
    _resource(res)
{
    const_cast<IPEndpointInfoPtr&>(_info) = IPEndpointInfoPtr::dynamicCast(_delegate->getInfo());
    assert(_info);
}

IceWS::EndpointI::EndpointI(const InstancePtr& instance, Short type, const IceInternal::EndpointIPtr& del,
                            vector<string>& args, bool oaEndpoint) :
    IceInternal::EndpointI(""),
    _instance(instance),
    _type(type),
    _delegate(del)
{
    const_cast<IPEndpointInfoPtr&>(_info) = IPEndpointInfoPtr::dynamicCast(_delegate->getInfo());
    assert(_info);

    vector<string> unknown;

    ostringstream ostr;
    for(vector<string>::iterator p = args.begin(); p != args.end(); ++p)
    {
        if(p->find_first_of(" \t\n\r") != string::npos)
        {
            ostr << " \"" << *p << "\"";
        }
        else
        {
            ostr << " " << *p;
        }
    }
    const string str = ostr.str();

    for(vector<string>::size_type n = 0; n < args.size(); ++n)
    {
        string option = args[n];
        if(option.length() < 2 || option[0] != '-')
        {
            unknown.push_back(option);
            continue;
        }

        string argument;
        if(n + 1 < args.size() && args[n + 1][0] != '-')
        {
            argument = args[++n];
        }

        switch(option[1])
        {
            case 'r':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -r option in endpoint `" + protocol() + str +
                        _delegate->options() + "'";
                    throw ex;
                }
                const_cast<string&>(_resource) = argument;
                break;
            }

            default:
            {
                unknown.push_back(option);
                if(!argument.empty())
                {
                    unknown.push_back(argument);
                }
                break;
            }
        }
    }

    // TODO: Require a resource value?
    if(_resource.empty())
    {
        const_cast<string&>(_resource) = "/";
    }

    //
    // Replace argument vector with only those we didn't recognize.
    //
    args = unknown;
}

IceWS::EndpointI::EndpointI(const InstancePtr& instance, Short type, const IceInternal::EndpointIPtr& del,
                            IceInternal::BasicStream* s) :
    _instance(instance),
    _type(type),
    _delegate(del)
{
    const_cast<IPEndpointInfoPtr&>(_info) = IPEndpointInfoPtr::dynamicCast(_delegate->getInfo());
    assert(_info);

    s->read(const_cast<string&>(_resource), false);
}

void
IceWS::EndpointI::startStreamWrite(IceInternal::BasicStream* s) const
{
    s->startWriteEncaps();
}

void
IceWS::EndpointI::streamWrite(IceInternal::BasicStream* s) const
{
    _delegate->streamWrite(s);
    s->write(_resource, false);
}

void
IceWS::EndpointI::endStreamWrite(IceInternal::BasicStream* s) const
{
    s->endWriteEncaps();
}

Ice::EndpointInfoPtr
IceWS::EndpointI::getInfo() const
{
    class InfoI : public IceWS::EndpointInfo
    {
    public:

        InfoI(Short ty, Int to, bool comp, const string& host, Int port, const string& resource) :
            IceWS::EndpointInfo(to, comp, host, port, resource),
            _type(ty)
        {
        }

        virtual Short
        type() const
        {
            return _type;
        }

        virtual bool
        datagram() const
        {
            return false;
        }

        virtual bool
        secure() const
        {
            return _type == WSSEndpointType;
        }

    private:

        Short _type;
    };
    return new InfoI(_type, _delegate->timeout(), _delegate->compress(), _info->host, _info->port, _resource);
}

Short
IceWS::EndpointI::type() const
{
    return _type;
}

std::string
IceWS::EndpointI::protocol() const
{
    return _type == WSEndpointType ? "ws" : "wss";
}

Int
IceWS::EndpointI::timeout() const
{
    return _delegate->timeout();
}

IceInternal::EndpointIPtr
IceWS::EndpointI::timeout(Int timeout) const
{
    IceInternal::EndpointIPtr del = _delegate->timeout(timeout);
    if(del == _delegate)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(_instance, _type, del, _resource);
    }
}

IceInternal::EndpointIPtr
IceWS::EndpointI::connectionId(const string& connectionId) const
{
    IceInternal::EndpointIPtr del = _delegate->connectionId(connectionId);
    if(del == _delegate)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(_instance, _type, del, _resource);
    }
}

bool
IceWS::EndpointI::compress() const
{
    return _delegate->compress();
}

IceInternal::EndpointIPtr
IceWS::EndpointI::compress(bool compress) const
{
    IceInternal::EndpointIPtr del = _delegate->compress(compress);
    if(del == _delegate)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(_instance, _type, del, _resource);
    }
}

bool
IceWS::EndpointI::datagram() const
{
    return false;
}

bool
IceWS::EndpointI::secure() const
{
    return _type == WSSEndpointType;
}

IceInternal::TransceiverPtr
IceWS::EndpointI::transceiver(IceInternal::EndpointIPtr& endp) const
{
    endp = const_cast<EndpointI*>(this);
    return 0;
}

vector<IceInternal::ConnectorPtr>
IceWS::EndpointI::connectors(Ice::EndpointSelectionType selType) const
{
    return _instance->endpointHostResolver()->resolve(_info->host, _info->port, selType, const_cast<EndpointI*>(this));
}

void
IceWS::EndpointI::connectors_async(Ice::EndpointSelectionType selType,
                                   const IceInternal::EndpointI_connectorsPtr& callback) const
{
    _instance->endpointHostResolver()->resolve(_info->host, _info->port, selType, const_cast<EndpointI*>(this),
                                               callback);
}

IceInternal::AcceptorPtr
IceWS::EndpointI::acceptor(IceInternal::EndpointIPtr& endp, const string& adapterName) const
{
    IceInternal::EndpointIPtr delEndp;
    IceInternal::AcceptorPtr delAcc = _delegate->acceptor(delEndp, adapterName);

    endp = new EndpointI(_instance, _type, delEndp, _resource);

    return new AcceptorI(_instance, _type, delAcc, adapterName, _info->host, _info->port);
}

vector<IceInternal::EndpointIPtr>
IceWS::EndpointI::expand() const
{
    vector<IceInternal::EndpointIPtr> v = _delegate->expand();

    vector<IceInternal::EndpointIPtr> endps;
    if(v.empty())
    {
        endps.push_back(const_cast<EndpointI*>(this));
    }
    else
    {
        for(vector<IceInternal::EndpointIPtr>::const_iterator p = v.begin(); p != v.end(); ++p)
        {
            endps.push_back(new EndpointI(_instance, _type, *p, _resource));
        }
    }

    return endps;
}

bool
IceWS::EndpointI::equivalent(const IceInternal::EndpointIPtr& endpoint) const
{
    const EndpointI* wsEndpointI = dynamic_cast<const EndpointI*>(endpoint.get());
    if(!wsEndpointI)
    {
        return false;
    }
    return _delegate->equivalent(wsEndpointI->_delegate);
}

vector<IceInternal::ConnectorPtr>
IceWS::EndpointI::connectors(const vector<IceInternal::Address>& addresses,
                             const IceInternal::NetworkProxyPtr& proxy) const
{
    vector<IceInternal::ConnectorPtr> v = _delegate->connectors(addresses, proxy);

    vector<IceInternal::ConnectorPtr> connectors;
    for(vector<IceInternal::ConnectorPtr>::iterator p = v.begin(); p != v.end(); ++p)
    {
        connectors.push_back(new ConnectorI(_instance, _type, *p, _info->host, _info->port, _resource));
    }
    return connectors;
}

bool
IceWS::EndpointI::operator==(const Ice::LocalObject& r) const
{
    const EndpointI* p = dynamic_cast<const EndpointI*>(&r);
    if(!p)
    {
        return false;
    }

    if(this == p)
    {
        return true;
    }

    if(_type != p->_type)
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
IceWS::EndpointI::operator<(const Ice::LocalObject& r) const
{
    const EndpointI* p = dynamic_cast<const EndpointI*>(&r);
    if(!p)
    {
        const IceInternal::EndpointI* e = dynamic_cast<const IceInternal::EndpointI*>(&r);
        if(!e)
        {
            return false;
        }
        return type() < e->type();
    }

    if(this == p)
    {
        return false;
    }

    if(_type < p->_type)
    {
        return true;
    }
    else if (p->_type < _type)
    {
        return false;
    }

    if(_delegate < p->_delegate)
    {
        return true;
    }
    else if (p->_delegate < _delegate)
    {
        return false;
    }

    if(_resource < p->_resource)
    {
        return true;
    }
    else if (p->_resource < _resource)
    {
        return false;
    }

    return false;
}

void
IceWS::EndpointI::hashInit(Ice::Int& h) const
{
    _delegate->hashInit(h);
    IceInternal::hashAdd(h, _resource);
}

string
IceWS::EndpointI::options() const
{
    //
    // WARNING: Certain features, such as proxy validation in Glacier2,
    // depend on the format of proxy strings. Changes to toString() and
    // methods called to generate parts of the reference string could break
    // these features. Please review for all features that depend on the
    // format of proxyToString() before changing this and related code.
    //
    ostringstream s;

    s << _delegate->options();

    if(!_resource.empty())
    {
        s << " -r ";
        bool addQuote = _resource.find(':') != string::npos;
        if(addQuote)
        {
            s << "\"";
        }
        s << _resource;
        if(addQuote)
        {
            s << "\"";
        }
    }

    return s.str();
}

IceWS::EndpointFactoryI::EndpointFactoryI(const InstancePtr& instance, const IceInternal::EndpointFactoryPtr& del,
                                          int type, const string& prot) :
    _instance(instance),
    _delegate(del),
    _type(type),
    _protocol(prot)
{
}

IceWS::EndpointFactoryI::~EndpointFactoryI()
{
}

Short
IceWS::EndpointFactoryI::type() const
{
    return _type;
}

string
IceWS::EndpointFactoryI::protocol() const
{
    return _protocol;
}

IceInternal::EndpointIPtr
IceWS::EndpointFactoryI::create(vector<string>& args, bool oaEndpoint) const
{
    IceInternal::EndpointIPtr del = _delegate->create(args, oaEndpoint);
    return new EndpointI(_instance, _type, del, args, oaEndpoint);
}

IceInternal::EndpointIPtr
IceWS::EndpointFactoryI::read(IceInternal::BasicStream* s) const
{
    IceInternal::EndpointIPtr e = _delegate->read(s);
    return new EndpointI(_instance, _type, e, s);
}

void
IceWS::EndpointFactoryI::destroy()
{
    const_cast<InstancePtr&>(_instance) = 0;
}
