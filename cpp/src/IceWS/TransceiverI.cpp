// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceWS/TransceiverI.h>
#include <IceWS/EndpointInfo.h>
#include <IceWS/ConnectionInfo.h>
#include <IceWS/Instance.h>
#include <IceWS/Util.h>
#include <Ice/Communicator.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Buffer.h>
#include <Ice/LocalException.h>
#include <Ice/Stats.h>
#include <Ice/Base64.h>
#include <IceUtil/Random.h>
#include <IceUtil/StringUtil.h>

#include <IceUtil/DisableWarnings.h>

#include <stdint.h>
#include <climits>

using namespace std;
using namespace Ice;
using namespace IceWS;
using namespace IceInternal;

//
// WebSocket opcodes
//
#define OP_CONT     0x0     // Continuation frame
#define OP_TEXT     0x1     // Text frame
#define OP_DATA     0x2     // Data frame
#define OP_RES_0x3  0x3     // Reserved
#define OP_RES_0x4  0x4     // Reserved
#define OP_RES_0x5  0x5     // Reserved
#define OP_RES_0x6  0x6     // Reserved
#define OP_RES_0x7  0x7     // Reserved
#define OP_CLOSE    0x8     // Connection close
#define OP_PING     0x9     // Ping
#define OP_PONG     0xA     // Pong
#define OP_RES_0xB  0xB     // Reserved
#define OP_RES_0xC  0xC     // Reserved
#define OP_RES_0xD  0xD     // Reserved
#define OP_RES_0xE  0xE     // Reserved
#define OP_RES_0xF  0xF     // Reserved
#define FLAG_FINAL  0x80    // Last frame
#define FLAG_MASKED 0x80    // Payload is masked

#define CLOSURE_NORMAL         1000
#define CLOSURE_SHUTDOWN       1001
#define CLOSURE_PROTOCOL_ERROR 1002
#define CLOSURE_TOO_BIG        1009

namespace
{

const string _iceProtocol = "ice.zeroc.com";
const string _wsUUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void htonll(Long v, Byte* dest)
{
    //
    // Transfer a 64-bit integer in network (big-endian) order.
    //
#ifdef ICE_BIG_ENDIAN
    const Byte* src = reinterpret_cast<const Byte*>(&v);
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#else
    const Byte* src = reinterpret_cast<const Byte*>(&v) + sizeof(Long) - 1;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest++ = *src--;
    *dest = *src;
#endif
}

Long nlltoh(const Byte* src)
{
    Long v;

    //
    // Extract a 64-bit integer in network (big-endian) order.
    //
#ifdef ICE_BIG_ENDIAN
    Byte* dest = reinterpret_cast<Byte*>(&v);
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest = *src;
#else
    Byte* dest = reinterpret_cast<Byte*>(&v) + sizeof(Long) - 1;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest-- = *src++;
    *dest = *src;
#endif

    return v;
}

}

NativeInfoPtr
IceWS::TransceiverI::getNativeInfo()
{
    return _delegate->getNativeInfo();
}

#ifdef ICE_USE_IOCP
AsyncInfo*
IceWS::TransceiverI::getAsyncInfo(SocketOperation status)
{
    return _delegate->getNativeInfo()->getAsyncInfo(status);
}
#endif

