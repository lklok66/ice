// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceSSL;

final class SslConnector implements IceInternal.Connector
{
    private static class ConnectThread extends Thread
    {
	ConnectThread(javax.net.ssl.SSLContext ctx, java.net.InetSocketAddress addr)
	{
	    _ctx = ctx;
	    _addr = addr;
	}

	public void
	run()
	{
	    try
	    {
		javax.net.SocketFactory factory = _ctx.getSocketFactory();
		javax.net.ssl.SSLSocket fd =
		    (javax.net.ssl.SSLSocket)factory.createSocket(_addr.getAddress(), _addr.getPort());
		synchronized(this)
		{
		    _fd = fd;
		    notifyAll();
		}
	    }
	    catch(java.io.IOException ex)
	    {
		synchronized(this)
		{
		    _ex = ex;
		    notifyAll();
		}
	    }
	}

	javax.net.ssl.SSLSocket
	getFd(int timeout)
	    throws java.io.IOException
	{
	    javax.net.ssl.SSLSocket fd = null;

	    synchronized(this)
	    {
		while(_fd == null && _ex == null)
		{
		    try
		    {
			wait(timeout);
			break;
		    }
		    catch(InterruptedException ex)
		    {
			continue;
		    }
		}

		if(_ex != null)
		{
		    throw _ex;
		}

		fd = _fd;
		_fd = null;
	    }

	    return fd;
	}

	private javax.net.ssl.SSLContext _ctx;
	private java.net.InetSocketAddress _addr;
	private javax.net.ssl.SSLSocket _fd;
	private java.io.IOException _ex;
    }

    private static class HandshakeThread extends Thread
    {
	HandshakeThread(javax.net.ssl.SSLSocket fd)
	{
	    _fd = fd;
	    _ok = false;
	}

	public void
	run()
	{
	    try
	    {
		_fd.startHandshake();
		synchronized(this)
		{
		    _ok = true;
		    notifyAll();
		}

	    }
	    catch(java.io.IOException ex)
	    {
		synchronized(this)
		{
		    _ex = ex;
		    notifyAll();
		}
	    }
	}

	boolean
	waitForHandshake(int timeout)
	    throws java.io.IOException
	{
	    boolean result = false;

	    synchronized(this)
	    {
		while(!_ok && _ex == null)
		{
		    try
		    {
			wait(timeout);
			break;
		    }
		    catch(InterruptedException ex)
		    {
			continue;
		    }
		}

		if(_ex != null)
		{
		    throw _ex;
		}

		result = _ok;
	    }

	    return result;
	}

	private javax.net.ssl.SSLSocket _fd;
	private boolean _ok;
	private java.io.IOException _ex;
    }

    public IceInternal.Transceiver
    connect(int timeout)
    {
	if(_instance.networkTraceLevel() >= 2)
	{
	    String s = "trying to establish ssl connection to " + toString();
	    _logger.trace(_instance.networkTraceCategory(), s);
	}

	javax.net.ssl.SSLSocket fd = null;
	try
	{
	    //
	    // If a connect timeout is specified, do the connect in a separate thread.
	    //
	    if(timeout >= 0)
	    {
		ConnectThread ct = new ConnectThread(_ctx.sslContext(), _addr);
		ct.start();
		fd = ct.getFd(timeout == 0 ? 1 : timeout);
		if(fd == null)
		{
		    throw new Ice.ConnectTimeoutException();
		}
	    }
	    else
	    {
		javax.net.SocketFactory factory = _ctx.sslContext().getSocketFactory();
		fd = (javax.net.ssl.SSLSocket)factory.createSocket(_addr.getAddress(), _addr.getPort());
	    }

	    fd.setUseClientMode(true);

	    String[] cipherSuites = _ctx.filterCiphers(fd.getSupportedCipherSuites(), fd.getEnabledCipherSuites());
	    if(_instance.securityTraceLevel() > 0)
	    {
		StringBuffer s = new StringBuffer();
		s.append("enabling ciphersuites for ssl socket\n" + IceInternal.Network.fdToString(fd) + ":");
		for(int i = 0; i < cipherSuites.length; ++i)
		{
		    s.append("\n  " + cipherSuites[i]);
		}
		_logger.trace(_instance.securityTraceCategory(), s.toString());
	    }
	    fd.setEnabledCipherSuites(cipherSuites);

	    //
	    // If a connect timeout is specified, do the SSL handshake in a separate thread.
	    //
	    if(timeout >= 0)
	    {
		HandshakeThread ht = new HandshakeThread(fd);
		ht.start();
		if(!ht.waitForHandshake(timeout == 0 ? 1 : timeout))
		{
		    throw new Ice.ConnectTimeoutException();
		}
	    }
	    else
	    {
		fd.startHandshake();
	    }
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

	if(_instance.networkTraceLevel() >= 1)
	{
	    String s = "ssl connection established\n" + IceInternal.Network.fdToString(fd);
	    _logger.trace(_instance.networkTraceCategory(), s);
	}

	return new SslTransceiver(_instance, fd);
    }

    public String
    toString()
    {
	return IceInternal.Network.addrToString(_addr);
    }

    //
    // Only for use by SslEndpoint
    //
    SslConnector(Instance instance, String host, int port)
    {
	_instance = instance;
	_ctx = instance.clientContext();
	_logger = instance.communicator().getLogger();

	_addr = IceInternal.Network.getAddress(host, port);
    }

    private Instance _instance;
    private Context _ctx;
    private Ice.Logger _logger;
    private java.net.InetSocketAddress _addr;
}
