// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceSSL/TransceiverI.h>
#include <IceSSL/Instance.h>
#include <IceSSL/Util.h>
#include <Ice/Communicator.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Stats.h>
#include <Ice/Buffer.h>
#include <Ice/Network.h>
#include <Ice/LocalException.h>

#include <openssl/err.h>
#include <openssl/bio.h>

using namespace std;
using namespace Ice;
using namespace IceSSL;

IceInternal::NativeInfoPtr
IceSSL::TransceiverI::getNativeInfo()
{
    return this;
}

#if defined(ICE_USE_IOCP)
IceInternal::AsyncInfo*
IceSSL::TransceiverI::getAsyncInfo(IceInternal::SocketOperation status)
{
    switch(status)
    {
    case IceInternal::SocketOperationRead:
        return &_read;
    case IceInternal::SocketOperationWrite:
        return &_write;
    default:
        assert(false);
        return 0;
    }
}
#endif

void
IceSSL::TransceiverI::close()
{
    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        out << "closing ssl connection\n" << toString();
    }

    if(_ssl)
    {
        int err = SSL_shutdown(_ssl);
    
        //
        // Call it one more time if it returned 0.
        //
        if(err == 0)
        {
            SSL_shutdown(_ssl);
        }

        SSL_free(_ssl);
        _ssl = 0;
    }

    assert(_fd != INVALID_SOCKET);
    try
    {
        IceInternal::closeSocket(_fd);
        _fd = INVALID_SOCKET;
    }
    catch(const SocketException&)
    {
        _fd = INVALID_SOCKET;
        throw;
    }
}

bool
IceSSL::TransceiverI::write(IceInternal::Buffer& buf)
{
#ifdef ICE_USE_IOCP
    if(_writeI != _writeBuffer.end())
    {
        if(!send())
        {
            return false;
        }
    }
#endif

    // Its impossible for the packetSize to be more than an Int.
    int packetSize = static_cast<int>(buf.b.end() - buf.i);
    while(buf.i != buf.b.end())
    {
        ERR_clear_error(); // Clear any spurious errors.
        assert(_fd != INVALID_SOCKET);
#ifndef ICE_USE_IOCP
        int ret = SSL_write(_ssl, reinterpret_cast<const void*>(&*buf.i), packetSize);
#else
        int ret;
        if(_sentBytes)
        {
            ret = _sentBytes;
            _sentBytes = 0;
        }
        else
        {
            ret = SSL_write(_ssl, reinterpret_cast<const void*>(&*buf.i), packetSize);
            if(ret > 0)
            {
                if(!send())
                {
                    _sentBytes = ret;
                    return false;
                }
            }
        }
#endif
        if(ret <= 0)
        {
            switch(SSL_get_error(_ssl, ret))
            {
            case SSL_ERROR_NONE:
                assert(false);
                break;
            case SSL_ERROR_ZERO_RETURN:
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            case SSL_ERROR_WANT_READ:
            {
                assert(false);
                break;
            }
            case SSL_ERROR_WANT_WRITE:
            {
#ifdef ICE_USE_IOCP
                if(send())
                {
                    continue;
                }
#endif
                return false;
            }
            case SSL_ERROR_SYSCALL:
            {
#ifndef ICE_USE_IOCP
                if(ret == -1)
                {
                    if(IceInternal::interrupted())
                    {
                        continue;
                    }

                    if(IceInternal::noBuffers() && packetSize > 1024)
                    {
                        packetSize /= 2;
                        continue;
                    }

                    if(IceInternal::wouldBlock())
                    {
                        assert(SSL_want_write(_ssl));
                        return false;
                    }

                    if(IceInternal::connectionLost())
                    {
                        ConnectionLostException ex(__FILE__, __LINE__);
                        ex.error = IceInternal::getSocketErrno();
                        throw ex;
                    }
                }
#endif
                if(ret == 0)
                {
                    ConnectionLostException ex(__FILE__, __LINE__);
                    ex.error = 0;
                    throw ex;
                }

                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            case SSL_ERROR_SSL:
            {
                ProtocolException ex(__FILE__, __LINE__);
                ex.reason = "SSL protocol error during write:\n" + _instance->sslErrors();
                throw ex;
            }
            }
        }

        if(_instance->networkTraceLevel() >= 3)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "sent " << ret << " of " << packetSize << " bytes via ssl\n" << toString();
        }

        if(_stats)
        {
            _stats->bytesSent(type(), static_cast<Int>(ret));
        }

        buf.i += ret;

        if(packetSize > buf.b.end() - buf.i)
        {
            packetSize = static_cast<int>(buf.b.end() - buf.i);
        }
    }

    return true;
}