SocketOperation
IceWS::TransceiverI::initialize(Buffer& readBuffer, Buffer& writeBuffer, bool& hasMoreData)
{
    //
    // Delegate logs exceptions that occur during initialize(), so there's no need to trap them here.
    //
    if(_state == StateInitializeDelegate)
    {
        SocketOperation op = _delegate->initialize(readBuffer, writeBuffer, hasMoreData);
        if(op != SocketOperationNone)
        {
            return op;
        }
        _state = StateConnected;
    }

    try
    {
        if(_state == StateConnected)
        {
            //
            // We don't know how much we'll need to read.
            //
            _readBuffer.b.resize(1024);
            _readI = _readBuffer.i = _readBuffer.b.begin();

            //
            // The server waits for the client's upgrade request, the
            // client sends the upgrade request.
            //
            _state = StateRequestPending;
            if(!_incoming)
            {
                //
                // Compose the upgrade request.
                //
                ostringstream out;
                out << "GET " << _resource << " HTTP/1.1\r\n"
                    << "Host: " << _host << ":" << _port << "\r\n"
                    << "Upgrade: websocket\r\n"
                    << "Connection: Upgrade\r\n"
                    << "Sec-WebSocket-Protocol: " << _iceProtocol << "\r\n"
                    << "Sec-WebSocket-Version: 13\r\n"
                    << "Sec-WebSocket-Key: ";

                //
                // The value for Sec-WebSocket-Key is a 16-byte random number,
                // encoded with Base64.
                //
                vector<unsigned char> key(16);
                IceUtilInternal::generateRandom(reinterpret_cast<char*>(&key[0]), static_cast<int>(key.size()));
                _key = IceInternal::Base64::encode(key);
                out << _key << "\r\n\r\n"; // EOM

                string str = out.str();
                _writeBuffer.b.resize(str.size());
                memcpy(&_writeBuffer.b[0], str.c_str(), str.size());
                _writeBuffer.i = _writeBuffer.b.begin();
            }
        }

        //
        // Try to write the client's upgrade request.
        //
        if(_state == StateRequestPending && !_incoming)
        {
            if(_writeBuffer.i < _writeBuffer.b.end())
            {
                SocketOperation s = _delegate->write(_writeBuffer);
                if(s)
                {
                    return s;
                }
            }
            assert(_writeBuffer.i == _writeBuffer.b.end());
            _state = StateResponsePending;
        }

        while(true)
        {
            if(_readBuffer.i < _readBuffer.b.end())
            {
                SocketOperation s = _delegate->read(_readBuffer, hasMoreData);
                if(s == SocketOperationWrite || _readBuffer.i == _readBuffer.b.begin())
                {
                    return s;
                }
            }

            //
            // Try to read the client's upgrade request or the server's response.
            //
            if((_state == StateRequestPending && _incoming) || (_state == StateResponsePending && !_incoming))
            {
                //
                // Check if we have enough data for a complete message.
                //
                const Ice::Byte* p = _parser->isCompleteMessage(&_readBuffer.b[0], _readBuffer.i);
                if(!p)
                {
                    if(_readBuffer.i < _readBuffer.b.end())
                    {
                        return SocketOperationRead;
                    }

                    //
                    // Enlarge the buffer and try to read more.
                    //
                    const size_t oldSize = static_cast<size_t>(_readBuffer.i - _readBuffer.b.begin());
                    if(oldSize + 1024 > _instance->messageSizeMax())
                    {
                        throw MemoryLimitException(__FILE__, __LINE__);
                    }
                    _readBuffer.b.resize(oldSize + 1024);
                    _readBuffer.i = _readBuffer.b.begin() + oldSize;
                    continue; // Try again to read the response/request
                }

                //
                // Set _readI at the end of the response/request message.
                //
                _readI = _readBuffer.b.begin() + (p - &_readBuffer.b[0]);
            }

            //
            // We're done, the client's upgrade request or server's response is read.
            //
            break;
        }

        try
        {
            //
            // Parse the client's upgrade request.
            //
            if(_state == StateRequestPending && _incoming)
            {
                if(_parser->parse(&_readBuffer.b[0], _readI))
                {
                    handleRequest(_writeBuffer);
                    _state = StateResponsePending;
                }
                else
                {
                    throw ProtocolException(__FILE__, __LINE__, "incomplete request message");
                }
            }

            if(_state == StateResponsePending)
            {
                if(_incoming)
                {
                    if(_writeBuffer.i < _writeBuffer.b.end())
                    {
                        SocketOperation s = _delegate->write(_writeBuffer);
                        if(s)
                        {
                            return s;
                        }
                    }
                }
                else
                {
                    //
                    // Parse the server's response
                    //
                    if(_parser->parse(&_readBuffer.b[0], _readI))
                    {
                        handleResponse();
                    }
                    else
                    {
                        throw ProtocolException(__FILE__, __LINE__, "incomplete response message");
                    }
                }
            }
        }
        catch(const WebSocketException& ex)
        {
            throw ProtocolException(__FILE__, __LINE__, ex.reason);
        }

        _state = StateHandshakeComplete;

        hasMoreData |= _readI < _readBuffer.i;
    }
    catch(const Ice::LocalException& ex)
    {
        if(_instance->traceLevel() >= 2)
        {
            Trace out(_instance->logger(), _instance->traceCategory());
            out << protocol() << " connection HTTP upgrade request failed\n" << toString() << "\n" << ex;
        }
        throw;
    }

    if(_instance->traceLevel() >= 1)
    {
        Trace out(_instance->logger(), _instance->traceCategory());
        if(_incoming)
        {
            out << "accepted " << protocol() << " connection HTTP upgrade request\n" << toString();
        }
        else
        {
            out << protocol() << " connection HTTP upgrade request accepted\n" << toString();
        }
    }

    return SocketOperationNone;
}

SocketOperation
IceWS::TransceiverI::closing(bool initiator, const Ice::LocalException& reason)
{
    if(_instance->traceLevel() >= 1)
    {
        Trace out(_instance->logger(), _instance->traceCategory());
        out << "gracefully closing " << protocol() << " connection\n" << toString();
    }

    if(_state == StateClosingRequestPending && _closingInitiator)
    {
        //
        // If we initiated a close connection but also received a
        // close connection, we assume we didn't initiated the
        // connection and we send the close frame now. This is to
        // ensure that if both peers close the connection at the same
        // time we don't hang having both peer waiting for the close
        // frame of the other.
        //
        assert(!initiator);
        _closingInitiator = false;
        return SocketOperationWrite;
    }
    else if(_state >= StateClosingRequestPending)
    {
        return SocketOperationNone;
    }

    _closingInitiator = initiator;
    if(dynamic_cast<const Ice::CloseConnectionException*>(&reason))
    {
        _closingReason = CLOSURE_NORMAL;
    }
    else if(dynamic_cast<const Ice::ObjectAdapterDeactivatedException*>(&reason) ||
            dynamic_cast<const Ice::CommunicatorDestroyedException*>(&reason))
    {
        _closingReason = CLOSURE_SHUTDOWN;
    }
    else if(dynamic_cast<const Ice::ProtocolException*>(&reason))
    {
        _closingReason  = CLOSURE_PROTOCOL_ERROR;
    }
    else if(dynamic_cast<const Ice::MemoryLimitException*>(&reason))
    {
        _closingReason = CLOSURE_TOO_BIG;
    }

    _state = StateClosingRequestPending;
    return initiator ? SocketOperationRead : SocketOperationWrite;
}

