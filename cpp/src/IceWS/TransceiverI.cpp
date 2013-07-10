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
    return _delegate->getAsyncInfo(status);
}
#endif

SocketOperation
IceWS::TransceiverI::initialize(Buffer& readBuffer, Buffer& writeBuffer)
{
    try
    {
        if(_state == StateInitializeDelegate)
        {
            SocketOperation op = _delegate->initialize(readBuffer, writeBuffer);
            if(op != SocketOperationNone)
            {
                return op;
            }
            _state = StateConnected;
        }

        if(_state == StateConnected)
        {
            //
            // We don't know how much we'll need to read.
            //
            readBuffer.b.resize(1024);
            readBuffer.i = readBuffer.b.begin();

            if(_incoming)
            {
                _state = StateRequestPending;

#ifdef ICE_USE_IOCP
                return SocketOperationRead;
#else
                if(!read(readBuffer))
                {
                    return SocketOperationRead;
                }
#endif
            }
            else
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
                IceUtilInternal::generateRandom(reinterpret_cast<char*>(&key[0]), key.size());
                _key = IceInternal::Base64::encode(key);
                out << _key << "\r\n\r\n"; // EOM

                string str = out.str();
                writeBuffer.b.resize(str.size());
                memcpy(&writeBuffer.b[0], str.c_str(), str.size());
                writeBuffer.i = writeBuffer.b.begin();

#ifdef ICE_USE_IOCP
                _state = StateRequestPending;
                return SocketOperationWrite;
#else
                if(write(writeBuffer))
                {
                    //
                    // Try reading the response.
                    //
                    _state = StateResponsePending;
                    if(!read(readBuffer))
                    {
                        return SocketOperationRead;
                    }
                }
                else
                {
                    _state = StateRequestPending;
                    return SocketOperationWrite;
                }
#endif
            }
        }

        if(_state == StateRequestPending)
        {
            if(_incoming)
            {
                assert(readBuffer.i == readBuffer.b.end());

                //
                // Parse the client's upgrade request.
                //
                try
                {
                    if(_parser->parse(&readBuffer.b[0], readBuffer.b.end()))
                    {
                        //
                        // Check for extra data after the request.
                        //
                        if(_readBuffer.b.size() > 0)
                        {
                            throw ProtocolException(__FILE__, __LINE__, "unexpected data in request message");
                        }

                        handleRequest(writeBuffer);
                        _state = StateResponsePending;
#ifdef ICE_USE_IOCP
                        return SocketOperationWrite;
#else
                        if(!write(writeBuffer))
                        {
                            return SocketOperationWrite;
                        }
#endif
                    }
                    else
                    {
                        throw ProtocolException(__FILE__, __LINE__, "incomplete request message");
                    }
                }
                catch(const WebSocketException& ex)
                {
                    throw ProtocolException(__FILE__, __LINE__, ex.reason);
                }
            }
            else
            {
                assert(writeBuffer.i == writeBuffer.b.end());

                //
                // The write has completed, now we check for a response.
                //
                _state = StateResponsePending;

                if(!read(readBuffer))
                {
                    return SocketOperationRead;
                }
            }
        }

        if(_state == StateResponsePending)
        {
            if(_incoming)
            {
                assert(writeBuffer.i == writeBuffer.b.end());
                _state = StateHandshakeComplete;
            }
            else
            {
                assert(readBuffer.i == readBuffer.b.end());

                //
                // Parse the server's response.
                //
                try
                {
                    if(_parser->parse(&readBuffer.b[0], readBuffer.b.end()))
                    {
                        handleResponse();
                        _state = StateHandshakeComplete;
                    }
                    else
                    {
                        throw ProtocolException(__FILE__, __LINE__, "incomplete response message");
                    }
                }
                catch(const WebSocketException& ex)
                {
                    throw ProtocolException(__FILE__, __LINE__, ex.reason);
                }
            }
        }

        assert(_state == StateHandshakeComplete);

        readBuffer.b.reset();
        writeBuffer.b.reset();

        _writeState = WriteStateStartFrame;

        //
        // Check if we've already read some extra data from the socket.
        //
        if(_readBuffer.b.size() >= 2)
        {
            _readState = ReadStateFrameOpcode;
        }
        else if(_readBuffer.b.size() == 1)
        {
            _readBuffer.b.resize(2); // Need the first two bytes of a frame.
            _readState = ReadStateFrameOpcode;
        }
        else
        {
            _readState = ReadStateStartFrame;
        }
    }
    catch(const Ice::LocalException& ex)
    {
        if(_instance->networkTraceLevel() >= 2)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "failed to establish ws connection\n";
            out << toString() << "\n" << ex;
        }
        throw;
    }

    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        if(_incoming)
        {
            out << "accepted ws connection\n" << toString();
        }
        else
        {
            out << "ws connection established\n" << toString();
        }
    }

    return SocketOperationNone;
}

