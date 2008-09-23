// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Transceiver.h>

#include <Ice/Instance.h>
#include <Ice/TraceLevels.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Stats.h>
#include <Ice/Buffer.h>
#include <Ice/Network.h>
#include <Ice/LocalException.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

SOCKET
IceObjC::Transceiver::fd()
{
    assert(_fd != INVALID_SOCKET);
    return _fd;
}

void
IceObjC::Transceiver::close()
{
    if(_traceLevels->network >= 1)
    {
        Trace out(_logger, _traceLevels->networkCat);
        out << "closing " << _type << " connection\n" << toString();
    }

    CFReadStreamClose(_readStream);
    CFWriteStreamClose(_writeStream);

    assert(_fd != INVALID_SOCKET);
    try
    {
        closeSocket(_fd);
        _fd = INVALID_SOCKET;
    }
    catch(const SocketException&)
    {
        _fd = INVALID_SOCKET;
        throw;
    }
}

bool
IceObjC::Transceiver::write(Buffer& buf)
{
    // Its impossible for the packetSize to be more than an Int.
    int packetSize = static_cast<int>(buf.b.end() - buf.i);
    
    while(buf.i != buf.b.end())
    {
        if(!CFWriteStreamCanAcceptBytes(_writeStream) && CFWriteStreamGetStatus(_writeStream) != kCFStreamStatusError)
        {
            return false;
        }
        assert(CFWriteStreamGetStatus(_writeStream) >= kCFStreamStatusOpen);

        assert(_fd != INVALID_SOCKET);
        CFIndex ret = CFWriteStreamWrite(_writeStream, reinterpret_cast<const UInt8*>(&*buf.i), packetSize);

        if(ret == SOCKET_ERROR)
        {
            assert(CFWriteStreamGetStatus(_writeStream) == kCFStreamStatusError);
            CFErrorRef err = CFWriteStreamCopyError(_writeStream);
            errno = CFErrorGetCode(err);
            CFRelease(err);

            if(interrupted())
            {
                continue;
            }

            if(noBuffers() && packetSize > 1024)
            {
                packetSize /= 2;
                continue;
            }

            if(wouldBlock())
            {
                return false;
            }
            
            if(connectionLost())
            {
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = getSocketErrno();
                throw ex;
            }
            else
            {
                SocketException ex(__FILE__, __LINE__);
                ex.error = getSocketErrno();
                throw ex;
            }
        }

        if(_traceLevels->network >= 3)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << "sent " << ret << " of " << packetSize << " bytes via " << _type << "\n" << toString();
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
IceObjC::Transceiver::read(Buffer& buf)
{
    // Its impossible for the packetSize to be more than an Int.
    int packetSize = static_cast<int>(buf.b.end() - buf.i);
    
    while(buf.i != buf.b.end())
    {
        if(!CFReadStreamHasBytesAvailable(_readStream) && CFReadStreamGetStatus(_readStream) != kCFStreamStatusError)
        {
            return false;
        }
        assert(CFReadStreamGetStatus(_readStream) >= kCFStreamStatusOpen);

        assert(_fd != INVALID_SOCKET);
        CFIndex ret = CFReadStreamRead(_readStream, reinterpret_cast<UInt8*>(&*buf.i), packetSize);

        if(ret == 0)
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

        if(ret == SOCKET_ERROR)
        {
            assert(CFReadStreamGetStatus(_readStream) == kCFStreamStatusError);
            CFErrorRef err = CFReadStreamCopyError(_readStream);
            errno = CFErrorGetCode(err);
            CFRelease(err);

            if(interrupted())
            {
                continue;
            }
            
            if(noBuffers() && packetSize > 1024)
            {
                packetSize /= 2;
                continue;
            }

            if(wouldBlock())
            {
                return false;
            }
            
            if(connectionLost())
            {
                //
                // See the commment above about shutting down the
                // socket if the connection is lost while reading
                // data.
                //
                //assert(_fd != INVALID_SOCKET);
                //shutdownSocketReadWrite(_fd);
            
                ConnectionLostException ex(__FILE__, __LINE__);
                ex.error = getSocketErrno();
                throw ex;
            }
            else
            {
                SocketException ex(__FILE__, __LINE__);
                ex.error = getSocketErrno();
                throw ex;
            }
        }

        if(_traceLevels->network >= 3)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << "received " << ret << " of " << packetSize << " bytes via " << _type << "\n" << toString();
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

bool
IceObjC::Transceiver::hasMoreData()
{
    return CFReadStreamHasBytesAvailable(_readStream) || CFReadStreamGetStatus(_readStream) == kCFStreamStatusError;
}

string
IceObjC::Transceiver::type() const
{
    return _type;
}

string
IceObjC::Transceiver::toString() const
{
    return _desc;
}

SocketStatus
IceObjC::Transceiver::initialize()
{
    if(_state == StateNeedConnect)
    {
        _state = StateConnectPending;
        return NeedConnect;
    }
    else if(_state <= StateConnectPending)
    {
        try
        {
            CFStreamStatus status = CFReadStreamGetStatus(_readStream);
            if(status == kCFStreamStatusOpening)
            {
                return NeedConnect;
            }
            else if(status == kCFStreamStatusError)
            {
                CFErrorRef err = CFReadStreamCopyError(_readStream);
                errno = CFErrorGetCode(err);
                CFRelease(err);

                if(connectionRefused())
                {
                    ConnectionRefusedException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
                else if(connectFailed())
                {
                    ConnectFailedException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
                else
                {
                    SocketException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
            }

            assert(status == kCFStreamStatusOpen);
            _state = StateConnected;
            _desc = fdToString(_fd);
        }
        catch(const Ice::LocalException& ex)
        {
            if(_traceLevels->network >= 2)
            {
                Trace out(_logger, _traceLevels->networkCat);
                out << "failed to establish " << _type << " connection\n" << _desc << "\n" << ex;
            }
            throw;
        }

        if(_traceLevels->network >= 1)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << _type << " connection established\n" << _desc;
        }
    }
    assert(_state == StateConnected);
    return Finished;
}

void
IceObjC::Transceiver::checkSendSize(const Buffer& buf, size_t messageSizeMax)
{
    if(buf.b.size() > messageSizeMax)
    {
        throw MemoryLimitException(__FILE__, __LINE__);
    }
}

IceObjC::Transceiver::Transceiver(const InstancePtr& instance, 
                                  SOCKET fd, 
                                  const string& type,
                                  CFReadStreamRef readStream,
                                  CFWriteStreamRef writeStream,
                                  bool connected) :
    _traceLevels(instance->traceLevels()),
    _logger(instance->initializationData().logger),
    _stats(instance->initializationData().stats),
    _type(type),
    _fd(fd),
    _readStream(readStream),
    _writeStream(writeStream),
    _state(connected ? StateConnected : StateNeedConnect),
    _desc(fdToString(_fd))
{
}

IceObjC::Transceiver::~Transceiver()
{
    assert(_fd == INVALID_SOCKET);
    CFRelease(_readStream);
    CFRelease(_writeStream);
}