void
IceWS::TransceiverI::close()
{
    _delegate->close();
}

SocketOperation
IceWS::TransceiverI::write(Buffer& buf)
{
    if(_state < StateHandshakeComplete)
    {
        if(_state < StateConnected)
        {
            return _delegate->write(buf);
        }
        else
        {
            return _delegate->write(_writeBuffer);
        }
    }
    else if(_state > StateHandshakeComplete)
    {
        if(!_closingInitiator && _state == StateClosingResponsePending)
        {
            return SocketOperationRead;
        }
    }

    //
    // Nothing to do if the buffer was sent and unless we are closing
    // the connection in which case we might need to send the close
    // frame.
    //
    if(!buf.b.empty() && buf.i == buf.b.end() && _state == StateHandshakeComplete)
    {
        return SocketOperationNone;
    }

    do
    {
        preWrite(buf);

        if(_writeBuffer.i < _writeBuffer.b.end())
        {
            SocketOperation s = _delegate->write(_writeBuffer);
            if(s)
            {
                return s;
            }
        }

        if(_incoming && !buf.b.empty())
        {
            SocketOperation s = _delegate->write(buf);
            if(s)
            {
                return s;
            }
        }

        SocketOperation s = postWrite(buf);
        if(s)
        {
            return s;
        }
    }
    while(!buf.b.empty() && buf.i != buf.b.end());

    return SocketOperationNone;
}

SocketOperation
IceWS::TransceiverI::read(Buffer& buf, bool& hasMoreData)
{
    if(_state < StateHandshakeComplete)
    {
        if(_state < StateConnected)
        {
            return _delegate->read(buf, hasMoreData);
        }
        else
        {
            if(_delegate->read(_readBuffer, hasMoreData) == SocketOperationWrite)
            {
                return SocketOperationWrite;
            }
            else
            {
                return SocketOperationNone;
            }
        }
    }
    else if(_state > StateHandshakeComplete)
    {
        if(_closingInitiator && _state == StateClosingResponsePending)
        {
            return SocketOperationWrite;
        }
    }

    if(!buf.b.empty() && buf.i == buf.b.end())
    {
        return SocketOperationNone;
    }

    do
    {
        SocketOperation s = preRead(buf);
        if(s == SocketOperationWrite)
        {
            return s;
        }

        if(buf.i < buf.b.end())
        {
            if(_readState == ReadStatePayload)
            {
                SocketOperation s = _delegate->read(buf, hasMoreData);
                if(s)
                {
                    postRead(buf);
                    return s;
                }
            }
            else
            {
                SocketOperation s = _delegate->read(_readBuffer, hasMoreData);
                if(s == SocketOperationWrite)
                {
                    return s;
                }
            }
        }
    }
    while(postRead(buf) && buf.i != buf.b.end());
    hasMoreData = _readI < _readBuffer.i;
    return buf.i == buf.b.end() ? SocketOperationNone : SocketOperationRead;
}

#ifdef ICE_USE_IOCP
bool
IceWS::TransceiverI::startWrite(Buffer& buf)
{
    if(_state < StateHandshakeComplete)
    {
        if(_state < StateConnected)
        {
            return _delegate->startWrite(buf);
        }
        else
        {
            return _delegate->startWrite(buf);
        }
    }

    preWrite(buf);

    if(_writeBuffer.i < _writeBuffer.b.end())
    {
        _delegate->startWrite(_writeBuffer);
        return false;
    }
    else
    {
        assert(_incoming);
        return _delegate->startWrite(buf);
    }
}

void
IceWS::TransceiverI::finishWrite(Buffer& buf)
{
    if(_state < StateHandshakeComplete)
    {
        if(_state < StateConnected)
        {
            _delegate->finishWrite(buf);
        }
        else
        {
            _delegate->finishWrite(_writeBuffer);
        }
        return;
    }

    if(_writeBuffer.i < _writeBuffer.b.end())
    {
        _delegate->finishWrite(_writeBuffer);
    }
    else
    {
        assert(_incoming);
        _delegate->finishWrite(buf);
    }

    postWrite(buf);
}