void
IceWS::TransceiverI::close()
{
    // TODO: Send close frame

    if(_state == StateHandshakeComplete && _instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "closing ws connection\n" << toString();
    }

    _delegate->close();
}

bool
IceWS::TransceiverI::write(Buffer& buf)
{
    Buffer& wb = preWrite(buf);

    _delegate->write(wb);

    postWrite(buf);

    return buf.i == buf.b.end();
}

bool
IceWS::TransceiverI::read(Buffer& buf)
{
    Buffer& rb = preRead(buf);

    _delegate->read(rb);

    postRead(buf);

    return buf.i == buf.b.end();
}

#ifdef ICE_USE_IOCP
bool
IceWS::TransceiverI::startWrite(Buffer& buf)
{
    // TODO
    return _delegate->startWrite(buf);
}

void
IceWS::TransceiverI::finishWrite(Buffer& buf)
{
    // TODO
    _delegate->finishWrite(buf);
}

void
IceWS::TransceiverI::startRead(Buffer& buf)
{
    _delegate->startRead(buf);
}

void
IceWS::TransceiverI::finishRead(Buffer& buf)
{
    _delegate->finishRead(buf);
}
#endif

string
IceWS::TransceiverI::type() const
{
    return _type == WSEndpointType ? "ws" : "wss"; // TODO
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
    info->resource = _resource;
    return info;
}

void
IceWS::TransceiverI::checkSendSize(const Buffer& buf, size_t messageSizeMax)
{
    _delegate->checkSendSize(buf, messageSizeMax);
}

IceWS::TransceiverI::TransceiverI(const InstancePtr& instance, Short type, const TransceiverPtr& del,
                                  const string& host, int port, const string& resource) :
    _instance(instance),
    _type(type),
    _delegate(del),
    _host(host),
    _port(port),
    _resource(resource),
    _logger(instance->communicator()->getLogger()),
    _incoming(false),
    _state(StateInitializeDelegate),
    _parser(new HttpParser),
    _readState(ReadStateInitializing),
    _readBuffer(0),
    _lastFrame(false),
    _opCode(0),
    _readHeaderLength(0),
    _payloadLength(0),
    _writeState(WriteStateInitializing),
    _writeBuffer(0),
    _writeHeaderLength(0)
{
}

