// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/OpaqueEndpointI.h>
#include <Ice/BasicStream.h>
#include <Ice/Exception.h>
#include <Ice/DefaultsAndOverrides.h>
#include <Ice/Base64.h>
#include <Ice/HashUtil.h>
#include <Ice/LocalException.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

IceInternal::OpaqueEndpointI::OpaqueEndpointI(vector<string>& args) :
    EndpointI(""), _rawEncoding(Encoding_1_0)
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

    int topt = 0;
    int vopt = 0;

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
            case 't':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -t option in endpoint `opaque" + str + "'";
                    throw ex;
                }
                istringstream p(argument);
                Ice::Int t;
                if(!(p >> t) || !p.eof())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "invalid type value `" + argument + "' in endpoint `opaque" + str + "'";
                    throw ex;
                }
                else if(t < 0 || t > 65535)
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "type value `" + argument + "' out of range in endpoint `opaque" + str + "'";
                    throw ex;
                }
                _type = static_cast<Ice::Short>(t);
                ++topt;
                if(topt > 1)
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "multiple -t options in endpoint `opaque" + str + "'";
                    throw ex;
                }
                break;
            }

            case 'v':
            {
                if(argument.empty())
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -v option in endpoint `opaque" + str + "'";
                    throw ex;
                }
                for(string::size_type i = 0; i < argument.size(); ++i)
                {
                    if(!Base64::isBase64(argument[i]))
                    {
                        EndpointParseException ex(__FILE__, __LINE__);
                        ostringstream ostr;
                        ostr << "invalid base64 character `" << argument[i] << "' (ordinal " << (int)argument[i]
                             << ") in endpoint `opaque" << str << "'";
                        ex.str = ostr.str();
                        throw ex;
                    }
                }
                const_cast<vector<Byte>&>(_rawBytes) = Base64::decode(argument);
                ++vopt;
                if(vopt > 1)
                {
                    EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "multiple -v options in endpoint `opaque" + str + "'";
                    throw ex;
                }
                break;
            }

            case 'e':
            {
                if(argument.empty())
                {
                    Ice::EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "no argument provided for -e option in endpoint `opaque" + str + "'";
                    throw ex;
                }

                try
                {
                    _rawEncoding = Ice::stringToEncodingVersion(argument);
                }
                catch(const Ice::VersionParseException& e)
                {
                    Ice::EndpointParseException ex(__FILE__, __LINE__);
                    ex.str = "invalid encoding version `" + argument + "' in endpoint `opaque" + str + "':\n" + e.str;
                    throw ex;
                }
                break;
            }

            default:
            {
                unknown.push_back(option);
                break;
            }
        }
    }

    if(topt != 1)
    {
        EndpointParseException ex(__FILE__, __LINE__);
        ex.str = "no -t option in endpoint `opaque" + str + "'";
        throw ex;
    }
    if(vopt != 1)
    {
        EndpointParseException ex(__FILE__, __LINE__);
        ex.str = "no -v option in endpoint `opaque" + str + "'";
        throw ex;
    }

    //
    // Replace argument vector with only those we didn't recognize.
    //
    args = unknown;
}

IceInternal::OpaqueEndpointI::OpaqueEndpointI(Short type, BasicStream* s) :
    EndpointI(""),
    _type(type)
{
    _rawEncoding = s->getReadEncoding();
    Int sz = s->getReadEncapsSize();
    s->readBlob(const_cast<vector<Byte>&>(_rawBytes), sz);
}

void
IceInternal::OpaqueEndpointI::startStreamWrite(BasicStream* s) const
{
    s->startWriteEncaps(_rawEncoding, DefaultFormat);
}

void
IceInternal::OpaqueEndpointI::streamWrite(BasicStream* s) const
{
    s->writeBlob(_rawBytes);
}

namespace
{

class InfoI : public Ice::OpaqueEndpointInfo
{
public:

    InfoI(Ice::Short type, const Ice::EncodingVersion& rawEncoding, const Ice::ByteSeq& rawByes);

    virtual Ice::Short
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
        return false;
    }

private:

    Ice::Short _type;
};


