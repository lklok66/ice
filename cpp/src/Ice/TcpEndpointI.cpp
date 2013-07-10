// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/TcpEndpointI.h>
#include <Ice/Network.h>
#include <Ice/TcpAcceptor.h>
#include <Ice/TcpConnector.h>
#include <Ice/TcpTransceiver.h>
#include <Ice/BasicStream.h>
#include <Ice/LocalException.h>
#include <Ice/Instance.h>
#include <Ice/DefaultsAndOverrides.h>
#include <Ice/HashUtil.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

IceInternal::TcpEndpointI::TcpEndpointI(const InstancePtr& instance, const string& ho, Int po, Int ti,
                                        const string& conId, bool co) :
    EndpointI(conId),
    _instance(instance),
    _host(ho),
    _port(po),
    _timeout(ti),
    _compress(co)
{
}

IceInternal::TcpEndpointI::TcpEndpointI(const InstancePtr& instance, vector<string>& args, bool oaEndpoint) :
    EndpointI(""),
    _instance(instance),
    _port(0),
    _timeout(-1),
    _compress(false)
{
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
            case 'h':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -h option in endpoint `tcp" + str + "'";
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
                    ex.str = "no argument provided for -p option in endpoint `tcp" + str + "'";
                    throw ex;
                }
                istringstream p(argument);
                if(!(p >> const_cast<Int&>(_port)) || !p.eof())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "invalid port value `" + argument + "' in endpoint `tcp" + str + "'";
                    throw ex;
                }
                else if(_port < 0 || _port > 65535)
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "port value `" + argument + "' out of range in endpoint `tcp" + str + "'";
                    throw ex;
                }
                break;
            }

            case 't':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -t option in endpoint `tcp" + str + "'";
                    throw ex;
                }
                istringstream t(argument);
                if(!(t >> const_cast<Int&>(_timeout)) || !t.eof())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "invalid timeout value `" + argument + "' in endpoint `tcp" + str + "'";
                    throw ex;
                }
                break;
            }

            case 'z':
            {
                if(!argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "unexpected argument `" + argument + "' provided for -z option in `tcp" + str + "'";
                    throw ex;
                }
                const_cast<bool&>(_compress) = true;
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

    if(_host.empty())
    {
        const_cast<string&>(_host) = _instance->defaultsAndOverrides()->defaultHost;
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
            ex.str = "`-h *' not valid for proxy endpoint `tcp" + str + "'";
            throw ex;
        }
    }

    //
    // Replace argument vector with only those we didn't recognize.
    //
    args = unknown;
}

IceInternal::TcpEndpointI::TcpEndpointI(BasicStream* s) :
    _instance(s->instance()),
    _port(0),
    _timeout(-1),
    _compress(false)
{
    s->read(const_cast<string&>(_host), false);
    s->read(const_cast<Int&>(_port));
    s->read(const_cast<Int&>(_timeout));
    s->read(const_cast<bool&>(_compress));
}

void
IceInternal::TcpEndpointI::streamWrite(BasicStream* s) const
{
    s->write(_host, false);
    s->write(_port);
    s->write(_timeout);
    s->write(_compress);
}

EndpointInfoPtr
IceInternal::TcpEndpointI::getInfo() const
{
    class InfoI : public Ice::TCPEndpointInfo
    {
    public:

        InfoI(Ice::Int to, bool comp, const string& host, Ice::Int port) :
            TCPEndpointInfo(to, comp, host, port)
        {
        }

        virtual Ice::Short
        type() const
        {
            return TCPEndpointType;
        }

        virtual bool
        datagram() const
        {
            return false;
        }

        virtual bool
        secure() const
        {
            return false;
        }
    };

    return new InfoI(_timeout, _compress, _host, _port);
}

Short
IceInternal::TcpEndpointI::type() const
{
    return TCPEndpointType;
}

string
IceInternal::TcpEndpointI::protocol() const
{
    return "tcp";
}

Int
IceInternal::TcpEndpointI::timeout() const
{
    return _timeout;
}

EndpointIPtr
IceInternal::TcpEndpointI::timeout(Int timeout) const
{
    if(timeout == _timeout)
    {
        return const_cast<TcpEndpointI*>(this);
    }
    else
    {
        return new TcpEndpointI(_instance, _host, _port, timeout, _connectionId, _compress);
    }
}

EndpointIPtr
IceInternal::TcpEndpointI::connectionId(const string& connectionId) const
{
    if(connectionId == _connectionId)
    {
        return const_cast<TcpEndpointI*>(this);
    }
    else
    {
        return new TcpEndpointI(_instance, _host, _port, _timeout, connectionId, _compress);
    }
}

bool
IceInternal::TcpEndpointI::compress() const
{
    return _compress;
}