void
IceWS::TransceiverI::startRead(Buffer& buf)
{
    if(_state < StateHandshakeComplete)
    {
        if(_state < StateConnected)
        {
            _delegate->startRead(buf);
        }
        else
        {
            _delegate->startRead(_readBuffer);
        }
        return;
    }

    SocketOperation op = preRead(buf);

    if(buf.i == buf.b.end() || op != SocketOperationRead)
    {
        _delegate->getNativeInfo()->completed(IceInternal::SocketOperationRead);
    }
    else if(_readState == ReadStatePayload)
    {
        _delegate->startRead(buf);
    }
    else
    {
        _delegate->startRead(_readBuffer);
    }
}

void
IceWS::TransceiverI::finishRead(Buffer& buf)
{
    if(_state < StateHandshakeComplete)
    {
        if(_state < StateConnected)
        {
            _delegate->finishRead(buf);
        }
        else
        {
            _delegate->finishRead(_readBuffer);
        }
        return;
    }

    if(buf.i == buf.b.end())
    {
        // Nothing to do.
    }
    else if(_readState == ReadStatePayload)
    {
        _delegate->finishRead(buf);
    }
    else
    {
        _delegate->finishRead(_readBuffer);
    }
    postRead(buf);
}
#endif

string
IceWS::TransceiverI::protocol() const
{
    return _instance->protocol();
}

string
IceWS::TransceiverI::toString() const
{
    return _delegate->toString();
}

Ice::ConnectionInfoPtr
IceWS::TransceiverI::getInfo() const
{
    IPConnectionInfoPtr di = IPConnectionInfoPtr::dynamicCast(_delegate->getInfo());
    assert(di);
    IceWS::ConnectionInfoPtr info = new IceWS::ConnectionInfo();
    info->localAddress = di->localAddress;
    info->localPort = di->localPort;
    info->remoteAddress = di->remoteAddress;
    info->remotePort = di->remotePort;
    return info;
}

void
IceWS::TransceiverI::checkSendSize(const Buffer& buf, size_t messageSizeMax)
{
    _delegate->checkSendSize(buf, messageSizeMax);
}

IceWS::TransceiverI::TransceiverI(const InstancePtr& instance, const TransceiverPtr& del,
                                  const string& host, int port, const string& resource) :
    _instance(instance),
    _delegate(del),
    _host(host),
    _port(port),
    _resource(resource),
    _incoming(false),
    _state(StateInitializeDelegate),
    _parser(new HttpParser),
    _readState(ReadStateOpcode),
    _readBuffer(0),
    _readBufferSize(1024),
    _readLastFrame(false),
    _readOpCode(0),
    _readHeaderLength(0),
    _readPayloadLength(0),
    _writeState(WriteStateHeader),
    _writeBuffer(0),
    _writeBufferSize(1024)
{
    //
    // For client connections, the sent frame payload must be
    // masked. So we copy and send the message buffer data in chuncks
    // of data whose size is up to the write buffer size.
    //
    const_cast<size_t&>(_writeBufferSize) = max(IceInternal::getSendBufferSize(del->getNativeInfo()->fd()), 1024);

    //
    // Write and read buffer size must be large enough to hold the frame header!
    //
    assert(_writeBufferSize > 256);
    assert(_readBufferSize > 256);
}

IceWS::TransceiverI::TransceiverI(const InstancePtr& instance, const TransceiverPtr& del) :
    _instance(instance),
    _delegate(del),
    _port(-1),
    _incoming(true),
    _state(StateInitializeDelegate),
    _parser(new HttpParser),
    _readState(ReadStateOpcode),
    _readBuffer(0),
    _readBufferSize(1024),
    _readLastFrame(false),
    _readOpCode(0),
    _readHeaderLength(0),
    _readPayloadLength(0),
    _writeState(WriteStateHeader),
    _writeBuffer(0),
    _writeBufferSize(1024)
{
    //
    // Write and read buffer size must be large enough to hold the frame header!
    //
    assert(_writeBufferSize > 256);
    assert(_readBufferSize > 256);
}

IceWS::TransceiverI::~TransceiverI()
{
}

