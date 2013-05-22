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

IceWS::EndpointI::EndpointI(const InstancePtr& instance, const string& ho, Int po, Int ti, const string& conId,
                            bool co, const string& res) :
    IceInternal::EndpointI(conId),
    _instance(instance),
    _host(ho),
    _port(po),
    _timeout(ti),
    _compress(co),
    _resource(res)
{
}

IceWS::EndpointI::EndpointI(const InstancePtr& instance, const string& str, bool oaEndpoint) :
    IceInternal::EndpointI(""),
    _instance(instance),
    _port(0),
    _timeout(-1),
    _compress(false)
{
    const string delim = " \t\n\r";

    string::size_type beg;
    string::size_type end = 0;

    while(true)
    {
        beg = str.find_first_not_of(delim, end);
        if(beg == string::npos)
        {
            break;
        }

        end = str.find_first_of(delim, beg);
        if(end == string::npos)
        {
            end = str.length();
        }

        string option = str.substr(beg, end - beg);
        if(option.length() != 2 || option[0] != '-')
        {
            EndpointParseException ex(__FILE__, __LINE__);
            ex.str = "expected an endpoint option but found `" + option + "' in endpoint `ws " + str + "'";
            throw ex;
        }

        string argument;
        string::size_type argumentBeg = str.find_first_not_of(delim, end);
        if(argumentBeg != string::npos && str[argumentBeg] != '-')
        {
            beg = argumentBeg;
            end = str.find_first_of(delim, beg);
            if(end == string::npos)
            {
                end = str.length();
            }
            argument = str.substr(beg, end - beg);
            if(argument[0] == '\"' && argument[argument.size() - 1] == '\"')
            {
                argument = argument.substr(1, argument.size() - 2);
            }
        }

        switch(option[1])
        {
            case 'h':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -h option in endpoint `ws " + str + "'";
                    throw ex;
                }
                const_cast<string&>(_host) = argument;
                break;
            }

            case 'p':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -p option in endpoint `ws " + str + "'";
                    throw ex;
                }
                istringstream p(argument);
                if(!(p >> const_cast<Int&>(_port)) || !p.eof())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "invalid port value `" + argument + "' in endpoint `ws " + str + "'";
                    throw ex;
                }
                else if(_port < 0 || _port > 65535)
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "port value `" + argument + "' out of range in endpoint `ws " + str + "'";
                    throw ex;
                }
                break;
            }

            case 't':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -t option in endpoint `ws " + str + "'";
                    throw ex;
                }
                istringstream t(argument);
                if(!(t >> const_cast<Int&>(_timeout)) || !t.eof())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "invalid timeout value `" + argument + "' in endpoint `ws " + str + "'";
                    throw ex;
                }
                break;
            }

            case 'z':
            {
                if(!argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "unexpected argument `" + argument + "' provided for -z option in `ws " + str + "'";
                    throw ex;
                }
                const_cast<bool&>(_compress) = true;
                break;
            }

            case 'r':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -r option in endpoint `ws " + str + "'";
                    throw ex;
                }
                const_cast<string&>(_resource) = argument;
                break;
            }

            default:
            {
                Ice::EndpointParseException ex(__FILE__, __LINE__);
                ex.str = "unknown option `" + option + "' in endpoint `ws " + str + "'";
                throw ex;
            }
        }
    }

    if(_host.empty())
    {
        const_cast<string&>(_host) = _instance->defaultHost();
    }
    else if(_host == "*")
    {
        if(oaEndpoint)
        {
            const_cast<string&>(_host) = string();
        }
        else
        {
            EndpointParseException ex(__FILE__, __LINE__);
            ex.str = "`-h *' not valid for proxy endpoint `ws " + str + "'";
            throw ex;
        }
    }

    // TODO: Require a resource value?
    if(_resource.empty())
    {
        const_cast<string&>(_resource) = "/";
    }
}

IceWS::EndpointI::EndpointI(const InstancePtr& instance, IceInternal::BasicStream* s) :
    _instance(instance),
    _port(0),
    _timeout(-1),
    _compress(false)
{
    s->startReadEncaps();
    s->read(const_cast<string&>(_host), false);
    s->read(const_cast<Int&>(_port));
    s->read(const_cast<Int&>(_timeout));
    s->read(const_cast<bool&>(_compress));
    s->read(const_cast<string&>(_resource), false);
    s->endReadEncaps();
}

void
IceWS::EndpointI::streamWrite(IceInternal::BasicStream* s) const
{
    s->write(EndpointType);
    s->startWriteEncaps();
    s->write(_host, false);
    s->write(_port);
    s->write(_timeout);
    s->write(_compress);
    s->write(_resource, false);
    s->endWriteEncaps();
}