EndpointIPtr
IceInternal::TcpEndpointI::compress(bool compress) const
{
    if(compress == _compress)
    {
        return const_cast<TcpEndpointI*>(this);
    }
    else
    {
        return new TcpEndpointI(_instance, _host, _port, _timeout, _connectionId, compress);
    }
}

bool
IceInternal::TcpEndpointI::datagram() const
{
    return false;
}

bool
IceInternal::TcpEndpointI::secure() const
{
    return false;
}

TransceiverPtr
IceInternal::TcpEndpointI::transceiver(EndpointIPtr& endp) const
{
    endp = const_cast<TcpEndpointI*>(this);
    return 0;
}

vector<ConnectorPtr>
IceInternal::TcpEndpointI::connectors(EndpointSelectionType selType) const
{
    return _instance->endpointHostResolver()->resolve(_host, _port, selType, const_cast<TcpEndpointI*>(this));
}

void
IceInternal::TcpEndpointI::connectors_async(EndpointSelectionType selType, const EndpointI_connectorsPtr& cb) const
{
    _instance->endpointHostResolver()->resolve(_host, _port, selType, const_cast<TcpEndpointI*>(this), cb);
}

AcceptorPtr
IceInternal::TcpEndpointI::acceptor(EndpointIPtr& endp, const string&) const
{
    TcpAcceptor* p = new TcpAcceptor(_instance, _host, _port);
    endp = new TcpEndpointI(_instance, _host, p->effectivePort(), _timeout, _connectionId, _compress);
    return p;
}


vector<EndpointIPtr>
IceInternal::TcpEndpointI::expand() const
{
    vector<EndpointIPtr> endps;
    vector<string> hosts = getHostsForEndpointExpand(_host, _instance->protocolSupport(), false);
    if(hosts.empty())
    {
        endps.push_back(const_cast<TcpEndpointI*>(this));
    }
    else
    {
        for(vector<string>::const_iterator p = hosts.begin(); p != hosts.end(); ++p)
        {
            endps.push_back(new TcpEndpointI(_instance, *p, _port, _timeout, _connectionId, _compress));
        }
    }
    return endps;
}

bool
IceInternal::TcpEndpointI::equivalent(const EndpointIPtr& endpoint) const
{
    const TcpEndpointI* tcpEndpointI = dynamic_cast<const TcpEndpointI*>(endpoint.get());
    if(!tcpEndpointI)
    {
        return false;
    }
    return tcpEndpointI->_host == _host && tcpEndpointI->_port == _port;
}

vector<ConnectorPtr>
IceInternal::TcpEndpointI::connectors(const vector<Address>& addresses, const NetworkProxyPtr& proxy) const
{
    vector<ConnectorPtr> connectors;
    for(unsigned int i = 0; i < addresses.size(); ++i)
    {
        connectors.push_back(new TcpConnector(_instance, addresses[i], proxy, _timeout, _connectionId));
    }
    return connectors;
}

bool
IceInternal::TcpEndpointI::operator==(const LocalObject& r) const
{
    const TcpEndpointI* p = dynamic_cast<const TcpEndpointI*>(&r);
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

    return true;
}

bool
IceInternal::TcpEndpointI::operator<(const LocalObject& r) const
{
    const TcpEndpointI* p = dynamic_cast<const TcpEndpointI*>(&r);
    if(!p)
    {
        const EndpointI* e = dynamic_cast<const EndpointI*>(&r);
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

    return false;
}

void
IceInternal::TcpEndpointI::hashInit(Ice::Int& h) const
{
    hashAdd(h, _host);
    hashAdd(h, _port);
    hashAdd(h, _timeout);
    hashAdd(h, _connectionId);
    hashAdd(h, _compress);
}

string
IceInternal::TcpEndpointI::options() const
{
    //
    // WARNING: Certain features, such as proxy validation in Glacier2,
    // depend on the format of proxy strings. Changes to toString() and
    // methods called to generate parts of the reference string could break
    // these features. Please review for all features that depend on the
    // format of proxyToString() before changing this and related code.
    //
    ostringstream s;

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

    return s.str();
}

IceInternal::TcpEndpointFactory::TcpEndpointFactory(const InstancePtr& instance)
    : _instance(instance)
{
}

IceInternal::TcpEndpointFactory::~TcpEndpointFactory()
{
}

Short
IceInternal::TcpEndpointFactory::type() const
{
    return TCPEndpointType;
}

string
IceInternal::TcpEndpointFactory::protocol() const
{
    return "tcp";
}

EndpointIPtr
IceInternal::TcpEndpointFactory::create(vector<string>& args, bool oaEndpoint) const
{
    return new TcpEndpointI(_instance, args, oaEndpoint);
}

EndpointIPtr
IceInternal::TcpEndpointFactory::read(BasicStream* s) const
{
    return new TcpEndpointI(s);
}

void
IceInternal::TcpEndpointFactory::destroy()
{
    _instance = 0;
}