void
IceWS::TransceiverI::handleRequest(Buffer& responseBuffer)
{
    string val;

    //
    // HTTP/1.1
    //
    if(_parser->versionMajor() != 1 || _parser->versionMinor() != 1)
    {
        throw WebSocketException("unsupported HTTP version");
    }

    //
    // "An |Upgrade| header field containing the value 'websocket',
    //  treated as an ASCII case-insensitive value."
    //
    if(!_parser->getHeader("Upgrade", val, true))
    {
        throw WebSocketException("missing value for Upgrade field");
    }
    else if(val != "websocket")
    {
        throw WebSocketException("invalid value `" + val + "' for Upgrade field");
    }

    //
    // "A |Connection| header field that includes the token 'Upgrade',
    //  treated as an ASCII case-insensitive value.
    //
    if(!_parser->getHeader("Connection", val, true))
    {
        throw WebSocketException("missing value for Connection field");
    }
    else if(val.find("upgrade") == string::npos)
    {
        throw WebSocketException("invalid value `" + val + "' for Connection field");
    }

    //
    // "A |Sec-WebSocket-Version| header field, with a value of 13."
    //
    if(!_parser->getHeader("Sec-WebSocket-Version", val, false))
    {
        throw WebSocketException("missing value for WebSocket version");
    }
    else if(val != "13")
    {
        throw WebSocketException("unsupported WebSocket version `" + val + "'");
    }

    //
    // "Optionally, a |Sec-WebSocket-Protocol| header field, with a list
    //  of values indicating which protocols the client would like to
    //  speak, ordered by preference."
    //
    bool addProtocol = false;
    if(_parser->getHeader("Sec-WebSocket-Protocol", val, true))
    {
        vector<string> protocols;
        if(!IceUtilInternal::splitString(val, ",", protocols))
        {
            throw WebSocketException("invalid value `" + val + "' for WebSocket protocol");
        }
        for(vector<string>::iterator p = protocols.begin(); p != protocols.end(); ++p)
        {
            if(IceUtilInternal::trim(*p) != _iceProtocol)
            {
                throw WebSocketException("unknown value `" + *p + "' for WebSocket protocol");
            }
            addProtocol = true;
        }
    }

    //
    // "A |Sec-WebSocket-Key| header field with a base64-encoded
    //  value that, when decoded, is 16 bytes in length."
    //
    string key;
    if(!_parser->getHeader("Sec-WebSocket-Key", key, false))
    {
        throw WebSocketException("missing value for WebSocket key");
    }

    vector<unsigned char> decodedKey = Base64::decode(key);
    if(decodedKey.size() != 16)
    {
        throw WebSocketException("invalid value `" + key + "' for WebSocket key");
    }

    //
    // Retain the target resource.
    //
    const_cast<string&>(_resource) = _parser->uri();

    //
    // Compose the response.
    //
    ostringstream out;
    out << "HTTP/1.1 101 Switching Protocols\r\n"
        << "Upgrade: websocket\r\n"
        << "Connection: Upgrade\r\n";
    if(addProtocol)
    {
        out << "Sec-WebSocket-Protocol: " << _iceProtocol << "\r\n";
    }

    //
    // The response includes:
    //
    // "A |Sec-WebSocket-Accept| header field.  The value of this
    //  header field is constructed by concatenating /key/, defined
    //  above in step 4 in Section 4.2.2, with the string "258EAFA5-
    //  E914-47DA-95CA-C5AB0DC85B11", taking the SHA-1 hash of this
    //  concatenated value to obtain a 20-byte value and base64-
    //  encoding (see Section 4 of [RFC4648]) this 20-byte hash.
    //
    out << "Sec-WebSocket-Accept: ";
    string input = key + _wsUUID;
    vector<unsigned char> v(&input[0], &input[0] + input.size());
    vector<unsigned char> hash = calcSHA1(v);
    out << IceInternal::Base64::encode(hash) << "\r\n" << "\r\n"; // EOM

    string str = out.str();
    responseBuffer.b.resize(str.size());
    memcpy(&responseBuffer.b[0], str.c_str(), str.size());
    responseBuffer.i = responseBuffer.b.begin();
}

void
IceWS::TransceiverI::handleResponse()
{
    string val;

    //
    // HTTP/1.1
    //
    if(_parser->versionMajor() != 1 || _parser->versionMinor() != 1)
    {
        throw WebSocketException("unsupported HTTP version");
    }

    //
    // "If the status code received from the server is not 101, the
    //  client handles the response per HTTP [RFC2616] procedures.  In
    //  particular, the client might perform authentication if it
    //  receives a 401 status code; the server might redirect the client
    //  using a 3xx status code (but clients are not required to follow
    //  them), etc."
    //
    if(_parser->status() != 101)
    {
        ostringstream out;
        out << "unexpected status value " << _parser->status();
        if(!_parser->reason().empty())
        {
            out << ":" << endl << _parser->reason();
        }
        throw WebSocketException(out.str());
    }

    //
    // "If the response lacks an |Upgrade| header field or the |Upgrade|
    //  header field contains a value that is not an ASCII case-
    //  insensitive match for the value "websocket", the client MUST
    //  _Fail the WebSocket Connection_."
    //
    if(!_parser->getHeader("Upgrade", val, true))
    {
        throw WebSocketException("missing value for Upgrade field");
    }
    else if(val != "websocket")
    {
        throw WebSocketException("invalid value `" + val + "' for Upgrade field");
    }

    //
    // "If the response lacks a |Connection| header field or the
    //  |Connection| header field doesn't contain a token that is an
    //  ASCII case-insensitive match for the value "Upgrade", the client
    //  MUST _Fail the WebSocket Connection_."
    //
    if(!_parser->getHeader("Connection", val, true))
    {
        throw WebSocketException("missing value for Connection field");
    }
    else if(val.find("upgrade") == string::npos)
    {
        throw WebSocketException("invalid value `" + val + "' for Connection field");
    }

    //
    // "If the response includes a |Sec-WebSocket-Protocol| header field
    //  and this header field indicates the use of a subprotocol that was
    //  not present in the client's handshake (the server has indicated a
    //  subprotocol not requested by the client), the client MUST _Fail
    //  the WebSocket Connection_."
    //
    if(_parser->getHeader("Sec-WebSocket-Protocol", val, true) && val != _iceProtocol)
    {
        throw WebSocketException("invalid value `" + val + "' for WebSocket protocol");
    }

    //
    // "If the response lacks a |Sec-WebSocket-Accept| header field or
    //  the |Sec-WebSocket-Accept| contains a value other than the
    //  base64-encoded SHA-1 of the concatenation of the |Sec-WebSocket-
    //  Key| (as a string, not base64-decoded) with the string "258EAFA5-
    //  E914-47DA-95CA-C5AB0DC85B11" but ignoring any leading and
    //  trailing whitespace, the client MUST _Fail the WebSocket
    //  Connection_."
    //
    if(!_parser->getHeader("Sec-WebSocket-Accept", val, false))
    {
        throw WebSocketException("missing value for Sec-WebSocket-Accept");
    }
    string input = _key + _wsUUID;
    vector<unsigned char> v(&input[0], &input[0] + input.size());
    vector<unsigned char> hash = calcSHA1(v);
    if(val != IceInternal::Base64::encode(hash))
    {
        throw WebSocketException("invalid value `" + val + "' for Sec-WebSocket-Accept");
    }
}

