// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{

    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Net;
    using System.Net.Sockets;

    sealed class TcpTransceiver : Transceiver
    {
        public int initialize()
        {
            if(_state == StateNeedConnect)
            {
                _state = StateConnectPending;
                return SocketOperation.Connect;
            }
            else if(_state <= StateConnectPending)
            {
                try
                {
#if SILVERLIGHT
                    Network.doFinishConnectAsync(_fd, _socketWriteEventArgs);
#else
                    Network.doFinishConnectAsync(_fd, _writeResult);
#endif
                    _writeResult = null;
                    _state = StateConnected;
                    _desc = Network.fdToString(_fd);
                }
                catch(Ice.LocalException ex)
                {
                    if(_traceLevels.network >= 2)
                    {
                        System.Text.StringBuilder s = new System.Text.StringBuilder();
                        s.Append("failed to establish tcp connection\n");
                        s.Append(Network.fdLocalAddressToString(_fd));
                        Debug.Assert(_addr != null);
                        s.Append("\nremote address = " + _addr.ToString() + "\n");
                        s.Append(ex.ToString());
                        _logger.trace(_traceLevels.networkCat, s.ToString());
                    }
                    throw;
                }

                if(_traceLevels.network >= 1)
                {
                    string s = "tcp connection established\n" + _desc;
                    _logger.trace(_traceLevels.networkCat, s);
                }
            }
            Debug.Assert(_state == StateConnected);
            return SocketOperation.None;
        }

        public void close()
        {
            //
            // If the transceiver is not connected, its description is simply "not connected",
            // which isn't very helpful.
            //
            if(_state == StateConnected && _traceLevels.network >= 1)
            {
                string s = "closing tcp connection\n" + ToString();
                _logger.trace(_traceLevels.networkCat, s);
            }

            Debug.Assert(_fd != null);
            try
            {
                _fd.Close();
            }
            catch(SocketException ex)
            {
                throw new Ice.SocketException(ex);
            }
            finally
            {
                _fd = null;
            }
        }

        public bool write(Buffer buf)
        {
#if COMPACT || SILVERLIGHT
            //
            // The Silverlight & Compact .NET Frameworks does not support the use of synchronous socket
            // operations on a non-blocking socket. Returning false here forces the
            // caller to schedule an asynchronous operation.
            //
            return false;
#else
            int packetSize = buf.b.remaining();
            if(AssemblyUtil.platform_ == AssemblyUtil.Platform.Windows)
            {
                //
                // On Windows, limiting the buffer size is important to prevent
                // poor throughput performance when transferring large amounts of
                // data. See Microsoft KB article KB823764.
                //
                if(_maxSendPacketSize > 0 && packetSize > _maxSendPacketSize / 2)
                {
                    packetSize = _maxSendPacketSize / 2;
                }
            }
            else
            {
                if(_blocking > 0)
                {
                    return false;
                }
            }

            while(buf.b.hasRemaining())
            {
                try
                {
                    Debug.Assert(_fd != null);

                    int ret;
                    try
                    {
                        ret = _fd.Send(buf.b.rawBytes(), buf.b.position(), packetSize, SocketFlags.None);
                    }
                    catch(SocketException e)
                    {
                        if(Network.wouldBlock(e))
                        {
                            return false;
                        }
                        throw;
                    }

                    Debug.Assert(ret > 0);

                    if(_traceLevels.network >= 3)
                    {
                        string s = "sent " + ret + " of " + packetSize + " bytes via tcp\n" + ToString();
                        _logger.trace(_traceLevels.networkCat, s);
                    }

                    if(_stats != null)
                    {
                        _stats.bytesSent(type(), ret);
                    }

                    buf.b.position(buf.b.position() + ret);
                    if(packetSize > buf.b.remaining())
                    {
                        packetSize = buf.b.remaining();
                    }
                }
                catch(SocketException ex)
                {
                    if(Network.connectionLost(ex))
                    {
                        throw new Ice.ConnectionLostException(ex);
                    }

                    throw new Ice.SocketException(ex);
                }
            }

            return true; // No more data to send.
#endif
        }

        public bool read(Buffer buf)
        {
#if COMPACT || SILVERLIGHT
            //
            // The Silverlight & Compact .NET Frameworks does not support the use of synchronous socket
            // operations on a non-blocking socket.
            //
            return false;
#else
            // COMPILERFIX: Workaround for Mac OS X broken poll(), see Mono bug #470120
            if(AssemblyUtil.osx_)
            {
                if(_blocking > 0)
                {
                    return false;
                }
            }

            int remaining = buf.b.remaining();
            int position = buf.b.position();

            while(buf.b.hasRemaining())
            {
                try
                {
                    Debug.Assert(_fd != null);

                    int ret;
                    try
                    {
                        //
                        // Try to receive first. Much of the time, this will work and we
                        // avoid the cost of calling Poll().
                        //
                        ret = _fd.Receive(buf.b.rawBytes(), position, remaining, SocketFlags.None);
                        if(ret == 0)
                        {
                            throw new Ice.ConnectionLostException();
                        }
                    }
                    catch(SocketException e)
                    {
                        if(Network.wouldBlock(e))
                        {
                            return false;
                        }
                        throw;
                    }

                    Debug.Assert(ret > 0);

                    if(_traceLevels.network >= 3)
                    {
                        string s = "received " + ret + " of " + remaining + " bytes via tcp\n" + ToString();
                        _logger.trace(_traceLevels.networkCat, s);
                    }

                    if(_stats != null)
                    {
                        _stats.bytesReceived(type(), ret);
                    }

                    remaining -= ret;
                    buf.b.position(position += ret);
                }
                catch(SocketException ex)
                {
                    //
                    // On Mono, calling shutdownReadWrite() followed by read() causes Socket.Receive() to
                    // raise a socket exception with the "interrupted" error code. We need to check the
                    // socket's Connected status before checking for the interrupted case.
                    //
                    if(!_fd.Connected)
                    {
                        throw new Ice.ConnectionLostException(ex);
                    }

                    if(Network.interrupted(ex))
                    {
                        continue;
                    }

                    if(Network.connectionLost(ex))
                    {
                        throw new Ice.ConnectionLostException(ex);
                    }

                    throw new Ice.SocketException(ex);
                }
            }

            return true;
#endif
        }

        public bool startRead(Buffer buf, 
#if SILVERLIGHT
                              IceInternal.ThreadPool.AsyncCallback callback,
#else
                              AsyncCallback callback, 
#endif
                              object state)
        {
            Debug.Assert(_fd != null && _readResult == null);

#if !COMPACT && !SILVERLIGHT
            // COMPILERFIX: Workaround for Mac OS X broken poll(), see Mono bug #470120
            if(AssemblyUtil.osx_)
            {
                if(++_blocking == 1)
                {
                    Network.setBlock(_fd, true);
                }
            }
#endif

            int packetSize = buf.b.remaining();
            if(_maxReceivePacketSize > 0 && packetSize > _maxReceivePacketSize)
            {
                packetSize = _maxReceivePacketSize;
            }

            try
            {
#if SILVERLIGHT
                if(_readResult == null)
                {
                    _readResult = new IceInternal.ThreadPool.IAsyncResult();
                }
                _socketReadEventArgs = new SocketAsyncEventArgs();
                _socketReadEventArgs.RemoteEndPoint = _addr;
                _socketReadEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(callback);
                _socketReadEventArgs.UserToken = _readResult;
                _readResult.AsyncState = state;
                _readResult.EventArgs = _socketReadEventArgs;
                _socketReadEventArgs.SetBuffer(buf.b.rawBytes(), buf.b.position(), packetSize);
                _readResult.CompletedSynchronously = !_fd.ReceiveAsync(_socketReadEventArgs);
#else
                _readResult = _fd.BeginReceive(buf.b.rawBytes(), buf.b.position(), packetSize, SocketFlags.None, 
                                               callback, state);
#endif
            }
            catch(SocketException ex)
            {
                if(Network.connectionLost(ex))
                {
                    throw new Ice.ConnectionLostException(ex);
                }

                throw new Ice.SocketException(ex);
            }


            return _readResult.CompletedSynchronously;
        }

        public void finishRead(Buffer buf)
        {
            if(_fd == null) // Transceiver was closed
            {
                _readResult = null;
                return;
            }

            Debug.Assert(_fd != null && _readResult != null);

            try
            {
#if SILVERLIGHT
                SocketAsyncEventArgs eventArgs = (SocketAsyncEventArgs)_readResult.EventArgs;
                int ret = 0;
                if(eventArgs.SocketError == SocketError.Success)
                {
                    ret = eventArgs.BytesTransferred;
                }
#else
                int ret = _fd.EndReceive(_readResult);
#endif
                _readResult = null;
                if(ret == 0)
                {
                    throw new Ice.ConnectionLostException();
                }

#if !COMPACT && !SILVERLIGHT
                // COMPILERFIX: Workaround for Mac OS X broken poll(), see Mono bug #470120
                if(AssemblyUtil.osx_)
                {
                    if(--_blocking == 0)
                    {
                        Network.setBlock(_fd, false);
                    }
                }
#endif
                Debug.Assert(ret > 0);

                if(_traceLevels.network >= 3)
                {
                    int packetSize = buf.b.remaining();
                    if(_maxReceivePacketSize > 0 && packetSize > _maxReceivePacketSize)
                    {
                        packetSize = _maxReceivePacketSize;
                    }
                    string s = "received " + ret + " of " + packetSize + " bytes via tcp\n" + ToString();
                    _logger.trace(_traceLevels.networkCat, s);
                }

                if(_stats != null)
                {
                    _stats.bytesReceived(type(), ret);
                }

                buf.b.position(buf.b.position() + ret);
            }
            catch(SocketException ex)
            {
                if(Network.connectionLost(ex))
                {
                    throw new Ice.ConnectionLostException(ex);
                }

                throw new Ice.SocketException(ex);
            }
            catch(ObjectDisposedException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
        }

        public bool startWrite(Buffer buf, 
#if SILVERLIGHT
                               IceInternal.ThreadPool.AsyncCallback callback, 
#else
                               AsyncCallback callback, 
#endif
                               object state, out bool completed)
        {
            Debug.Assert(_fd != null && _writeResult == null);

#if !COMPACT && !SILVERLIGHT
            // COMPILERFIX: Workaround for Mac OS X broken poll(), see Mono bug #470120
            if(AssemblyUtil.osx_)
            {
                if(++_blocking == 1)
                {
                    Network.setBlock(_fd, true);
                }
            }
#endif

            if(_state < StateConnected)
            {
#if SILVERLIGHT
                _socketWriteEventArgs = new SocketAsyncEventArgs();
                _writeResult = Network.doConnectAsync(_fd, _addr, callback, _socketWriteEventArgs, state);
#else
                _writeResult = Network.doConnectAsync(_fd, _addr, callback, state);
#endif
                completed = false;
                return _writeResult.CompletedSynchronously;
            }

            //
            // We limit the packet size for beginWrite to ensure connection timeouts are based
            // on a fixed packet size.
            //
            int packetSize = buf.b.remaining();
            if(_maxSendPacketSize > 0 && packetSize > _maxSendPacketSize)
            {
                packetSize = _maxSendPacketSize;
            }

            try
            {
#if SILVERLIGHT
                _socketWriteEventArgs.SetBuffer(buf.b.rawBytes(), buf.b.position(), packetSize);
                _writeResult = new IceInternal.ThreadPool.IAsyncResult();
                _socketWriteEventArgs.UserToken = _writeResult;
                _writeResult.AsyncState = state;
                _writeResult.EventArgs = _socketWriteEventArgs;
                _writeResult.CompletedSynchronously = !_fd.SendAsync(_socketWriteEventArgs);
#else
                _writeResult = _fd.BeginSend(buf.b.rawBytes(), buf.b.position(), packetSize, SocketFlags.None, 
                                             callback, state);
#endif
            }
            catch(SocketException ex)
            {
                if(Network.connectionLost(ex))
                {
                    throw new Ice.ConnectionLostException(ex);
                }

                throw new Ice.SocketException(ex);
            }
            catch(ObjectDisposedException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
            
            completed = packetSize == buf.b.remaining();
	        return _writeResult.CompletedSynchronously;
        }

        public void finishWrite(Buffer buf)
        {
            if(_fd == null) // Transceiver was closed
            {
                if(buf.size() - buf.b.position() < _maxSendPacketSize)
                {
                    buf.b.position(buf.size()); // Assume all the data was sent for at-most-once semantics.
                }
                _writeResult = null;
                return;
            }

            Debug.Assert(_fd != null && _writeResult != null);

            if(_state < StateConnected)
            {
                return;
            }

            try
            {
#if SILVERLIGHT
                SocketAsyncEventArgs eventArgs = (SocketAsyncEventArgs)_writeResult.EventArgs;
                int ret = 0;
                if(eventArgs.SocketError == SocketError.Success)
                {
                    ret = eventArgs.BytesTransferred;
                }
#else
                int ret = _fd.EndSend(_writeResult);
#endif
                _writeResult = null;
                if(ret == 0)
                {
                    throw new Ice.ConnectionLostException();
                }
                Debug.Assert(ret > 0);

#if !COMPACT && !SILVERLIGHT
                // COMPILERFIX: Workaround for Mac OS X broken poll(), see Mono bug #470120
                if(AssemblyUtil.osx_)
                {
                    if(--_blocking == 0)
                    {
                        Network.setBlock(_fd, false);
                    }
                }
#endif

                if(_traceLevels.network >= 3)
                {
                    int packetSize = buf.b.remaining();
                    if(_maxSendPacketSize > 0 && packetSize > _maxSendPacketSize)
                    {
                        packetSize = _maxSendPacketSize;
                    }
                    string s = "sent " + ret + " of " + packetSize + " bytes via tcp\n" + ToString();
                    _logger.trace(_traceLevels.networkCat, s);
                }

                if(_stats != null)
                {
                    _stats.bytesSent(type(), ret);
                }

                buf.b.position(buf.b.position() + ret);
            }
            catch(SocketException ex)
            {
                if(Network.connectionLost(ex))
                {
                    throw new Ice.ConnectionLostException(ex);
                }

                throw new Ice.SocketException(ex);
            }
            catch(ObjectDisposedException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
        }

        public string type()
        {
            return "tcp";
        }

        public Ice.ConnectionInfo
        getInfo()
        {
            Debug.Assert(_fd != null);
            Ice.TCPConnectionInfo info = new Ice.TCPConnectionInfo();
            EndPoint localEndpoint = Network.getLocalAddress(_fd);
            info.localAddress = Network.endpointAddressToString(localEndpoint);
            info.localPort = Network.endpointPort(localEndpoint);
            EndPoint remoteEndpoint = Network.getRemoteAddress(_fd);
            info.remoteAddress = Network.endpointAddressToString(remoteEndpoint);
            info.remotePort = Network.endpointPort(remoteEndpoint);
            return info;
        }

        public void checkSendSize(Buffer buf, int messageSizeMax)
        {
            if(buf.size() > messageSizeMax)
            {
                Ex.throwMemoryLimitException(buf.size(), messageSizeMax);
            }
        }

        public override string ToString()
        {
            return _desc;
        }

        //
        // Only for use by TcpConnector, TcpAcceptor
        //
        internal TcpTransceiver(Instance instance, Socket fd, EndPoint addr, bool connected)
        {
            _fd = fd;
#if SILVERLIGHT
            _addr = (DnsEndPoint)addr;
#else
            _addr = (IPEndPoint)addr;
#endif
            _traceLevels = instance.traceLevels();
            _logger = instance.initializationData().logger;
            _stats = instance.initializationData().stats;
            _state = connected ? StateConnected : StateNeedConnect;
            _desc = connected ? Network.fdToString(_fd) : "<not connected>";

            _maxSendPacketSize = Network.getSendBufferSize(fd);
            if(_maxSendPacketSize < 512)
            {
                _maxSendPacketSize = 0;
            }

            _maxReceivePacketSize = Network.getRecvBufferSize(fd);
            if(_maxReceivePacketSize < 512)
            {
                _maxReceivePacketSize = 0;
            }
        }

        private Socket _fd;
#if SILVERLIGHT
        private DnsEndPoint _addr;
#else
        private IPEndPoint _addr;
#endif
        private TraceLevels _traceLevels;
        private Ice.Logger _logger;
        private Ice.Stats _stats;
        private string _desc;
        private int _state;
        private int _maxSendPacketSize;
        private int _maxReceivePacketSize;

#if !SILVERLIGHT
        private int _blocking = 0;
#endif	
#if SILVERLIGHT
        private SocketAsyncEventArgs	_socketWriteEventArgs;
        private SocketAsyncEventArgs	_socketReadEventArgs;
        private IceInternal.ThreadPool.IAsyncResult _writeResult;
        private IceInternal.ThreadPool.IAsyncResult _readResult;
#else
        private IAsyncResult _writeResult;
        private IAsyncResult _readResult;
#endif
        private const int StateNeedConnect = 0;
        private const int StateConnectPending = 1;
        private const int StateConnected = 2;
    }

}
