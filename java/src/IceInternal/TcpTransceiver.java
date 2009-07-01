// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;


final class TcpTransceiver implements Transceiver
{
    public boolean
    initialize(final AsyncCallback callback)
    {
        switch(_state)
        {
        case StateNeedConnect:
        {
            // Don't share the _fd with the transceiver. close could be called
            // while the connect thread is running.
            final java.nio.channels.SocketChannel fd = _fd;
            _state = StateConnecting;
            Thread connectThread = new Thread(new Runnable()
            {
                public void run()
                {
                    // Here the connect blocks until complete. If the
                    // transceiver is closed in the meantime, an IOException is
                    // thrown.
                    try
                    {
                        Network.doConnect(fd, _addr);
                    }
                    catch(RuntimeException ex)
                    {
                        if(_traceLevels.network >= 2)
                        {
                            String s = "failed to establish tcp connection\n" + _desc + "\n" + ex;
                            _logger.trace(_traceLevels.networkCat, s);
                        }
                        callback.complete(ex);
                        return;
                    }
                    callback.complete(null);
                }
            });
            
            connectThread.setName("TcpConnectorThread");
            connectThread.start();
            return false;
        }
        
        case StateConnecting:
        {
            assert(_fd != null);

            _state = StateConnected;
            //
            // TODO: Android bug 3123 prevents us from obtaining a useful description
            // of the remote socket address after we have set socket options. We'll use
            // the original target address until this bug is fixed.
            //
            //_desc = Network.fdToString(_fd);
            _desc = Network.addressesToString((java.net.InetSocketAddress)_fd.socket().getLocalSocketAddress(), _addr);

            if(_traceLevels.network >= 1)
            {
                String s = "tcp connection established\n" + _desc;
                _logger.trace(_traceLevels.networkCat, s);
            }
            
            startThreads();
            break;
        }
        
        case StateConnected:
        case StateShutdown:
            break;
        }

        return true;
    }
    
    class ReadThread extends TransceiverReadThread
    {
        protected void read(Buffer buf)
        {
            int remaining = 0;
            if(_traceLevels.network >= 3)
            {
                remaining = buf.b.remaining();
            }

            while(buf.b.hasRemaining())
            {
                try
                {
                    assert(_fd != null);
                    int ret = _fd.read(buf.b);

                    if(ret == -1)
                    {
                        throw new Ice.ConnectionLostException();
                    }
                    assert ret != 0;

                    if(ret > 0)
                    {
                        if(_traceLevels.network >= 3)
                        {
                            String s = "received " + ret + " of " + remaining + " bytes via tcp\n" + _desc;
                            _logger.trace(_traceLevels.networkCat, s);
                        }

                        if(_stats != null)
                        {
                            _stats.bytesReceived(type(), ret);
                        }
                    }
                }
                catch(java.io.InterruptedIOException ex)
                {
                    continue;
                }
                catch(java.io.IOException ex)
                {
                    if(Network.connectionLost(ex))
                    {
                        Ice.ConnectionLostException se = new Ice.ConnectionLostException();
                        se.initCause(ex);
                        throw se;
                    }

                    Ice.SocketException se = new Ice.SocketException();
                    se.initCause(ex);
                    throw se;
                }
            }
        }
    }
    
    class WriteThread extends TransceiverWriteThread
    {
        protected void write(Buffer buf)
        {
            final int size = buf.b.limit();
            int packetSize = size - buf.b.position();
            if(_maxPacketSize > 0 && packetSize > _maxPacketSize)
            {
                packetSize = _maxPacketSize;
                buf.b.limit(buf.b.position() + packetSize);
            }

            while(buf.b.hasRemaining())
            {
                try
                {
                    assert (_fd != null);
                    int ret = _fd.write(buf.b);

                    if(ret == -1)
                    {
                        throw new Ice.ConnectionLostException();
                    }
                    assert ret != 0;
                    if(_traceLevels.network >= 3)
                    {
                        String s = "sent " + ret + " of " + size + " bytes via tcp\n" + _desc;
                        _logger.trace(_traceLevels.networkCat, s);
                    }

                    if(_stats != null)
                    {
                        _stats.bytesSent(type(), ret);
                    }

                    if(packetSize == _maxPacketSize)
                    {
                        assert (buf.b.position() == buf.b.limit());
                        packetSize = size - buf.b.position();
                        if(packetSize > _maxPacketSize)
                        {
                            packetSize = _maxPacketSize;
                        }
                        buf.b.limit(buf.b.position() + packetSize);
                    }
                }
                catch(java.io.InterruptedIOException ex)
                {
                    continue;
                }
                catch(java.io.IOException ex)
                {
                    Ice.SocketException se = new Ice.SocketException();
                    se.initCause(ex);
                    throw se;
                }
            }
        }
    }