SocketOperation
IceWS::TransceiverI::preRead(Buffer& buf)
{
    if(_readState == ReadStateOpcode)
    {
        //
        // Is there enough data available to read the opcode?
        //
        if(!readBuffered(2))
        {
            _readStart = _readBuffer.i;
            return SocketOperationRead;
        }

        //
        // Most-significant bit indicates whether this is the
        // last frame. Least-significant four bits hold the
        // opcode.
        //
        unsigned char ch = static_cast<unsigned char>(*_readI++);
        _readLastFrame = (ch & FLAG_FINAL) == FLAG_FINAL;
        _readOpCode = ch & 0xf;

        ch = static_cast<unsigned char>(*_readI++);

        //
        // Check the MASK bit. Messages sent by a client must be masked;
        // messages sent by a server must not be masked.
        //
        const bool masked = (ch & FLAG_MASKED) == FLAG_MASKED;
        if(masked != _incoming)
        {
            throw ProtocolException(__FILE__, __LINE__, "invalid masking");
        }

        //
        // Extract the payload length, which can have the following values:
        //
        // 0-125: The payload length
        // 126:   The subsequent two bytes contain the payload length
        // 127:   The subsequent eight bytes contain the payload length
        //
        _readPayloadLength = (ch & 0x7f);
        if(_readPayloadLength < 126)
        {
            _readHeaderLength = 0;
        }
        else if(_readPayloadLength == 126)
        {
            _readHeaderLength = 2; // Need to read a 16-bit payload length.
        }
        else
        {
            _readHeaderLength = 8; // Need to read a 64-bit payload length.
        }
        if(masked)
        {
            _readHeaderLength += 4; // Need to read a 32-bit mask.
        }

        _readState = ReadStateHeader;
    }

    if(_readState == ReadStateHeader)
    {
        //
        // Is there enough data available to read the header?
        //
        if(_readHeaderLength > 0 && !readBuffered(_readHeaderLength))
        {
            _readStart = _readBuffer.i;
            return SocketOperationRead;
        }

        if(_readPayloadLength == 126)
        {
            _readPayloadLength = static_cast<size_t>(ntohs(*reinterpret_cast<uint16_t*>(_readI)));
            _readI += 2;
        }
        else if(_readPayloadLength == 127)
        {
            assert(_readPayloadLength == 127);
            Long l = nlltoh(_readI);
            _readI += 8;
            if(l < 0 || l > INT_MAX)
            {
                ostringstream ostr;
                ostr << "invalid WebSocket payload length: " << l;
                throw ProtocolException(__FILE__, __LINE__, ostr.str());
            }
            _readPayloadLength = static_cast<size_t>(l);
        }

        //
        // Read the mask if this is an incoming connection.
        //
        if(_incoming)
        {
            assert(_readBuffer.i - _readI >= 4); // We must have needed to read the mask.
            memcpy(_readMask, _readI, 4); // Copy the mask.
            _readI += 4;
        }

        switch(_readOpCode)
        {
        case OP_CONT: // Continuation frame
        {
            // TODO: Add support for continuation frames?
            throw ProtocolException(__FILE__, __LINE__, "continuation frames not supported");
        }
        case OP_TEXT: // Text frame
        {
            throw ProtocolException(__FILE__, __LINE__, "text frames not supported");
        }
        case OP_DATA: // Data frame
        {
            if(!_readLastFrame)
            {
                throw ProtocolException(__FILE__, __LINE__, "continuation frames not supported");
            }
            if(_readPayloadLength <= 0)
            {
                throw ProtocolException(__FILE__, __LINE__, "payload length is 0");
            }
            break;
        }
        case OP_CLOSE: // Connection close
        {
            if(_instance->traceLevel() >= 2)
            {
                Trace out(_instance->logger(), _instance->traceCategory());
                out << "received " << protocol() << " connection close frame\n" << toString();
            }

            if(_state == StateClosingRequestPending)
            {
                //
                // If we receive a close frame while we were actually
                // waiting to send one, change the role and send a
                // close frame response.
                //
                if(!_closingInitiator)
                {
                    _closingInitiator = true;
                }
                _state = StateClosingResponsePending;
                return SocketOperationWrite;
            }
            else
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = 0;
                throw ex;
            }
        }
        case OP_PING: // Ping
        case OP_PONG: // Pong
        {
            throw ProtocolException(__FILE__, __LINE__, "ping or pong control frames not supported");
        }
        default:
        {
            ostringstream ostr;
            ostr << "unsupported opcode: " << _readOpCode;
            throw ProtocolException(__FILE__, __LINE__, ostr.str());
        }
        }
        _readState = ReadStatePayload;
    }

    _readStart = buf.i;

    if(_readI < _readBuffer.i)
    {
        size_t n = min(_readBuffer.i - _readI, buf.b.end() - buf.i);
        memcpy(buf.i, _readI, n);
        buf.i += n;
        _readI += n;
    }

    return buf.i < buf.b.end() ? SocketOperationRead : SocketOperationNone;
}