IceWS::TransceiverI::TransceiverI(const InstancePtr& instance, Short type, const TransceiverPtr& del) :
    _instance(instance),
    _type(type),
    _delegate(del),
    _port(-1),
    _logger(instance->communicator()->getLogger()),
    _incoming(true),
    _state(StateInitializeDelegate),
    _parser(new HttpParser),
    _readState(ReadStateInitializing),
    _readBuffer(0),
    _lastFrame(false),
    _opCode(0),
    _readHeaderLength(0),
    _payloadLength(0),
    _writeState(WriteStateInitializing),
    _writeBuffer(0),
    _writeHeaderLength(0)
{
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
    out << IceInternal::Base64::encode(hash) << "\r\n"
        << "\r\n"; // EOM

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

Buffer&
IceWS::TransceiverI::preRead(Buffer& buf)
{
    if(_state < StateHandshakeComplete)
    {
        return buf;
    }

    if(_readState == ReadStateStartFrame)
    {
        _readBuffer.b.resize(2); // Need the first two bytes of a frame.
        _readBuffer.i = _readBuffer.b.begin();
        _readState = ReadStateFrameOpcode;
    }

    return _readBuffer;
}

void
IceWS::TransceiverI::postRead(Buffer& buf)
{
    //
    // We use the caller's buffer when _state < StateHandshakeComplete, otherwise we use
    // our internal buffer (_readBuffer).
    //

    //
    // When awaiting the initial HTTP request or response, we don't know exactly how many bytes
    // we need to read.
    //
    if((_state == StateRequestPending && _incoming) || (_state == StateResponsePending && !_incoming))
    {
        //
        // Check if we have enough data for a complete message.
        //
        const Ice::Byte* p = _parser->isCompleteMessage(&buf.b[0], buf.i);
        if(p)
        {
            //
            // We may have read additional data (such as the server's validate-connection
            // message), so we preserve this in our internal buffer for the next call to read().
            //
            const int remaining = static_cast<int>(buf.i - p);
            if(remaining > 0)
            {
                _readBuffer.b.resize(remaining);
                memcpy(&_readBuffer.b[0], p, remaining);
                _readBuffer.i = _readBuffer.b.end();
            }

            //
            // Shrink the buffer to match the current size so that we stop reading.
            //
            buf.b.resize(p - buf.b.begin());
            buf.i = buf.b.end();
        }
        else
        {
            //
            // Enlarge the buffer and try to read more.
            //
            const size_t oldSize = static_cast<size_t>(buf.i - buf.b.begin());
            if(oldSize + 1024 > _instance->messageSizeMax())
            {
                throw MemoryLimitException(__FILE__, __LINE__);
            }
            buf.b.resize(oldSize + 1024);
            buf.i = buf.b.begin() + oldSize;
        }
    }

    if(_state < StateHandshakeComplete || _readBuffer.i == _readBuffer.b.begin())
    {
        return;
    }

    const size_t bytesRead = static_cast<size_t>(_readBuffer.i - _readBuffer.b.begin());
    assert(bytesRead > 0);

    if(_readState == ReadStateFrameOpcode && bytesRead >= 2)
    {
        unsigned char ch;

        ch = static_cast<unsigned char>(_readBuffer.b[0]);

        //
        // Most-significant bit indicates whether this is the last frame.
        //
        _lastFrame = (ch & FLAG_FINAL) == FLAG_FINAL;

        //
        // Least-significant four bits hold the opcode.
        //
        _opCode = ch & 0xf;

        ch = static_cast<unsigned char>(_readBuffer.b[1]);

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
        _payloadLength = (ch & 0x7f);
        _readHeaderLength = 2;
        if(_payloadLength < 126)
        {
            if(_incoming)
            {
                _readHeaderLength = 6; // Need to read a 32-bit mask.
            }
        }
        else if(_payloadLength == 126)
        {
            if(_incoming)
            {
                _readHeaderLength = 8; // Need to read a 16-bit payload length and a 32-bit mask.
            }
            else
            {
                _readHeaderLength = 4; // Need to read a 16-bit payload length.
            }
        }
        else
        {
            assert(_payloadLength == 127);
            if(_incoming)
            {
                _readHeaderLength = 14; // Need to read a 64-bit payload length and a 32-bit mask.
            }
            else
            {
                _readHeaderLength = 10; // Need to read a 64-bit payload length.
            }
        }

        //
        // If we haven't already read enough data for the header, resize the buffer
        // for the next read.
        //
        if(_readBuffer.b.size() < _readHeaderLength)
        {
            _readBuffer.b.resize(_readHeaderLength);
            _readBuffer.i = _readBuffer.b.begin() + bytesRead;
        }

        _readState = ReadStateFrameHeader;
    }

    if(_readState == ReadStateFrameHeader && bytesRead >= _readHeaderLength)
    {
        //
        // We've read a complete header.
        //

        if(_payloadLength < 126)
        {
            if(_incoming)
            {
                assert(bytesRead >= 6); // We must have needed to read the mask.
                memcpy(_mask, &_readBuffer.b[2], 4); // Copy the mask.
            }
        }
        else if(_payloadLength == 126)
        {
            _payloadLength = static_cast<size_t>(ntohs(*reinterpret_cast<uint16_t*>(&_readBuffer.b[2])));
            assert(bytesRead >= _incoming ? 8 : 4);
            if(_incoming)
            {
                memcpy(_mask, &_readBuffer.b[4], 4); // Copy the mask.
            }
        }
        else
        {
            assert(_payloadLength == 127);
            Long l = nlltoh(&_readBuffer.b[2]);
            if(l < 0 || l > INT_MAX)
            {
                ostringstream ostr;
                ostr << "invalid WebSocket payload length: " << l;
                throw ProtocolException(__FILE__, __LINE__, ostr.str());
            }
            _payloadLength = static_cast<size_t>(l);
            assert(bytesRead >= _incoming ? 14 : 10);
            if(_incoming)
            {
                memcpy(_mask, &_readBuffer.b[10], 4); // Copy the mask.
            }
        }

        _readState = ReadStateHandleOpcode;
    }

    if(_readState == ReadStateHandleOpcode)
    {
        switch(_opCode)
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
            if(!_lastFrame)
            {
                throw ProtocolException(__FILE__, __LINE__, "continuation frames not supported");
            }

            break;
        }
        case OP_CLOSE: // Connection close
        {
            if(_state == StateClosing)
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = 0;
                throw ex;
            }
            else
            {
                _state = StateClosing;
                // TODO: Need to send a matching close frame
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = 0;
                throw ex;
            }
            break;
        }
        case OP_PING: // Ping
        {
            // TODO: Need to read the payload (if any) send a matching pong frame
            break;
        }
        case OP_PONG: // Pong
        {
            // TODO: Need to read the payload (if any); no response needed
            break;
        }
        case OP_RES_0x3: // Reserved
        case OP_RES_0x4:
        case OP_RES_0x5:
        case OP_RES_0x6:
        case OP_RES_0x7:
        case OP_RES_0xB:
        case OP_RES_0xC:
        case OP_RES_0xD:
        case OP_RES_0xE:
        case OP_RES_0xF:
        {
            ostringstream ostr;
            ostr << "unsupported opcode: " << _opCode;
            throw ProtocolException(__FILE__, __LINE__, ostr.str());
        }
        default:
        {
            ostringstream ostr;
            ostr << "invalid opcode: " << _opCode;
            throw ProtocolException(__FILE__, __LINE__, ostr.str());
        }
        }

        if(_payloadLength > 0)
        {
            //
            // Have we already read some or all of the payload?
            //
            const size_t extra = _readBuffer.b.size() - _readHeaderLength;
            if(extra > 0)
            {
                // TODO: What if we've read MORE than the current payload length?
                assert(extra <= _payloadLength);
                //
                // Shift the remaining data to the beginning of the buffer.
                //
                memcpy(&_readBuffer.b[0], &_readBuffer.b[_readHeaderLength], extra);
                _readBuffer.b.resize(_payloadLength);
                _readI = _readBuffer.b.begin();
                _readBuffer.i = _readBuffer.b.begin() + extra;
            }
            else
            {
                _readBuffer.b.resize(_payloadLength);
                _readBuffer.i = _readI = _readBuffer.b.begin();
            }
            _readState = ReadStateFramePayload;
        }
        else
        {
            _readState = ReadStateStartFrame;
        }
    }

    if(_readState == ReadStateFramePayload)
    {
        if(_readI < _readBuffer.i)
        {
            switch(_opCode)
            {
            case OP_CONT: // Continuation frame
            {
                assert(false);
                // TODO: Add support for continuation frames?
            }
            case OP_TEXT: // Text frame
            {
                assert(false);
            }
            case OP_DATA: // Data frame
            {
                const size_t bytesRequested = static_cast<size_t>(buf.b.end() - buf.i);
                const size_t bytesAvailable = static_cast<size_t>(_readBuffer.i - _readI);
                const size_t bytesCopied = bytesRequested < bytesAvailable ? bytesRequested : bytesAvailable;
                memcpy(buf.i, _readI, bytesCopied);
                if(_incoming)
                {
                    //
                    // Unmask the data.
                    //
                    Buffer::Container::iterator i = buf.i;
                    Buffer::Container::const_iterator end = i + bytesCopied;
                    for(int n = i - buf.b.begin(); i < end; ++n, ++i)
                    {
                        *i ^= _mask[n % 4];
                    }
                }
                buf.i += bytesCopied;
                _readI += bytesCopied;
                break;
            }
            case OP_CLOSE: // Connection close
            {
                assert(false);
                // TODO: Need to send a matching close frame
                break;
            }
            case OP_PING: // Ping
            {
                assert(false);
                // TODO: Need to send a matching pong frame
                break;
            }
            case OP_PONG: // Pong
            {
                //
                // Nothing to do.
                //
                break;
            }
            default:
            {
                assert(false);
            }
            }
        }

        //
        // TODO: If we limit the size of _readBuffer to the size requested in buf (instead
        // of using _payloadLength), we need to modify this test.
        //
        //if(_readI == _readBuffer.b.end() && _readBuffer.b.size() == _payloadLength)
        //
        if(_readI == _readBuffer.b.end())
        {
            _readState = ReadStateStartFrame;
        }
    }
}

