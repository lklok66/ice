// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{

    using System.Diagnostics;
    using System.Collections;

    sealed class TcpEndpointI : EndpointI
    {
	internal const short TYPE = 1;
	
	public TcpEndpointI(Instance instance, string ho, int po, int ti, string conId, bool co, bool pub)
	{
	    instance_ = instance;
	    _host = ho;
	    _port = po;
	    _timeout = ti;
	    _connectionId = conId;
	    _compress = co;
	    _publish = pub;
	    calcHashValue();
	}
	
	public TcpEndpointI(Instance instance, string str)
	{
	    instance_ = instance;
	    _host = null;
	    _port = 0;
	    _timeout = -1;
	    _compress = false;
	    _publish = true;
	    
	    char[] separators = { ' ', '\t', '\n', '\r' };
	    string[] arr = str.Split(separators);
	    
	    int i = 0;
	    while(i < arr.Length)
	    {
		if(arr[i].Length == 0)
		{
		    i++;
		    continue;
		}
		
		string option = arr[i++];
		if(option.Length != 2 || option[0] != '-')
		{
		    Ice.EndpointParseException e = new Ice.EndpointParseException();
		    e.str = "tcp " + str;
		    throw e;
		}
		
		string argument = null;
		if(i < arr.Length && arr[i][0] != '-')
		{
		    argument = arr[i++];
		}
		
		switch(option[1])
		{
		    case 'h': 
		    {
			if(argument == null)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException();
			    e.str = "tcp " + str;
			    throw e;
			}
			
			_host = argument;
			break;
		    }
		    
		    case 'p': 
		    {
			if(argument == null)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException();
			    e.str = "tcp " + str;
			    throw e;
			}
			
			try
			{
			    _port = System.Int32.Parse(argument);
			}
			catch(System.FormatException ex)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException(ex);
			    e.str = "tcp " + str;
			    throw e;
			}

			if(_port < 0 || _port > 65535)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException();
			    e.str = "tcp " + str;
			    throw e;
			}
			
			break;
		    }
		    
		    case 't': 
		    {
			if(argument == null)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException();
			    e.str = "tcp " + str;
			    throw e;
			}
			
			try
			{
			    _timeout = System.Int32.Parse(argument);
			}
			catch(System.FormatException ex)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException(ex);
			    e.str = "tcp " + str;
			    throw e;
			}
			
			break;
		    }
		    
		    case 'z': 
		    {
			if(argument != null)
			{
			    Ice.EndpointParseException e = new Ice.EndpointParseException();
			    e.str = "tcp " + str;
			    throw e;
			}
			
			_compress = true;
			break;
		    }
		    
		    default: 
		    {
			Ice.EndpointParseException e = new Ice.EndpointParseException();
			e.str = "tcp " + str;
			throw e;
		    }
		}
	    }
	    
	    if(_host == null)
	    {
		_host = instance_.defaultsAndOverrides().defaultHost;
		if(_host == null)
		{
		    _host = "0.0.0.0";
		}
	    }
	    else if(_host.Equals("*"))
	    {
	        _host = "0.0.0.0";
	    }
	    
	    calcHashValue();
	}
	
	public TcpEndpointI(BasicStream s)
	{
	    instance_ = s.instance();
	    s.startReadEncaps();
	    _host = s.readString();
	    _port = s.readInt();
	    _timeout = s.readInt();
	    _compress = s.readBool();
	    s.endReadEncaps();
	    _publish = true;
	    calcHashValue();
	}
	
	//
	// Marshal the endpoint
	//
	public override void streamWrite(BasicStream s)
	{
	    s.writeShort(TYPE);
	    s.startWriteEncaps();
	    s.writeString(_host);
	    s.writeInt(_port);
	    s.writeInt(_timeout);
	    s.writeBool(_compress);
	    s.endWriteEncaps();
	}
	
	//
	// Convert the endpoint to its string form
	//
	public override string ice_toString_()
	{
	    //
	    // WARNING: Certain features, such as proxy validation in Glacier2,
	    // depend on the format of proxy strings. Changes to toString() and
	    // methods called to generate parts of the reference string could break
	    // these features. Please review for all features that depend on the
	    // format of proxyToString() before changing this and related code.
	    //
	    string s = "tcp -h " + _host + " -p " + _port;
	    if(_timeout != -1)
	    {
		s += " -t " + _timeout;
	    }
	    if(_compress)
	    {
		s += " -z";
	    }
	    return s;
	}
	
	//
	// Return the endpoint type
	//
	public override short type()
	{
	    return TYPE;
	}
	
	//
	// Return the timeout for the endpoint in milliseconds. 0 means
	// non-blocking, -1 means no timeout.
	//
	public override int timeout()
	{
	    return _timeout;
	}
	
	//
	// Return a new endpoint with a different timeout value, provided
	// that timeouts are supported by the endpoint. Otherwise the same
	// endpoint is returned.
	//
	public override EndpointI timeout(int timeout)
	{
	    if(timeout == _timeout)
	    {
		return this;
	    }
	    else
	    {
		return new TcpEndpointI(instance_, _host, _port, timeout, _connectionId, _compress, _publish);
	    }
	}

	//
	// Return a new endpoint with a different connection id.
	//
	public override EndpointI connectionId(string connectionId)
	{
	    if(connectionId == _connectionId)
	    {
		return this;
	    }
	    else
	    {
		return new TcpEndpointI(instance_, _host, _port, _timeout, connectionId, _compress, _publish);
	    }
	}
	
	//
	// Return true if the endpoints support bzip2 compress, or false
	// otherwise.
	//
	public override bool compress()
	{
	    return _compress;
	}
	
	//
	// Return a new endpoint with a different compression value,
	// provided that compression is supported by the
	// endpoint. Otherwise the same endpoint is returned.
	//
	public override EndpointI compress(bool compress)
	{
	    if(compress == _compress)
	    {
		return this;
	    }
	    else
	    {
		return new TcpEndpointI(instance_, _host, _port, _timeout, _connectionId, compress, _publish);
	    }
	}
	
	//
	// Return true if the endpoint is datagram-based.
	//
	public override bool datagram()
	{
	    return false;
	}
	
	//
	// Return true if the endpoint is secure.
	//
	public override bool secure()
	{
	    return false;
	}
	
	//
	// Return true if the endpoint type is unknown.
	//
	public override bool unknown()
	{
	    return false;
	}
	
	//
	// Return a client side transceiver for this endpoint, or null if a
	// transceiver can only be created by a connector.
	//
	public override Transceiver clientTransceiver()
	{
	    return null;
	}
	
	//
	// Return a server side transceiver for this endpoint, or null if a
	// transceiver can only be created by an acceptor. In case a
	// transceiver is created, this operation also returns a new
	// "effective" endpoint, which might differ from this endpoint,
	// for example, if a dynamic port number is assigned.
	//
	public override Transceiver serverTransceiver(ref EndpointI endpoint)
	{
	    endpoint = this;
	    return null;
	}
	
	//
	// Return a connector for this endpoint, or null if no connector
	// is available.
	//
	public override Connector connector()
	{
	    return new TcpConnector(instance_, _host, _port);
	}
	
	//
	// Return an acceptor for this endpoint, or null if no acceptors
	// is available. In case an acceptor is created, this operation
	// also returns a new "effective" endpoint, which might differ
	// from this endpoint, for example, if a dynamic port number is
	// assigned.
	//
	public override Acceptor acceptor(ref EndpointI endpoint, string adapterName)
	{
	    TcpAcceptor p = new TcpAcceptor(instance_, _host, _port);
	    endpoint = new TcpEndpointI(instance_, _host, p.effectivePort(), _timeout, _connectionId,
					_compress, _publish);
	    return p;
	}

	//
	// Expand endpoint out in to separate endpoints for each local
	// host if endpoint was configured with no host set. This
	// only applies for ObjectAdapter endpoints.
	//
	public override ArrayList
	expand()
	{
	    ArrayList endps = new ArrayList();
	    if(_host.Equals("0.0.0.0"))
	    {
	        string[] hosts = Network.getLocalHosts();
	        for(int i = 0; i < hosts.Length; ++i)
	        {
	            endps.Add(new TcpEndpointI(instance_, hosts[i], _port, _timeout, _connectionId, _compress,
	                                       hosts.Length == 1 || !hosts[i].Equals("127.0.0.1")));
	        }
	    }
	    else
	    {
	        endps.Add(this);
	    }
	    return endps;
	}
	
	//
	// Return whether endpoint should be published in proxies
	// created by Object Adapter.
	//
	public override bool
	publish()
	{
	    return _publish;
	}
	
	//
	// Check whether the endpoint is equivalent to a specific
	// Transceiver or Acceptor
	//
	public override bool equivalent(Transceiver transceiver)
	{
	    return false;
	}
	
	public override bool equivalent(Acceptor acceptor)
	{
	    TcpAcceptor tcpAcceptor = null;
	    try
	    {
		tcpAcceptor = (TcpAcceptor)acceptor;
	    }
	    catch(System.InvalidCastException)
	    {
		return false;
	    }
	    return tcpAcceptor.equivalent(_host, _port);
	}
	
	public override int GetHashCode()
	{
	    return _hashCode;
	}
	
	//
	// Compare endpoints for sorting purposes
	//
	public override bool Equals(object obj)
	{
	    return CompareTo(obj) == 0;
	}
	
	public override int CompareTo(object obj)
	{
	    TcpEndpointI p = null;
	    
	    try
	    {
		p = (TcpEndpointI)obj;
	    }
	    catch(System.InvalidCastException)
	    {
		return 1;
	    }
	    
	    if(this == p)
	    {
		return 0;
	    }
	    
	    if(_port < p._port)
	    {
		return -1;
	    }
	    else if(p._port < _port)
	    {
		return 1;
	    }
	    
	    if(_timeout < p._timeout)
	    {
		return -1;
	    }
	    else if(p._timeout < _timeout)
	    {
		return 1;
	    }

	    if(!_connectionId.Equals(p._connectionId))
	    {
		return _connectionId.CompareTo(p._connectionId);
	    }
	    
	    if(!_compress && p._compress)
	    {
		return -1;
	    }
	    else if(!p._compress && _compress)
	    {
		return 1;
	    }
	    
	    if(!_host.Equals(p._host))
	    {
		//
		// We do the most time-consuming part of the comparison last.
		//
		System.Net.IPEndPoint laddr = null;
		try
		{
		    laddr = Network.getAddress(_host, _port);
		}
		catch(Ice.DNSException)
		{
		}
		
		System.Net.IPEndPoint raddr = null;
		try
		{
		    raddr = Network.getAddress(p._host, p._port);
		}
		catch(Ice.DNSException)
		{
		}
		
		if(laddr == null && raddr != null)
		{
		    return -1;
		}
		else if(raddr == null && laddr != null)
		{
		    return 1;
		}
		else if(laddr != null && raddr != null)
		{
		    byte[] larr = laddr.Address.GetAddressBytes();
		    byte[] rarr = raddr.Address.GetAddressBytes();
		    Debug.Assert(larr.Length == rarr.Length);
		    for(int i = 0; i < larr.Length; i++)
		    {
			if(larr[i] < rarr[i])
			{
			    return -1;
			}
			else if(rarr[i] < larr[i])
			{
			    return 1;
			}
		    }
		}
	    }
	    
	    return 0;
	}
	
	private void calcHashValue()
	{
	    try
	    {
	        _hashCode = Network.getNumericHost(_host).GetHashCode();
	    }
	    catch(Ice.DNSException)
	    {
	        _hashCode = _host.GetHashCode();
	    }
	    _hashCode = 5 * _hashCode + _port;
	    _hashCode = 5 * _hashCode + _timeout;
	    _hashCode = 5 * _hashCode + _connectionId.GetHashCode();
	    _hashCode = 5 * _hashCode + (_compress? 1 : 0);
	}
	
	private Instance instance_;
	private string _host;
	private int _port;
	private int _timeout;
	private string _connectionId = "";
	private bool _compress;
	private bool _publish;
	private int _hashCode;
    }

    sealed class TcpEndpointFactory : EndpointFactory
    {
        internal TcpEndpointFactory(Instance instance)
        {
            instance_ = instance;
        }
	
        public short type()
        {
            return TcpEndpointI.TYPE;
        }
	
        public string protocol()
        {
            return "tcp";
        }
	
        public EndpointI create(string str)
        {
            return new TcpEndpointI(instance_, str);
        }
	
        public EndpointI read(BasicStream s)
        {
            return new TcpEndpointI(s);
        }
	
        public void destroy()
        {
            instance_ = null;
        }
	
        private Instance instance_;
    }

}