bool
IceWS::TransceiverI::postRead(Buffer& buf)
{
    if(_readState != ReadStatePayload)
    {
        return _readStart < _readBuffer.i; // Returns true if data was read.
    }

    if(_incoming)
    {
        //
        // Unmask the data we just read.
        //
        IceInternal::Buffer::Container::iterator p = _readStart;
        for(size_t n = _readStart - buf.b.begin(); p < buf.i; ++p, ++n)
        {
            *p ^= _readMask[n % 4];
        }
    }

    _readPayloadLength -= buf.i - _readStart;
    assert(_readPayloadLength >= 0);
    if(_readPayloadLength == 0)
    {
        //
        // We've read the complete payload, we're ready to read a new frame.
        //
        _readState = ReadStateOpcode;
    }

    return _readStart < buf.i; // Returns true if data was read.
}

void
IceWS::TransceiverI::preWrite(Buffer& buf)
{
    if(_state > StateHandshakeComplete)
    {
        if(_writeState == WriteStateHeader)
        {
            assert((_state == StateClosingRequestPending && !_closingInitiator) ||
                   (_state == StateClosingResponsePending && _closingInitiator));

            //
            // We need to prepare the frame header.
            //
            _writeBuffer.b.resize(_writeBufferSize);
            _writeBuffer.i = _writeBuffer.b.begin();

            //
            // Set the opcode - this is the one and only data frame.
            //
            *_writeBuffer.i++ = OP_CLOSE | FLAG_FINAL;
            *_writeBuffer.i++ = 0x02;

            if(!_incoming)
            {
                //
                // Add a random 32-bit mask to every outgoing frame, copy the payload data,
                // and apply the mask.
                //
                _writeBuffer.b[1] |= FLAG_MASKED;
                IceUtilInternal::generateRandom(reinterpret_cast<char*>(_writeMask), sizeof(_writeMask));
                memcpy(_writeBuffer.i, _writeMask, sizeof(_writeMask));
                _writeBuffer.i += sizeof(_writeMask);
            }

            *reinterpret_cast<uint16_t*>(_writeBuffer.i) = htons(static_cast<uint16_t>(_closingReason));
            *_writeBuffer.i++ ^= _writeMask[0];
            *_writeBuffer.i++ ^= _writeMask[1];

            _writeBuffer.b.resize(_writeBuffer.i - _writeBuffer.b.begin());
            _writeBuffer.i = _writeBuffer.b.begin();
            _writePayloadLength = 0;
            _writeState = WriteStateCloseFrame;
            return;
        }
        else if(_writeState == WriteStateCloseFrame)
        {
            assert(_writeBuffer.i != _writeBuffer.b.end());
            return;
        }
    }

    //
    // For an outgoing connection, each message must be masked with a random
    // 32-bit value, so we copy the entire message into the internal buffer
    // for writing.
    //
    // For an incoming connection, we use the internal buffer to hold the
    // frame header, and then write the caller's buffer to avoid copying.
    //
    if(_writeState == WriteStateHeader)
    {
        assert(buf.i == buf.b.begin());

        //
        // We need to prepare the frame header.
        //
        _writeBuffer.b.resize(_writeBufferSize);
        _writeBuffer.i = _writeBuffer.b.begin();

        //
        // Set the opcode - this is the one and only data frame.
        //
        *_writeBuffer.i++ = static_cast<Byte>(OP_DATA | FLAG_FINAL);

        //
        // Set the payload length.
        //
        const size_t payloadLength = buf.b.size();
        if(payloadLength <= 125)
        {
            *_writeBuffer.i++ = static_cast<Byte>(payloadLength);
        }
        else if(payloadLength > 125 && payloadLength <= USHRT_MAX)
        {
            //
            // Use an extra 16 bits to encode the payload length.
            //
            *_writeBuffer.i++ = static_cast<Byte>(126);
            *reinterpret_cast<uint16_t*>(_writeBuffer.i) = htons(static_cast<uint16_t>(payloadLength));
            _writeBuffer.i += 2;
        }
        else if(payloadLength > USHRT_MAX)
        {
            //
            // Use an extra 64 bits to encode the payload length.
            //
            *_writeBuffer.i++ = static_cast<Byte>(127);
            htonll(payloadLength, _writeBuffer.i);
            _writeBuffer.i += 8;
        }

        if(!_incoming)
        {
            //
            // Add a random 32-bit mask to every outgoing frame, copy the payload data,
            // and apply the mask.
            //
            _writeBuffer.b[1] |= FLAG_MASKED;
            IceUtilInternal::generateRandom(reinterpret_cast<char*>(_writeMask), sizeof(_writeMask));
            memcpy(_writeBuffer.i, _writeMask, sizeof(_writeMask));
            _writeBuffer.i += sizeof(_writeMask);
        }
        assert(_writeBuffer.i < _writeBuffer.b.end());
        _writeState = WriteStatePayload;
        _writePayloadLength = 0;

        //
        // For server connections, we use the _writeBuffer only to
        // write the header, the message is sent directly from the
        // message buffer. For client connections, we use the write
        // buffer for both the header and message buffer since we need
        // to mask the message data.
        //
        if(_incoming)
        {
            _writeBuffer.b.resize(_writeBuffer.i - _writeBuffer.b.begin());
            _writeBuffer.i = _writeBuffer.b.begin();
        }
    }

    if(!_incoming)
    {
        if((_writePayloadLength == 0 || _writeBuffer.i == _writeBuffer.b.end()))
        {
            if(_writeBuffer.i == _writeBuffer.b.end())
            {
                _writeBuffer.i = _writeBuffer.b.begin();
            }
            size_t n = buf.i - buf.b.begin();
            for(; n < buf.b.size() && _writeBuffer.i < _writeBuffer.b.end(); ++_writeBuffer.i, ++n)
            {
                *_writeBuffer.i = buf.b[n] ^ _writeMask[n % 4];
            }
            _writePayloadLength = n;

            if(_writeBuffer.i < _writeBuffer.b.end())
            {
                _writeBuffer.b.resize(_writeBuffer.i - _writeBuffer.b.begin());
            }
            _writeBuffer.i = _writeBuffer.b.begin();
        }
    }
}