//
// COMPILERFIX: inlining this constructor causes crashes with gcc 4.0.1.
//
InfoI::InfoI(Ice::Short type, const Ice::EncodingVersion& rawEncoding, const Ice::ByteSeq& rawBytes) :
    Ice::OpaqueEndpointInfo(-1, false, rawEncoding, rawBytes),
    _type(type)
{
}

}

Ice::EndpointInfoPtr
IceInternal::OpaqueEndpointI::getInfo() const
{
    return new InfoI(_type, _rawEncoding, _rawBytes);
}

Short
IceInternal::OpaqueEndpointI::type() const
{
    return _type;
}

std::string
IceInternal::OpaqueEndpointI::protocol() const
{
    return "opaque";
}

Int
IceInternal::OpaqueEndpointI::timeout() const
{
    return -1;
}

EndpointIPtr
IceInternal::OpaqueEndpointI::timeout(Int) const
{
    return const_cast<OpaqueEndpointI*>(this);
}

EndpointIPtr
IceInternal::OpaqueEndpointI::connectionId(const string&) const
{
    return const_cast<OpaqueEndpointI*>(this);
}

bool
IceInternal::OpaqueEndpointI::compress() const
{
    return false;
}

EndpointIPtr
IceInternal::OpaqueEndpointI::compress(bool) const
{
    return const_cast<OpaqueEndpointI*>(this);
}

bool
IceInternal::OpaqueEndpointI::datagram() const
{
    return false;
}

bool
IceInternal::OpaqueEndpointI::secure() const
{
    return false;
}

TransceiverPtr
IceInternal::OpaqueEndpointI::transceiver(EndpointIPtr& endp) const
{
    endp = const_cast<OpaqueEndpointI*>(this);
    return 0;
}

vector<ConnectorPtr>
IceInternal::OpaqueEndpointI::connectors(Ice::EndpointSelectionType) const
{
    vector<ConnectorPtr> ret;
    return ret;
}

void
IceInternal::OpaqueEndpointI::connectors_async(Ice::EndpointSelectionType, const EndpointI_connectorsPtr& cb) const
{
    cb->connectors(vector<ConnectorPtr>());
}

AcceptorPtr
IceInternal::OpaqueEndpointI::acceptor(EndpointIPtr& endp, const string&) const
{
    endp = const_cast<OpaqueEndpointI*>(this);
    return 0;
}

vector<EndpointIPtr>
IceInternal::OpaqueEndpointI::expand() const
{
    vector<EndpointIPtr> endps;
    endps.push_back(const_cast<OpaqueEndpointI*>(this));
    return endps;
}

bool
IceInternal::OpaqueEndpointI::equivalent(const EndpointIPtr&) const
{
    return false;
}

bool
IceInternal::OpaqueEndpointI::operator==(const LocalObject& r) const
{
    const OpaqueEndpointI* p = dynamic_cast<const OpaqueEndpointI*>(&r);
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

    if(_rawEncoding != p->_rawEncoding)
    {
        return false;
    }

    if(_rawBytes != p->_rawBytes)
    {
        return false;
    }

    return true;
}

bool
IceInternal::OpaqueEndpointI::operator<(const LocalObject& r) const
{
    const OpaqueEndpointI* p = dynamic_cast<const OpaqueEndpointI*>(&r);
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

    if(_type < p->_type)
    {
        return true;
    }
    else if(p->_type < _type)
    {
        return false;
    }

    if(_rawEncoding < p->_rawEncoding)
    {
        return true;
    }
    else if(p->_rawEncoding < _rawEncoding)
    {
        return false;
    }

    if(_rawBytes < p->_rawBytes)
    {
        return true;
    }
    else if(p->_rawBytes < _rawBytes)
    {
        return false;
    }

    return false;
}

void
IceInternal::OpaqueEndpointI::hashInit(Ice::Int& h) const
{
    hashAdd(h, _rawEncoding.major);
    hashAdd(h, _rawEncoding.minor);
    hashAdd(h, _rawBytes);
}

string
IceInternal::OpaqueEndpointI::options() const
{
    ostringstream s;
    string val = Base64::encode(_rawBytes);
    s << " -t " << _type << " -e " << _rawEncoding << " -v " << val;
    return s.str();
}