bool
IceSSL::TransceiverI::read(IceInternal::Buffer& buf)
{
#ifdef ICE_USE_IOCP
    if(_readI != _readBuffer.end())
    {
        if(!receive())
        {
            return false;
        }
    }
#endif

    // It's impossible for the packetSize to be more than an Int.
    int packetSize = static_cast<int>(buf.b.end() - buf.i);    
    while(buf.i != buf.b.end())
    {
        ERR_clear_error(); // Clear any spurious errors.
        assert(_fd != INVALID_SOCKET);
        int ret = SSL_read(_ssl, reinterpret_cast<void*>(&*buf.i), packetSize);
        if(ret <= 0)
        {
            switch(SSL_get_error(_ssl, ret))
            {
            case SSL_ERROR_NONE:
                assert(false);
                break;
            case SSL_ERROR_ZERO_RETURN:
            {
                //
                // If the connection is lost when reading data, we shut
                // down the write end of the socket. This helps to unblock
                // threads that are stuck in send() or select() while
                // sending data. Note: I don't really understand why
                // send() or select() sometimes don't detect a connection
                // loss. Therefore this helper to make them detect it.
                //
                //assert(_fd != INVALID_SOCKET);
                //shutdownSocketReadWrite(_fd);

                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = 0;
                throw ex;
            }
            case SSL_ERROR_WANT_READ:
            {
#ifdef ICE_USE_IOCP
                if(receive())
                {
                    continue;
                }
#endif
                return false;
            }
            case SSL_ERROR_WANT_WRITE:
            {
                assert(false);
                break;
            }
            case SSL_ERROR_SYSCALL:
            {
#ifndef ICE_USE_IOCP
                if(ret == -1)
                {
                    if(IceInternal::interrupted())
                    {
                        continue;
                    }

                    if(IceInternal::noBuffers() && packetSize > 1024)
                    {
                        packetSize /= 2;
                        continue;
                    }

                    if(IceInternal::wouldBlock())
                    {
                        assert(SSL_want_read(_ssl));
                        return false;
                    }

                    if(IceInternal::connectionLost())
                    {
                        //
                        // See the commment above about shutting down the
                        // socket if the connection is lost while reading
                        // data.
                        //
                        //assert(_fd != INVALID_SOCKET);
                        //shutdownSocketReadWrite(_fd);

                        ConnectionLostException ex(__FILE__, __LINE__);
                        ex.error = IceInternal::getSocketErrno();
                        throw ex;
                    }
                }
#endif

                if(ret == 0)
                {
                    ConnectionLostException ex(__FILE__, __LINE__);
                    ex.error = 0;
                    throw ex;
                }

                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            case SSL_ERROR_SSL:
            {
                //
                // Forcefully closing a connection can result in SSL_read reporting
                // "decryption failed or bad record mac". We trap that error and
                // treat it as the loss of a connection.
                //
                // NOTE: We have to compare the reason string instead
                // of the error codes because the error code values
                // changed between OpenSSL 0.9.7i and 0.9.7j and
                // between OpenSSL 0.9.8a and 0.9.8b...
                //
                //unsigned long e = ERR_peek_error();
                //if(ERR_GET_LIB(e) == ERR_LIB_SSL && ERR_GET_REASON(e) == SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC)
                //
                unsigned long e = ERR_peek_error();
                const char* estr = ERR_GET_LIB(e) == ERR_LIB_SSL ? ERR_reason_error_string(e) : 0;
                if(estr && strcmp(estr, "decryption failed or bad record mac") == 0)
                {
                    ConnectionLostException ex(__FILE__, __LINE__);
                    ex.error = 0;
                    throw ex;
                }
                else
                {
                    ProtocolException ex(__FILE__, __LINE__);
                    ex.reason = "SSL protocol error during read:\n" + _instance->sslErrors();
                    throw ex;
                }
            }
            }
        }

        if(_instance->networkTraceLevel() >= 3)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "received " << ret << " of " << packetSize << " bytes via ssl\n" << toString();
        }

        if(_stats)
        {
            _stats->bytesReceived(type(), static_cast<Int>(ret));
        }

        buf.i += ret;

        if(packetSize > buf.b.end() - buf.i)
        {
            packetSize = static_cast<int>(buf.b.end() - buf.i);
        }
    }

    return true;
}

