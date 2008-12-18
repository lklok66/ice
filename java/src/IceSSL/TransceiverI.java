// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceSSL;

import java.net.InetSocketAddress;

import IceInternal.AsyncCallback;
import IceInternal.Buffer;
import IceInternal.Network;
import IceInternal.TraceLevels;
import IceInternal.TransceiverReadThread;
import IceInternal.TransceiverWriteThread;

final class TransceiverI implements IceInternal.Transceiver
{
    class ReadThread extends TransceiverReadThread
    {
        protected void read(Buffer buf)
        {
            int remaining = 0;
            if(_traceLevels.network >= 3)
            {
                remaining = buf.b.remaining();
            }

            byte[] data = null;
            int off = 0;
            try
            {
                data = buf.b.array();
                off = buf.b.arrayOffset();
            }
            catch(UnsupportedOperationException ex)
            {
                assert(false);
            }

            int interval = 2000;
            while(buf.b.hasRemaining() && !destroyed())
            {
                int pos = buf.b.position();
                try
                {
                    assert(_fd != null);
                    _fd.setSoTimeout(interval);
                    int ret = _in.read(data, off + pos, buf.b.remaining());

                    if(ret == -1)
                    {
                        throw new Ice.ConnectionLostException();
                    }

                    if(ret > 0)
                    {
                        if(_traceLevels.network >= 3)
                        {
                            String s = "received " + ret + " of " + remaining + " bytes via ssl\n" + _desc;
                            _logger.trace(_traceLevels.networkCat, s);
                        }

                        if(_stats != null)
                        {
                            _stats.bytesReceived(type(), ret);
                        }

                        buf.b.position(pos + ret);
                    }
                }
                catch(java.net.SocketTimeoutException ex)
                {
                    if(ex.bytesTransferred > 0)
                    {
                        buf.b.position(pos + ex.bytesTransferred);
                    }
                }
                catch(java.io.InterruptedIOException ex)
                {
                    buf.b.position(pos + ex.bytesTransferred);
                }
                catch(java.io.IOException ex)
                {
                    if(IceInternal.Network.connectionLost(ex))
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

            if(destroyed())
            {
                throw new Ice.ConnectionLostException();
            }
        }
    }
    
    class WriteThread extends TransceiverWriteThread
    {
        protected void write(Buffer buf)
        {
            byte[] data = null;
            int off = 0;
            try
            {
                data = buf.b.array();
                off = buf.b.arrayOffset();
            }
            catch(UnsupportedOperationException ex)
            {
                assert(false);
            }

            while(buf.b.hasRemaining() && !destroyed())
            {
                int pos = buf.b.position();
                try
                {
                    assert(_fd != null);
                    int rem = buf.b.remaining();
                    _out.write(data, off + pos, rem);
                    buf.b.position(pos + rem);

                    if(_traceLevels.network >= 3)
                    {
                        String s = "sent " + rem + " of " + buf.b.limit() + " bytes via ssl\n" + _desc;
                        _logger.trace(_traceLevels.networkCat, s);
                    }

                    if(_stats != null)
                    {
                        _stats.bytesSent(type(), rem);
                    }

                    break;
                }
                catch(java.io.InterruptedIOException ex)
                {
                    buf.b.position(pos + ex.bytesTransferred);
                }
                catch(java.io.IOException ex)
                {
                    if(IceInternal.Network.connectionLost(ex))
                    {
                        //
                        // Java's SSL implementation might have successfully sent the
                        // packet but then detected loss of connection and raised an
                        // exception. As a result, we cannot be sure that it is safe
                        // to retry in this situation, so we raise LocalExceptionWrapper.
                        //
                        Ice.ConnectionLostException se = new Ice.ConnectionLostException();
                        se.initCause(ex);
                        // TODO: throw new IceInternal.LocalExceptionWrapper(se, false);
                        throw se;
                    }
                    
                    Ice.SocketException se = new Ice.SocketException();
                    se.initCause(ex);
                    throw se;
                }
            }

            if(destroyed() && buf.b.hasRemaining())
            {
                throw new Ice.ConnectionLostException();
            }
        }
    }
    
    // This thread is responsible for connecting the SSL socket in the case that the
    // ConnectorI created the transceiver, and then subsequently handshaking and validating
    // the SSL connection.
    class ConnectThread extends Thread
    {
        private AsyncCallback _callback;
        private javax.net.ssl.SSLSocket _connectfd = null;
        
        ConnectThread(AsyncCallback cb)
        {
            _callback = cb;
        }
        
        public javax.net.ssl.SSLSocket getFd()
        {
            return _connectfd;
        }
        
        public void run()
        {
            try
            {
                _connectfd = connect();
                validateConnection(_connectfd);
            }
            catch(RuntimeException ex)
            {
                if(_traceLevels.network >= 2)
                {
                    String s = "failed to establish ssl connection\n" + _desc + "\n" + ex;
                    _logger.trace(_traceLevels.networkCat, s);
                }
                _callback.complete(ex);
                return;
            }
            _callback.complete(null);
        }
        
        private javax.net.ssl.SSLSocket connect()
        {
            if(_incoming)
            {
                return _incomingfd;
            }

            javax.net.ssl.SSLSocket fd = null;
            try
            {
                javax.net.SocketFactory factory = _instance.context().getSocketFactory();
                fd = (javax.net.ssl.SSLSocket)factory.createSocket(_addr.getAddress(), _addr.getPort());

                fd.setUseClientMode(true);

                String[] cipherSuites = _instance.filterCiphers(fd.getSupportedCipherSuites(), fd.getEnabledCipherSuites());
                try
                {
                    fd.setEnabledCipherSuites(cipherSuites);
                }
                catch(IllegalArgumentException ex)
                {
                    Ice.SecurityException e = new Ice.SecurityException();
                    e.reason = "IceSSL: invalid ciphersuite";
                    e.initCause(ex);
                    throw e;
                }
                if(_instance.securityTraceLevel() > 0)
                {
                    StringBuffer s = new StringBuffer();
                    s.append("enabling SSL ciphersuites for socket\n" + IceInternal.Network.fdToString(fd) + ":");
                    for(int i = 0; i < cipherSuites.length; ++i)
                    {
                        s.append("\n  " + cipherSuites[i]);
                    }
                    _logger.trace(_instance.securityTraceCategory(), s.toString());
                }

                String[] protocols = _instance.protocols();
                if(protocols != null)
                {
                    try
                    {
                        fd.setEnabledProtocols(protocols);
                    }
                    catch(IllegalArgumentException ex)
                    {
                        Ice.SecurityException e = new Ice.SecurityException();
                        e.reason = "IceSSL: invalid protocol";
                        e.initCause(ex);
                        throw e;
                    }
                }

                fd.startHandshake();
                fd.getSession().invalidate();
                
                return fd;
            }
            catch(java.net.ConnectException ex)
            {
                if(fd != null)
                {
                    try
                    {
                        fd.close();
                    }
                    catch(java.io.IOException e)
                    {
                    }
                }
                Ice.ConnectFailedException se;
                if(IceInternal.Network.connectionRefused(ex))
                {
                    se = new Ice.ConnectionRefusedException();
                }
                else
                {
                    se = new Ice.ConnectFailedException();
                }
                se.initCause(ex);
                throw se;
            }
            catch(javax.net.ssl.SSLException ex)
            {
                if(fd != null)
                {
                    try
                    {
                        fd.close();
                    }
                    catch(java.io.IOException e)
                    {
                    }
                }
                Ice.SecurityException e = new Ice.SecurityException();
                e.initCause(ex);
                throw e;
            }
            catch(java.io.IOException ex)
            {
                if(fd != null)
                {
                    try
                    {
                        fd.close();
                    }
                    catch(java.io.IOException e)
                    {
                    }
                }

                if(IceInternal.Network.connectionLost(ex))
                {
                    throw new Ice.ConnectionLostException();
                }

                Ice.SocketException e = new Ice.SocketException();
                e.initCause(ex);
                throw e;
            }
            catch(RuntimeException ex)
            {
                if(fd != null)
                {
                    try
                    {
                        fd.close();
                    }
                    catch(java.io.IOException e)
                    {
                    }
                }
                throw ex;
            }
        }

        private void validateConnection(javax.net.ssl.SSLSocket fd)
        {
            if(!_incoming)
            {
                int verifyPeer = _instance.communicator().getProperties().getPropertyAsIntWithDefault(
                        "IceSSL.VerifyPeer", 2);
                if(verifyPeer > 0)
                {
                    try
                    {
                        fd.getSession().getPeerCertificates();
                    }
                    catch(javax.net.ssl.SSLPeerUnverifiedException ex)
                    {
                        Ice.SecurityException e = new Ice.SecurityException();
                        e.reason = "IceSSL: server did not supply a certificate";
                        e.initCause(ex);
                        throw e;
                    }
                }
            }
            
            _info = Util.populateConnectionInfo(fd.getSession(), fd, _adapterName, _incoming);
            _instance.verifyPeer(_info, fd, _host, _incoming);

            String desc = Network.fdToString(fd);
            if(_traceLevels.network >= 1)
            {
                String s;
                if(_incoming)
                {
                    s = "accepted ssl connection\n" + desc;
                }
                else
                {
                    s = "ssl connection established\n" + desc;
                }
                _logger.trace(_traceLevels.networkCat, s);
            }

            if(_instance.securityTraceLevel() >= 1)
            {
                _instance.traceConnection(fd, _incoming);
            }
        }
    }

    public boolean initialize(final AsyncCallback callback)
    {
        switch(_state)
        {
        case StateNeedConnect:
        {
            _state = StateConnecting;
            _connectThread = new ConnectThread(callback);
            _connectThread.setName("SSLConnectorThread");
            _connectThread.start();
            return false;
        }
        
        case StateConnecting:
        {
            _state = StateConnected;

            _fd = _connectThread.getFd();

            _desc = Network.fdToString(_fd);
            try
            {
                _in = _fd.getInputStream();
                _out = _fd.getOutputStream();
            }
            catch(java.io.IOException ex)
            {
                try
                {
                    _fd.close();
                }
                catch(java.io.IOException e)
                {
                }
                _fd = null;
                Ice.SocketException se = new Ice.SocketException();
                se.initCause(ex);
                throw se;
            }

            assert _readThread == null && _writeThread == null;
            
            StringBuffer desc = new StringBuffer();
            desc.append(_fd.getLocalAddress());
            desc.append(':');
            desc.append(_fd.getLocalPort());

            _readThread = new ReadThread();
            _readThread.setName("SSLTransceiverReadThread-" + desc);
            _readThread.start();
            
            _writeThread = new WriteThread();
            _writeThread.setName("SSLTransceiverWriteThread: " + desc);
            _writeThread.start();
            break;
        }
        
        case StateConnected:
            assert false; // Not expected
            
        case StateShutdown:
            _fd = _connectThread.getFd();
            try
            {
                _fd.close();
            }
            catch(java.io.IOException e)
            {
            }
            _fd = null;
            break;
        }
        return true;
    }
    
    public void
    close()
    {
        if(_traceLevels.network >= 1)
        {
            String s = "closing ssl connection\n" + _desc;
            _logger.trace(_traceLevels.networkCat, s);
        }
        
        // Return immediately if the connection hasn't completed yet.
        if(_fd == null)
        {
            return;
        }

        if(_readThread != null)
        {
            assert Thread.currentThread() != _readThread;
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
    
    public void shutdownReadWrite()
    {
        if(_traceLevels.network >= 2)
        {
            String s = "shutting down ssl connection for reading and writing\n" + _desc;
            _logger.trace(_traceLevels.networkCat, s);
        }
        _state = StateShutdown;

        if(_readThread != null)
        {
            _readThread.destroyThread();
        }
        if(_writeThread != null)
        {
            _writeThread.destroyThread();
        }

        /*
         * shutdownInput is not supported by an SSL socket.
         *
        try
        {
            _fd.shutdownInput(); // Shutdown socket for reading
            //_fd.shutdownOutput(); // Shutdown socket for writing
        }
        catch(UnsupportedOperationException ex)
        {
            // Ignore - shutdownInput not supported.
        }
        catch(java.net.SocketException ex)
        {
            // Ignore.
        }
        catch(java.io.IOException ex)
        {
            Ice.SocketException se = new Ice.SocketException();
            se.initCause(ex);
            throw se;
        }
        */
    }

    
    public void
    write(Buffer buf, AsyncCallback callback)
    {
        assert _state == StateConnected;
        _writeThread.write(buf, callback);
    }

    public void
    read(Buffer buf, AsyncCallback callback)
    {
        assert _state == StateConnected;
        _readThread.read(buf, callback);
    }

    public String
    type()
    {
        return "ssl";
    }

    public String
    toString()
    {
        return _desc;
    }

    public void
    checkSendSize(IceInternal.Buffer buf, int messageSizeMax)
    {
        if(buf.size() > messageSizeMax)
        {
            throw new Ice.MemoryLimitException();
        }
    }
    
    ConnectionInfo
    getConnectionInfo()
    {
        //
        // This can only be called on an open transceiver.
        //
        assert(_fd != null);
        return _info;
    }

    //
    // Only for use by AcceptorI.
    //
    TransceiverI(Instance instance, javax.net.ssl.SSLSocket fd, String host, String adapterName)
    {
        _incoming = true;
        _instance = instance;
        
        _adapterName = adapterName;
        _host = host;
        
        _traceLevels = instance.traceLevels();
        _logger = instance.getLogger();
        try
        {
            _stats = instance.communicator().getStats();
        }
        catch(Ice.CommunicatorDestroyedException ex)
        {
            // Ignore.
        }
        _incomingfd = fd;
        
        // Set temporary desc.
        _desc = Network.fdToString(_incomingfd);
    }

    //
    // Only for use by ConnectorI.
    //
    public TransceiverI(Instance instance, InetSocketAddress addr)
    {
        _incoming = false;
        _instance = instance;
        
        _host = addr.getHostName();
        _addr = addr;
        
        _traceLevels = instance.traceLevels();
        _logger = instance.getLogger();
        try
        {
            _stats = instance.communicator().getStats();
        }
        catch(Ice.CommunicatorDestroyedException ex)
        {
            // Ignore.
        }
        
        _desc = Network.addressesToString(_addr.getAddress(), 0, null, 0);
    }
 
    protected void
    finalize()
        throws Throwable
    {
        IceUtilInternal.Assert.FinalizerAssert(_fd == null);

        super.finalize();
    }
    
    private InetSocketAddress _addr; // Immutable
    private boolean _incoming; // Immutable
    private Instance _instance; // Immutable
    private TraceLevels _traceLevels; // Immutable
    private javax.net.ssl.SSLSocket _incomingfd;  // Immutable
    private Ice.Logger _logger; // Immutable
    private Ice.Stats _stats; // Immutable
    private String _host; // Immutable
    private String _adapterName; // Immutable

    private IceSSL.TransceiverI.ReadThread _readThread;
    private IceSSL.TransceiverI.WriteThread _writeThread;

    private javax.net.ssl.SSLSocket _fd;
    private java.io.InputStream _in;
    private java.io.OutputStream _out;
    private String _desc;
    private ConnectionInfo _info;
    private int _state = StateNeedConnect;

    private static final int StateNeedConnect = 0;
    private static final int StateConnecting = 1;
    private static final int StateConnected = 2;
    private static final int StateShutdown = 3;
    private ConnectThread _connectThread;
}