SocketOperation
IceWS::TransceiverI::postWrite(Buffer& buf)
{
    if(_state > StateHandshakeComplete && _writeState == WriteStateCloseFrame)
    {
        if(_writeBuffer.i == _writeBuffer.b.end())
        {
            assert((_state == StateClosingRequestPending && !_closingInitiator) ||
                   (_state == StateClosingResponsePending && _closingInitiator));

            if(_instance->traceLevel() >= 2)
            {
                Trace out(_instance->logger(), _instance->traceCategory());
                out << "sent " << protocol() << " connection close frame\n" << toString();
            }

            if(_state == StateClosingRequestPending && !_closingInitiator)
            {
                _state = StateClosingResponsePending;
                return SocketOperationRead;
            }
            else
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = 0;
                throw ex;
            }
        }
        else
        {
            return SocketOperationWrite;
        }
    }

    if(!_incoming && _writePayloadLength > 0)
    {
        if(_writeBuffer.i == _writeBuffer.b.end())
        {
            buf.i = buf.b.begin() + _writePayloadLength;
        }
    }

    if(buf.i == buf.b.end())
    {
        _writeState = WriteStateHeader;
        if((_state == StateClosingRequestPending && !_closingInitiator) ||
           (_state == StateClosingResponsePending && _closingInitiator)) 
        {
            return SocketOperationWrite;
        }
        return SocketOperationNone;
    }
    return SocketOperationWrite;
}

bool
IceWS::TransceiverI::readBuffered(IceInternal::Buffer::Container::size_type sz)
{
    if(_readI == _readBuffer.i)
    {
        _readBuffer.b.resize(_readBufferSize);
        _readI = _readBuffer.i = _readBuffer.b.begin();
    }
    else
    {
        IceInternal::Buffer::Container::size_type available = _readBuffer.i - _readI;
        if(available < sz)
        {
            assert(available < _readBufferSize);
            memmove(&_readBuffer.b[0], _readI, available);
            _readBuffer.b.resize(_readBufferSize);
            _readI = _readBuffer.b.begin();
            _readBuffer.i = _readI + available;
        }
    }

    if(_readI + sz > _readBuffer.i)
    {
        return false; // Not enough read.
    }
    assert(_readBuffer.i > _readI);
    return true;
}