#ifdef ICE_USE_IOCP
void
IceSSL::TransceiverI::startWrite(IceInternal::Buffer& buf)
{
    if(_state < StateConnected)
    {
        doConnectAsync(_fd, _connectAddr, _write);
        _desc = IceInternal::fdToString(_fd);
        return;
    }

    assert(!_writeBuffer.empty() && _writeI != _writeBuffer.end());

    int packetSize = static_cast<int>(_writeBuffer.end() - _writeI);
    if(_maxSendPacketSize > 0 && packetSize > _maxSendPacketSize)
    { 
        packetSize = _maxSendPacketSize;
    }

    _write.buf.len = packetSize;
    _write.buf.buf = reinterpret_cast<char*>(&*_writeI);
    int err = WSASend(_fd, &_write.buf, 1, &_write.count, 0, &_write, NULL);
    if(err == SOCKET_ERROR)
    {
        if(!IceInternal::wouldBlock())
        {
            if(IceInternal::connectionLost())
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            else
            {
                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
        }
    }
}

void
IceSSL::TransceiverI::finishWrite(IceInternal::Buffer& buf)
{
    if(_state < StateConnected)
    {
        doFinishConnectAsync(_fd, _write);
        return;
    }

    if(_write.count == SOCKET_ERROR)
    {
        WSASetLastError(_write.error);
        if(IceInternal::connectionLost())
        {
            ConnectionLostException ex(__FILE__, __LINE__);
            ex.error = IceInternal::getSocketErrno();
            throw ex;
        }
        else
        {
            SocketException ex(__FILE__, __LINE__);
            ex.error = IceInternal::getSocketErrno();
            throw ex;
        }
    }

    _writeI += _write.count;
}

void
IceSSL::TransceiverI::startRead(IceInternal::Buffer& buf)
{
    if(_readI == _readBuffer.end())
    {
        assert(!buf.b.empty() && buf.i != buf.b.end());
        assert(!BIO_ctrl_get_read_request(_iocpBio));

        int ret = SSL_read(_ssl, reinterpret_cast<void*>(&*buf.i), static_cast<int>(buf.b.end() - buf.i));
        assert(ret <= 0 && SSL_get_error(_ssl, ret) == SSL_ERROR_WANT_READ);

        assert(BIO_ctrl_get_read_request(_iocpBio));
        _readBuffer.resize(BIO_ctrl_get_read_request(_iocpBio));
        _readI = _readBuffer.begin();
    }

    assert(!_readBuffer.empty() && _readI != _readBuffer.end());

    int packetSize = static_cast<int>(_readBuffer.end() - _readI);
    if(_maxReceivePacketSize > 0 && packetSize > _maxReceivePacketSize)
    {
        packetSize = _maxReceivePacketSize;
    }

    _read.buf.len = packetSize;
    _read.buf.buf = reinterpret_cast<char*>(&*_readI);
    int err = WSARecv(_fd, &_read.buf, 1, &_read.count, &_read.flags, &_read, NULL);
    if(err == SOCKET_ERROR)
    {
        if(!IceInternal::wouldBlock())
        {
            if(IceInternal::connectionLost())
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            else
            {
                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
        }
    }
}

void 
IceSSL::TransceiverI::finishRead(IceInternal::Buffer& buf)
{
    if(_read.count == SOCKET_ERROR)
    {
        WSASetLastError(_read.error);
        if(IceInternal::connectionLost())
        {
            ConnectionLostException ex(__FILE__, __LINE__);
            ex.error = IceInternal::getSocketErrno();
            throw ex;
        }
        else
        {
            SocketException ex(__FILE__, __LINE__);
            ex.error = IceInternal::getSocketErrno();
            throw ex;
        }
    }

    _readI += _read.count;

    if(_readI == _readBuffer.end())
    {
        assert(_readI == _readBuffer.end());
        int n = BIO_write(_iocpBio, &_readBuffer[0], static_cast<int>(_readBuffer.size()));
        assert(n == static_cast<int>(_readBuffer.size()));
    }
}
#endif

string
IceSSL::TransceiverI::type() const
{
    return "ssl";
}

string
IceSSL::TransceiverI::toString() const
{
    return _desc;
}

IceInternal::SocketOperation
IceSSL::TransceiverI::initialize()
{
    try
    {
        if(_state == StateNeedConnect)
        {
            _state = StateConnectPending;
            return IceInternal::SocketOperationConnect; 
        }
        else if(_state <= StateConnectPending)
        {
#ifdef ICE_USE_CFSTREAM
            errno = _connectError;
#endif            
#ifndef ICE_USE_IOCP
            IceInternal::doFinishConnect(_fd);
#endif
            _state = StateConnected;
            _desc = IceInternal::fdToString(_fd);
        }
        assert(_state == StateConnected);

        if(!_ssl)
        {
#ifdef ICE_USE_IOCP
            //
            // On Windows, limiting the buffer size is important to prevent
            // poor throughput performances when transfering large amount of
            // data. See Microsoft KB article KB823764.
            //
            _maxSendPacketSize = IceInternal::getSendBufferSize(_fd) / 2;
            if(_maxSendPacketSize < 512)
            {
                _maxSendPacketSize = 0;
            }

            _maxReceivePacketSize = IceInternal::getRecvBufferSize(_fd);
            if(_maxReceivePacketSize < 512)
            {
                _maxReceivePacketSize = 0;
            }

            _readI = _readBuffer.end();
            _writeI = _writeBuffer.end();
            _sentBytes = 0;
#endif

#ifndef ICE_USE_IOCP
            // This static_cast is necessary due to 64bit windows. There SOCKET is a non-int type.
            BIO* bio = BIO_new_socket(static_cast<int>(_fd), 0);
#else
            BIO* bio;
            if(!BIO_new_bio_pair(&bio, _maxSendPacketSize, &_iocpBio, _maxReceivePacketSize))
            {
                bio = 0;
            }
#endif        
            if(!bio)
            {
                SecurityException ex(__FILE__, __LINE__);
                ex.reason = "openssl failure";
                throw ex;
            }

            _ssl = SSL_new(_instance->context());
            if(!_ssl)
            {
                BIO_free(bio);
                SecurityException ex(__FILE__, __LINE__);
                ex.reason = "openssl failure";
                throw ex;
            }
            SSL_set_bio(_ssl, bio, bio);
        }

        while(!SSL_is_init_finished(_ssl))
        {
            //
            // Only one thread calls initialize(), so synchronization is not necessary here.
            //
            int ret = _incoming ? SSL_accept(_ssl) : SSL_connect(_ssl);
#ifdef ICE_USE_IOCP
            if(BIO_ctrl_pending(_iocpBio))
            {
                if(!send())
                {
                    return IceInternal::SocketOperationWrite;
                }
                continue;
            }
#endif
            if(ret <= 0)
            {
                switch(SSL_get_error(_ssl, ret))
                {
                case SSL_ERROR_NONE:
                    assert(SSL_is_init_finished(_ssl));
                    break;
                case SSL_ERROR_ZERO_RETURN:
                {
                    ConnectionLostException ex(__FILE__, __LINE__);
                    ex.error = IceInternal::getSocketErrno();
                    throw ex;
                }
                case SSL_ERROR_WANT_READ:
                {
#ifdef ICE_USE_IOCP
                    if(receive())
                    {
                        continue;
                    }
#endif
                    return IceInternal::SocketOperationRead;
                }
                case SSL_ERROR_WANT_WRITE:
                {
#ifdef ICE_USE_IOCP
                    if(send())
                    {
                        continue;
                    }
#endif
                    return IceInternal::SocketOperationWrite;
                }
                case SSL_ERROR_SYSCALL:
                {            
                    if(ret == 0)
                    {
                        ConnectionLostException ex(__FILE__, __LINE__);
                        ex.error = 0;
                        throw ex;
                    }

#ifndef ICE_USE_IOCP                
                    if(ret == -1)
                    {
                        if(IceInternal::interrupted())
                        {
                            break;
                        }
                
                        if(IceInternal::wouldBlock())
                        {
                            if(SSL_want_read(_ssl))
                            {
                                return IceInternal::SocketOperationRead;
                            }
                            else if(SSL_want_write(_ssl))
                            {
                                return IceInternal::SocketOperationWrite;
                            }
                    
                            break;
                        }
                
                        if(IceInternal::connectionLost())
                        {
                            ConnectionLostException ex(__FILE__, __LINE__);
                            ex.error = IceInternal::getSocketErrno();
                            throw ex;
                        }
                    }
#endif
                    SocketException ex(__FILE__, __LINE__);
                    ex.error = IceInternal::getSocketErrno();
                    throw ex;
                }
                case SSL_ERROR_SSL:
                {
                    struct sockaddr_storage remoteAddr;
                    string desc = "<not available>";
                    if(IceInternal::fdToRemoteAddress(_fd, remoteAddr))
                    {
                        desc = IceInternal::addrToString(remoteAddr);
                    }
                    ostringstream ostr;
                    ostr << "SSL error occurred for new " << (_incoming ? "incoming" : "outgoing")
                         << " connection:\nremote address = " << desc << "\n" << _instance->sslErrors();
                    ProtocolException ex(__FILE__, __LINE__);
                    ex.reason = ostr.str();
                    throw ex;
                }
                }
            }
        }

        _instance->verifyPeer(_ssl, _fd, _host, _adapterName, _incoming);
    }
    catch(const Ice::LocalException& ex)
    {
        if(_instance->networkTraceLevel() >= 2)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "failed to establish ssl connection\n" << _desc << "\n" << ex;
        }
        throw;
    }

    if(_instance->networkTraceLevel() >= 1)
    {
        Trace out(_logger, _instance->networkTraceCategory());
        if(_incoming)
        {
            out << "accepted ssl connection\n" << _desc;
        }
        else
        {
            out << "ssl connection established\n" << _desc;
        }
    }

    if(_instance->securityTraceLevel() >= 1)
    {
        _instance->traceConnection(_ssl, _incoming);
    }

    return IceInternal::SocketOperationNone;
}

void
IceSSL::TransceiverI::checkSendSize(const IceInternal::Buffer& buf, size_t messageSizeMax)
{
    if(buf.b.size() > messageSizeMax)
    {
        throw MemoryLimitException(__FILE__, __LINE__);
    }
}

ConnectionInfo
IceSSL::TransceiverI::getConnectionInfo() const
{
    //
    // This can only be called on an open transceiver.
    //
    assert(_fd != INVALID_SOCKET);
    return populateConnectionInfo(_ssl, _fd, _adapterName, _incoming);
}

IceSSL::TransceiverI::TransceiverI(const InstancePtr& instance, SOCKET fd, const string& host, 
                                   const struct sockaddr_storage& addr) :
    NativeInfo(fd),
    _instance(instance),
    _logger(instance->communicator()->getLogger()),
    _stats(instance->communicator()->getStats()),
    _ssl(0),
    _host(host),
    _incoming(false),
    _state(StateNeedConnect)
#ifdef ICE_USE_IOCP
    , _read(IceInternal::SocketOperationRead), 
    _write(IceInternal::SocketOperationWrite)
#endif
{
    IceInternal::setBlock(fd, false);
    IceInternal::setTcpBufSize(fd, _instance->communicator()->getProperties(), _logger);

#ifndef ICE_USE_IOCP
    if(IceInternal::doConnect(_fd, addr))
    {
        _state = StateConnected;
        _desc = IceInternal::fdToString(_fd);
        if(_instance->networkTraceLevel() >= 1)
        {
            Trace out(_logger, _instance->networkTraceCategory());
            out << "ssl connection established\n" << _desc;
        }
    }
    else
    {
        _desc = IceInternal::fdToString(_fd);
    }
#else
    _connectAddr = addr;
#endif
}

IceSSL::TransceiverI::TransceiverI(const InstancePtr& instance, SOCKET fd, const string& adapterName) :
    NativeInfo(fd),
    _instance(instance),
    _logger(instance->communicator()->getLogger()),
    _stats(instance->communicator()->getStats()),
    _ssl(0),
    _incoming(true),
    _adapterName(adapterName),
    _state(StateConnected),
    _desc(IceInternal::fdToString(fd))
#ifdef ICE_USE_IOCP
    , _read(IceInternal::SocketOperationRead), 
    _write(IceInternal::SocketOperationWrite)
#endif
{
    IceInternal::setBlock(fd, false);
    IceInternal::setTcpBufSize(fd, _instance->communicator()->getProperties(), _logger);
}

IceSSL::TransceiverI::~TransceiverI()
{
    assert(_fd == INVALID_SOCKET);
}

#ifdef ICE_USE_IOCP
bool
IceSSL::TransceiverI::receive()
{
    if(_readI == _readBuffer.end())
    {
        assert(BIO_ctrl_get_read_request(_iocpBio));
        _readBuffer.resize(BIO_ctrl_get_read_request(_iocpBio));
        _readI = _readBuffer.begin();
    }

    int packetSize = static_cast<int>(_readBuffer.end() - _readI);
    while(_readI != _readBuffer.end())
    {
        assert(_fd != INVALID_SOCKET);
        ssize_t ret = ::recv(_fd, reinterpret_cast<char*>(&*_readI), packetSize, 0);
        if(ret == 0)
        {
            ConnectionLostException ex(__FILE__, __LINE__);
            ex.error = 0;
            throw ex;
        }

        if(ret == SOCKET_ERROR)
        {
            if(IceInternal::interrupted())
            {
                continue;
            }
            
            if(IceInternal::noBuffers() && packetSize > 1024)
            {
                packetSize /= 2;
                continue;
            }

            if(IceInternal::wouldBlock())
            {
                return false;
            }
            
            if(IceInternal::connectionLost())
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            else
            {
                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
        }

        _readI += ret;
        if(packetSize > _readBuffer.end() - _readI)
        {
            packetSize = static_cast<int>(_readBuffer.end() - _readI);
        }
    }

    assert(_readI == _readBuffer.end());
    int n = BIO_write(_iocpBio, &_readBuffer[0], static_cast<int>(_readBuffer.size()));
    assert(n == static_cast<int>(_readBuffer.size()));
    return true;
}

bool
IceSSL::TransceiverI::send()
{
    if(_writeI == _writeBuffer.end())
    {
        assert(BIO_ctrl_pending(_iocpBio));
        _writeBuffer.resize(BIO_ctrl_pending(_iocpBio));
        int n = BIO_read(_iocpBio, &_writeBuffer[0], static_cast<int>(_writeBuffer.size()));
        assert(n == static_cast<int>(_writeBuffer.size()));
        _writeI = _writeBuffer.begin();
    }

    int packetSize = static_cast<int>(_writeBuffer.end() - _writeI);
    if(_maxSendPacketSize > 0 && packetSize > _maxSendPacketSize)
    { 
        packetSize = _maxSendPacketSize;
    }

    while(_writeI != _writeBuffer.end())
    {
        int ret = ::send(_fd, reinterpret_cast<const char*>(&*_writeI), packetSize, 0);
        if(ret == 0)
        {
            ConnectionLostException ex(__FILE__, __LINE__);
            ex.error = 0;
            throw ex;
        }
        
        if(ret == SOCKET_ERROR)
        {
            if(IceInternal::interrupted())
            {
                continue;
            }

            if(IceInternal::noBuffers() && packetSize > 1024)
            {
                packetSize /= 2;
                continue;
            }

            if(IceInternal::wouldBlock())
            {
                return false;
            }
            
            if(IceInternal::connectionLost())
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
            else
            {
                SocketException ex(__FILE__, __LINE__);
                ex.error = IceInternal::getSocketErrno();
                throw ex;
            }
        }
        
        _writeI += ret;
        if(packetSize > _writeBuffer.end() - _writeI)
        {
            packetSize = static_cast<int>(_writeBuffer.end() - _writeI);
        }
    }
    return true;
}

#endif