Buffer&
IceWS::TransceiverI::preWrite(Buffer& buf)
{
    //
    // This method determines which buffer to write to the socket.
    //
    // For an outgoing connection, each message must be masked with a random
    // 32-bit value, so we copy the entire message into an internal buffer
    // for writing.
    //
    // For an incoming connection, we use the internal buffer to hold the
    // frame header, and then write the caller's buffer to avoid copying.
    //

    if(_writeState == WriteStateStartFrame)
    {
        assert(buf.i == buf.b.begin());

        //
        // We need to prepare the frame header.
        //
        const size_t payloadLength = buf.b.size();
        _writeBuffer.b.resize(2);

        //
        // Set the opcode - this is the one and only data frame.
        //
        _writeBuffer.b[0] = static_cast<Byte>(OP_DATA | FLAG_FINAL);

        //
        // Set the payload length.
        //
        if(payloadLength <= 125)
        {
            _writeBuffer.b[1] = static_cast<Byte>(payloadLength);
        }
        else if(payloadLength > 125 && payloadLength <= USHRT_MAX)
        {
            //
            // Use an extra 16 bits to encode the payload length.
            //
            _writeBuffer.b.resize(4);
            _writeBuffer.b[1] = static_cast<Byte>(126);
            *reinterpret_cast<uint16_t*>(&_writeBuffer.b[2]) = htons(static_cast<uint16_t>(payloadLength));
        }
        else if(payloadLength > USHRT_MAX)
        {
            //
            // Use an extra 64 bits to encode the payload length.
            //
            _writeBuffer.b.resize(10);
            _writeBuffer.b[1] = static_cast<Byte>(127);
            htonll(payloadLength, &_writeBuffer.b[2]);
        }

        _writeHeaderLength = _writeBuffer.b.size();

        if(!_incoming)
        {
            //
            // Add a random 32-bit mask to every outgoing frame, copy the payload data,
            // and apply the mask.
            //
            _writeBuffer.b[1] |= FLAG_MASKED;
            vector<unsigned char> mask(4);
            IceUtilInternal::generateRandom(reinterpret_cast<char*>(&mask[0]), mask.size());
            _writeBuffer.b.resize(_writeHeaderLength + 4 + payloadLength);
            memcpy(&_writeBuffer.b[_writeHeaderLength], &mask[0], 4);
            _writeHeaderLength += 4;

            Buffer::Container::iterator i = _writeBuffer.b.begin() + _writeHeaderLength;
            memcpy(i, buf.i, payloadLength);
            for(int n = 0; i < _writeBuffer.b.end(); ++i, ++n)
            {
                *i ^= mask[n % 4];
            }

            _writeState = WriteStatePayload;
        }
        else
        {
            _writeState = WriteStateHeader;
        }

        _writeBuffer.i = _writeBuffer.b.begin();
        return _writeBuffer;
    }
    else if(_writeState == WriteStateHeader)
    {
        assert(_incoming);

        if(_writeBuffer.i < _writeBuffer.b.end())
        {
            return _writeBuffer;
        }

        _writeState = WriteStatePayload;
    }
    else if(_writeState == WriteStatePayload && !_incoming)
    {
        return _writeBuffer;
    }

    return buf;
}

void
IceWS::TransceiverI::postWrite(Buffer& buf)
{
    assert(_writeState != WriteStateStartFrame);

    if(_writeState == WriteStatePayload)
    {
        if(!_incoming)
        {
            //
            // Advance the position of the caller's buffer to match the amount of data (excluding
            // the header) written from our internal buffer.
            //
            const size_t bytesWritten = static_cast<size_t>(_writeBuffer.i - _writeBuffer.b.begin());
            buf.i = buf.b.begin() + bytesWritten - _writeHeaderLength;
        }

        if(buf.i == buf.b.end())
        {
            _writeState = WriteStateStartFrame;
        }
    }
}
