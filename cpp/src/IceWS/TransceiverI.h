// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_WS_TRANSCEIVER_I_H
#define ICE_WS_TRANSCEIVER_I_H

#include <IceWS/InstanceF.h>
#include <IceWS/Plugin.h>
#include <IceWS/Util.h>

#include <Ice/LoggerF.h>
#include <Ice/StatsF.h>
#include <Ice/Transceiver.h>
#include <Ice/Network.h>
#include <Ice/Buffer.h>

namespace IceWS
{

class ConnectorI;
class AcceptorI;

class TransceiverI : public IceInternal::Transceiver, public IceInternal::NativeInfo
{
public:

    virtual IceInternal::NativeInfoPtr getNativeInfo();
#ifdef ICE_USE_IOCP
    virtual IceInternal::AsyncInfo* getAsyncInfo(IceInternal::SocketOperation);
#endif

    virtual IceInternal::SocketOperation initialize(IceInternal::Buffer&, IceInternal::Buffer&);
    virtual void close();
    virtual bool write(IceInternal::Buffer&);
    virtual bool read(IceInternal::Buffer&);
#ifdef ICE_USE_IOCP
    virtual bool startWrite(IceInternal::Buffer&);
    virtual void finishWrite(IceInternal::Buffer&);
    virtual void startRead(IceInternal::Buffer&);
    virtual void finishRead(IceInternal::Buffer&);
#endif
    virtual std::string type() const;
    virtual std::string toString() const;
    virtual Ice::ConnectionInfoPtr getInfo() const;
    virtual void checkSendSize(const IceInternal::Buffer&, size_t);

private:

    TransceiverI(const InstancePtr&, SOCKET, const IceInternal::NetworkProxyPtr&, const std::string&,
                 const IceInternal::Address&, const std::string&);
    TransceiverI(const InstancePtr&, SOCKET, const std::string&);
    virtual ~TransceiverI();

    void handleRequest(IceInternal::Buffer&);
    void handleResponse();
    IceInternal::Buffer& preRead(IceInternal::Buffer&);
    void postRead(IceInternal::Buffer&);

    IceInternal::Buffer& preWrite(IceInternal::Buffer&);
    void postWrite(IceInternal::Buffer&);

#ifdef ICE_USE_IOCP
    bool receive();
    bool send();
    int writeAsync(char*, int);
    int readAsync(char*, int);
#endif

    friend class ConnectorI;
    friend class AcceptorI;

    const InstancePtr _instance;
    const Ice::LoggerPtr _logger;
    const Ice::StatsPtr _stats;

    const IceInternal::NetworkProxyPtr _proxy;
    const std::string _host;
    const IceInternal::Address _addr;

    const std::string _adapterName;
    const bool _incoming;

    const std::string _resource;

    enum State
    {
        StateNeedConnect,
        StateConnectPending,
        StateProxyConnectRequest,
        StateProxyConnectRequestPending,
        StateConnected,
        StateRequestPending,
        StateResponsePending,
        StateHandshakeComplete,
        StateClosing
    };

    State _state;
    std::string _desc;

    HttpParserPtr _parser;
    std::string _key;

    enum ReadState
    {
        ReadStateInitializing,
        ReadStateStartFrame,
        ReadStateFrameOpcode,
        ReadStateFrameHeader,
        ReadStateHandleOpcode,
        ReadStateFramePayload
    };

    ReadState _readState;
    IceInternal::Buffer _readBuffer;
    IceInternal::Buffer::Container::iterator _readI;
    bool _lastFrame;
    int _opCode;
    size_t _readHeaderLength;
    size_t _payloadLength;
    unsigned char _mask[4];

    enum WriteState
    {
        WriteStateInitializing,
        WriteStateStartFrame,
        WriteStateHeader,
        WriteStatePayload
    };

    WriteState _writeState;
    IceInternal::Buffer _writeBuffer;
    IceInternal::Buffer::Container::iterator _writeI;
    size_t _writeHeaderLength;

#ifdef ICE_USE_IOCP
    int _maxSendPacketSize;
    int _maxReceivePacketSize;
    IceInternal::AsyncInfo _read;
    IceInternal::AsyncInfo _write;
#endif
};
typedef IceUtil::Handle<TransceiverI> TransceiverIPtr;

}

#endif