string
IceWS::EndpointI::toString() const
{
    //
    // WARNING: Certain features, such as proxy validation in Glacier2,
    // depend on the format of proxy strings. Changes to toString() and
    // methods called to generate parts of the reference string could break
    // these features. Please review for all features that depend on the
    // format of proxyToString() before changing this and related code.
    //
    ostringstream s;
    s << "ws";

    if(!_host.empty())
    {
        s << " -h ";
        bool addQuote = _host.find(':') != string::npos;
        if(addQuote)
        {
            s << "\"";
        }
        s << _host;
        if(addQuote)
        {
            s << "\"";
        }
    }

    s << " -p " << _port;

    if(_timeout != -1)
    {
        s << " -t " << _timeout;
    }

    if(_compress)
    {
        s << " -z";
    }

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

Ice::EndpointInfoPtr
IceWS::EndpointI::getInfo() const
{
    class InfoI : public IceWS::EndpointInfo
    {
    public:

        InfoI(Int to, bool comp, const string& host, Int port, const string& resource) :
            IceWS::EndpointInfo(to, comp, host, port, resource)
        {
        }

        virtual Short
        type() const
        {
            return EndpointType;
        }

        virtual bool
        datagram() const
        {
            return false;
        }

        virtual bool
        secure() const
        {
            return false; // TODO
        }
    };
    return new InfoI(_timeout, _compress, _host, _port, _resource);
}

Short
IceWS::EndpointI::type() const
{
    return EndpointType;
}

std::string
IceWS::EndpointI::protocol() const
{
    return "ws";
}

Int
IceWS::EndpointI::timeout() const
{
    return _timeout;
}

IceInternal::EndpointIPtr
IceWS::EndpointI::timeout(Int timeout) const
{
    if(timeout == _timeout)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(_instance, _host, _port, timeout, _connectionId, _compress, _resource);
    }
}

IceInternal::EndpointIPtr
IceWS::EndpointI::connectionId(const string& connectionId) const
{
    if(connectionId == _connectionId)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(_instance, _host, _port, _timeout, connectionId, _compress, _resource);
    }
}

bool
IceWS::EndpointI::compress() const
{
    return _compress;
}

IceInternal::EndpointIPtr
IceWS::EndpointI::compress(bool compress) const
{
    if(compress == _compress)
    {
        return const_cast<EndpointI*>(this);
    }
    else
    {
        return new EndpointI(_instance, _host, _port, _timeout, _connectionId, compress, _resource);
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
    return true;
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
    return _instance->endpointHostResolver()->resolve(_host, _port, selType, const_cast<EndpointI*>(this));
}

void
IceWS::EndpointI::connectors_async(Ice::EndpointSelectionType selType,
                                    const IceInternal::EndpointI_connectorsPtr& callback) const
{
    _instance->endpointHostResolver()->resolve(_host, _port, selType, const_cast<EndpointI*>(this), callback);
}

IceInternal::AcceptorPtr
IceWS::EndpointI::acceptor(IceInternal::EndpointIPtr& endp, const string& adapterName) const
{
    AcceptorI* p = new AcceptorI(_instance, adapterName, _host, _port);
    endp = new EndpointI(_instance, _host, p->effectivePort(), _timeout, _connectionId, _compress, _resource);
    return p;
}

vector<IceInternal::EndpointIPtr>
IceWS::EndpointI::expand() const
{
    vector<IceInternal::EndpointIPtr> endps;
    vector<string> hosts = IceInternal::getHostsForEndpointExpand(_host, _instance->protocolSupport(), false);
    if(hosts.empty())
    {
        endps.push_back(const_cast<EndpointI*>(this));
    }
    else
    {
        for(vector<string>::const_iterator p = hosts.begin(); p != hosts.end(); ++p)
        {
            endps.push_back(new EndpointI(_instance, *p, _port, _timeout, _connectionId, _compress, _resource));
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
    return wsEndpointI->_host == _host && wsEndpointI->_port == _port;
}

vector<IceInternal::ConnectorPtr>
IceWS::EndpointI::connectors(const vector<IceInternal::Address>& addresses,
                             const IceInternal::NetworkProxyPtr& proxy) const
{
    vector<IceInternal::ConnectorPtr> connectors;
    for(unsigned int i = 0; i < addresses.size(); ++i)
    {
        connectors.push_back(new ConnectorI(_instance, _host, addresses[i], proxy, _timeout, _connectionId, _resource));
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

    if(_host != p->_host)
    {
        return false;
    }

    if(_port != p->_port)
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

    if(_compress != p->_compress)
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

    if(_host < p->_host)
    {
        return true;
    }
    else if (p->_host < _host)
    {
        return false;
    }

    if(_port < p->_port)
    {
        return true;
    }
    else if(p->_port < _port)
    {
        return false;
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

    if(!_compress && p->_compress)
    {
        return true;
    }
    else if(p->_compress < _compress)
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

Ice::Int
IceWS::EndpointI::hashInit() const
{
    Int h = 5381;
    IceInternal::hashAdd(h, EndpointType);
    IceInternal::hashAdd(h, _host);
    IceInternal::hashAdd(h, _port);
    IceInternal::hashAdd(h, _timeout);
    IceInternal::hashAdd(h, _connectionId);
    IceInternal::hashAdd(h, _compress);
    IceInternal::hashAdd(h, _resource);
    return h;
}

IceWS::EndpointFactoryI::EndpointFactoryI(const InstancePtr& instance) :
    _instance(instance)
{
}

IceWS::EndpointFactoryI::~EndpointFactoryI()
{
}

Short
IceWS::EndpointFactoryI::type() const
{
    return EndpointType;
}

string
IceWS::EndpointFactoryI::protocol() const
{
    return "ws";
}

IceInternal::EndpointIPtr
IceWS::EndpointFactoryI::create(const string& str, bool oaEndpoint) const
{
    return new EndpointI(_instance, str, oaEndpoint);
}

IceInternal::EndpointIPtr
IceWS::EndpointFactoryI::read(IceInternal::BasicStream* s) const
{
    return new EndpointI(_instance, s);
}

void
IceWS::EndpointFactoryI::destroy()
{
    _instance = 0;
}
