// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ENDPOINT_I_H
#define TEST_ENDPOINT_I_H

#include <Ice/EndpointI.h>
#include <Configuration.h>

class EndpointI : public IceInternal::EndpointI
{
public:

    static Ice::Short TYPE_BASE;

    virtual void streamWrite(IceInternal::BasicStream*) const;
    virtual Ice::Short type() const;
    virtual Ice::Int timeout() const;
    virtual IceInternal::EndpointIPtr timeout(Ice::Int) const;
    virtual IceInternal::EndpointIPtr connectionId(const ::std::string&) const;
    virtual bool compress() const;
    virtual IceInternal::EndpointIPtr compress(bool) const;
    virtual bool datagram() const;
    virtual bool secure() const;
    virtual bool unknown() const;
    virtual IceInternal::TransceiverPtr transceiver(IceInternal::EndpointIPtr&) const;
    virtual std::vector<IceInternal::ConnectorPtr> connectors() const;
    virtual void connectors_async(const IceInternal::EndpointI_connectorsPtr&) const;
    virtual IceInternal::AcceptorPtr acceptor(IceInternal::EndpointIPtr&, const std::string&) const;
    virtual std::vector<IceInternal::EndpointIPtr> expand() const;
    virtual bool equivalent(const IceInternal::EndpointIPtr&) const;

    virtual std::string toString() const;

    virtual bool operator==(const IceInternal::EndpointI&) const;
    virtual bool operator!=(const IceInternal::EndpointI&) const;
    virtual bool operator<(const IceInternal::EndpointI&) const;

protected:

#if !defined(_MSC_VER) || _MSC_VER > 1300
    using IceInternal::EndpointI::connectors;
#endif
    
private:

    EndpointI(const IceInternal::EndpointIPtr&);
    friend class EndpointFactory;

#if defined(__SUNPRO_CC) || defined(__HP_aCC)
    //
    // COMPILERFIX: prevent the compiler from emitting a warning about
    // hidding these operators.
    //
    using LocalObject::operator==;
    using LocalObject::operator<;
#endif

    const IceInternal::EndpointIPtr _endpoint;
    const ConfigurationPtr _configuration;
};

#endif