    private void startThreads()
    {
        // Post connection establishment the socket should be blocking.
        Network.setBlock(_fd, true);
        
        StringBuffer desc = new StringBuffer();
        desc.append(_fd.socket().getLocalAddress());
        desc.append(':');
        desc.append(_fd.socket().getLocalPort());

        _readThread = new ReadThread();
        _readThread.setName("TcpTransceiverReadThread-" + desc);
        _readThread.start();
        
        _writeThread = new WriteThread();
        _writeThread.setName("TcpTransceiverWriteThread-" + desc);
        _writeThread.start();
    }

    public void
    close()
    {
        if(_traceLevels.network >= 1)
        {
            String s = "closing tcp connection\n" + _desc;
            _logger.trace(_traceLevels.networkCat, s);
        }

        if(_readThread != null)
        {
            assert Thread.currentThread() != _readThread;
            _readThread.destroyThread();
            while(_readThread.isAlive())
            {
                try
                {
                    _readThread.join();
                }
                catch(InterruptedException e)
                {
                }
            }
        }
        if(_writeThread != null)
        {
            assert Thread.currentThread() != _writeThread;
            _writeThread.destroyThread();
            while(_writeThread.isAlive())
            {
                try
                {
                    _writeThread.join();
                }
                catch(InterruptedException e)
                {
                }
            }
        }
        
        assert(_fd != null);
        try
        {
            _fd.close();
        }
        catch(java.io.IOException ex)
        {
            Ice.SocketException se = new Ice.SocketException();
            se.initCause(ex);
            throw se;
        }
        finally
        {
            _fd = null;
        }
    }
    
    public void
    shutdownReadWrite()
    {
        if(_traceLevels.network >= 2)
        {
            String s = "shutting down tcp connection for reading and writing\n" + _desc;
            _logger.trace(_traceLevels.networkCat, s);
        }
        
        _state = StateShutdown;

        assert(_fd != null);
        java.net.Socket socket = _fd.socket();
        try
        {
            socket.shutdownInput(); // Shutdown socket for reading
            socket.shutdownOutput(); // Shutdown socket for writing
        }
        catch(java.net.SocketException ex)
        {
            // Ignore.
        }
        catch(java.io.IOException ex)
        {
            // Ignore this. It can occur if a connection attempt fails.
        }
    }
    
    public void
    write(Buffer buf, AsyncCallback callback)
    {
        _writeThread.write(buf, callback);
    }

    public void
    read(Buffer buf, AsyncCallback callback)
    {
        _readThread.read(buf, callback);
    }

    public String
    type()
    {
        return "tcp";
    }

    public String
    toString()
    {
        return _desc;
    }

    public void
    checkSendSize(Buffer buf, int messageSizeMax)
    {
        if(buf.size() > messageSizeMax)
        {
            throw new Ice.MemoryLimitException();
        }
    }

    //
    // Only for use by TcpAcceptor
    //
    TcpTransceiver(Instance instance, java.nio.channels.SocketChannel fd)
    {
        _fd = fd;
        _traceLevels = instance.traceLevels();
        _logger = instance.initializationData().logger;
        _stats = instance.initializationData().stats;
        _desc = Network.fdToString(_fd);
        _state = StateConnected;
        startThreads();            
    }

    //
    // Only for use by TcpConnector
    //
    TcpTransceiver(Instance instance, java.net.InetSocketAddress addr)
    {
        _fd = Network.createTcpSocket();
        _addr = addr;
        _traceLevels = instance.traceLevels();
        _logger = instance.initializationData().logger;
        _stats = instance.initializationData().stats;
        _desc = Network.fdToString(_fd);
        _state = StateNeedConnect;   
    }

    protected synchronized void
    finalize()
        throws Throwable
    {
        IceUtilInternal.Assert.FinalizerAssert(_fd == null);

        super.finalize();
    }

    private WriteThread _writeThread;
    private ReadThread _readThread;

    private java.nio.channels.SocketChannel _fd;
    private TraceLevels _traceLevels;
    private Ice.Logger _logger;
    private Ice.Stats _stats;
    private String _desc;
    private int _state;
    private int _maxPacketSize = 0;

    private java.net.InetSocketAddress _addr;
    
    private static final int StateNeedConnect = 0;
    private static final int StateConnecting = 1;
    private static final int StateConnected = 2;
    private static final int StateShutdown = 3;
}
