// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceSSL;

import IceInternal.Network;
import IceInternal.TraceLevels;

final class AcceptorI implements IceInternal.Acceptor
{
    public void
    close()
    {
        if(_traceLevels.network >= 1)
        {
            String s = "stopping to accept ssl connections at " + toString();
            _logger.trace(_traceLevels.networkCat, s);
        }

        javax.net.ssl.SSLServerSocket fd;
        synchronized(this)
        {
            fd = _fd;
            _fd = null;
        }
        if(fd != null)
        {
            try
            {
                fd.close();
            }
            catch(java.io.IOException ex)
            {
                // Ignore.
            }
        }
    }

    public void
    listen()
    {
        // Nothing to do.

        if(_traceLevels.network >= 1)
        {
            String s = "accepting ssl connections at " + toString();
            _logger.trace(_traceLevels.networkCat, s);
        }
    }

    public IceInternal.Transceiver
    accept()
    {
        //
        // The plugin may not be fully initialized.
        //
        if(!_instance.initialized())
        {
            Ice.PluginInitializationException ex = new Ice.PluginInitializationException();
            ex.reason = "IceSSL: plugin is not initialized";
            throw ex;
        }
        
        javax.net.ssl.SSLSocket fd = null;
        try
        {
            fd = (javax.net.ssl.SSLSocket)_fd.accept();

            //
            // Check whether this socket is the result of a call to connectToSelf.
            // Despite the fact that connectToSelf immediately closes the socket,
            // the server-side handshake process does not raise an exception.
            // Furthermore, we can't simply proceed with the regular handshake
            // process because we don't want to pass such a socket to the
            // certificate verifier (if any).
            //
            // In order to detect a call to connectToSelf, we compare the remote
            // address of the newly-accepted socket to that in _connectToSelfAddr.
            //
            java.net.SocketAddress remoteAddr = fd.getRemoteSocketAddress();
            synchronized(this)
            {
                if(remoteAddr.equals(_connectToSelfAddr))
                {
                    try
                    {
                        fd.close();
                    }
                    catch(java.io.IOException e)
                    {
                    }
                    return null;
                }
            }

            fd.getSession();
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

            //
            // Unfortunately, the situation where the cipher suite does not match
            // the certificates is not detected until accept is called. If we were
            // to throw a LocalException, the IncomingConnectionFactory would
            // simply log it and call accept again, resulting in an infinite loop.
            // To avoid this problem, we check for the special case and throw
            // an exception that IncomingConnectionFactory doesn't trap.
            //
            if(ex.getMessage().toLowerCase().startsWith("no available certificate corresponds to the ssl cipher " +
                                                        "suites which are enabled"))
            {
                RuntimeException e = new RuntimeException();
                e.initCause(ex);
                throw e;
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

        if(_traceLevels.network >= 1)
        {
            _logger.trace(_traceLevels.networkCat, "accepting ssl connection\n" +
                          IceInternal.Network.fdToString(fd));
        }

        return new TransceiverI(_instance, fd, _host, _adapterName);
    }

    public void connectToSelf()
    {
        java.nio.channels.SocketChannel fd = IceInternal.Network.createTcpSocket();
        synchronized(this)
        {
            //
            // connectToSelf is called to wake up the thread blocked in
            // accept. We remember the originating address for use in
            // accept. See accept for details.
            //
            IceInternal.Network.doConnect(fd, _addr);
            _connectToSelfAddr = (java.net.InetSocketAddress)fd.socket().getLocalSocketAddress();
        }
        IceInternal.Network.closeSocket(fd);
    }

    public String
    toString()
    {
        return IceInternal.Network.addrToString(_addr);
    }

    int
    effectivePort()
    {
        return _addr.getPort();
    }

    AcceptorI(Instance instance, String adapterName, String host, int port)
    {
        _instance = instance;
        _traceLevels = instance.traceLevels();
        _adapterName = adapterName;
        _host = host;
        _logger = instance.communicator().getLogger();
        _backlog = instance.communicator().getProperties().getPropertyAsIntWithDefault("Ice.TCP.Backlog", 511);

        try
        {
            javax.net.ssl.SSLServerSocketFactory factory = _instance.context().getServerSocketFactory();
            _addr = Network.getAddressForServer(host, port, _instance.protocolSupport());
            if(_traceLevels.network >= 2)
            {
                String s = "attempting to bind to ssl socket " + toString();
                _logger.trace(_traceLevels.networkCat, s);
            }

            _fd = (javax.net.ssl.SSLServerSocket)factory.createServerSocket(port, _backlog, _addr.getAddress());
            // Retrieve the address to find out to which port we're bound.
            _addr = (java.net.InetSocketAddress)_fd.getLocalSocketAddress();

            int verifyPeer =
                _instance.communicator().getProperties().getPropertyAsIntWithDefault("IceSSL.VerifyPeer", 2);
            if(verifyPeer == 0)
            {
                _fd.setWantClientAuth(false);
                _fd.setNeedClientAuth(false);
            }
            else if(verifyPeer == 1)
            {
                _fd.setWantClientAuth(true);
            }
            else
            {
                _fd.setNeedClientAuth(true);
            }
            _fd.setUseClientMode(false);

            String[] cipherSuites =
                _instance.filterCiphers(_fd.getSupportedCipherSuites(), _fd.getEnabledCipherSuites());
            try
            {
                _fd.setEnabledCipherSuites(cipherSuites);
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
                s.append("enabling SSL ciphersuites for server socket " + toString() + ":");
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
                    _fd.setEnabledProtocols(protocols);
                }
                catch(IllegalArgumentException ex)
                {
                    Ice.SecurityException e = new Ice.SecurityException();
                    e.reason = "IceSSL: invalid protocol";
                    e.initCause(ex);
                    throw e;
                }
            }
        }
        catch(java.io.IOException ex)
        {
            try
            {
                if(_fd != null)
                {
                    _fd.close();
                }
            }
            catch(java.io.IOException e)
            {
            }
            _fd = null;
            Ice.SocketException se = new Ice.SocketException();
            se.initCause(ex);
            throw se;
        }
    }

    protected synchronized void
    finalize()
        throws Throwable
    {
        IceUtilInternal.Assert.FinalizerAssert(_fd == null);

        super.finalize();
    }

    private Instance _instance;
    private TraceLevels _traceLevels;
    private String _adapterName;
    private String _host;
    private Ice.Logger _logger;
    private javax.net.ssl.SSLServerSocket _fd;
    private int _backlog;
    private java.net.InetSocketAddress _addr;
    private java.net.InetSocketAddress _connectToSelfAddr;
}
