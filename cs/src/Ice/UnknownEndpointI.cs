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

    using System.Collections;

    sealed class UnknownEndpointI : EndpointI
    {
	public UnknownEndpointI(short type, BasicStream s)
	{
	    _type = type;
	    s.startReadEncaps();
	    int sz = s.getReadEncapsSize();
	    _rawBytes = new byte[sz];
	    s.readBlob(_rawBytes);
	    s.endReadEncaps();
	    calcHashValue();
	}
	
	//
	// Marshal the endpoint
	//
	public override void streamWrite(BasicStream s)
	{
	    s.writeShort(_type);
	    s.startWriteEncaps();
	    s.writeBlob(_rawBytes);
	    s.endWriteEncaps();
	}
	
	//
	// Convert the endpoint to its string form
	//
	public override string ice_toString_()
	{
	    return "";
	}
	
	//
	// Return the endpoint type
	//
	public override short type()
	{
	    return _type;
	}
	
	//
	// Return the timeout for the endpoint in milliseconds. 0 means
	// non-blocking, -1 means no timeout.
	//
	public override int timeout()
	{
	    return -1;
	}
	
	//
	// Return a new endpoint with a different timeout value, provided
	// that timeouts are supported by the endpoint. Otherwise the same
	// endpoint is returned.
	//
	public override EndpointI timeout(int t)
	{
	    return this;
	}

	//
	// Return a new endpoint with a different connection id.
	//
	public override EndpointI connectionId(string id)
	{
	    return this;
	}
	
	//
	// Return true if the endpoints support bzip2 compress, or false
	// otherwise.
	//
	public override bool compress()
	{
	    return false;
	}
	
	//
	// Return a new endpoint with a different compression value,
	// provided that compression is supported by the
	// endpoint. Otherwise the same endpoint is returned.
	//
	public override EndpointI compress(bool compress)
	{
	    return this;
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
	    return true;
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
	    endpoint = null;
	    return null;
	}
	
	//
	// Return a connector for this endpoint, or null if no connector
	// is available.
	//
	public override Connector connector()
	{
	    return null;
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
	    endpoint = null;
	    return null;
	}

        //
        // Expand endpoint out in to separate endpoints for each local
        // host if endpoint was configured with no host set. This
        // only applies for ObjectAdapter endpoints.
        //
        public override ArrayList
        expand(bool includeLoopback)
        {
            return null;
        }

        //
        // Return whether endpoint should be published in proxies
        // created by Object Adapter.
        //
        public override bool
        publish()
        {
            return false;
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
	    return false;
	}
	
	public override int GetHashCode()
	{
	    return _hashCode;
	}
	
	//
	// Compare endpoints for sorting purposes
	//
	public override bool Equals(System.Object obj)
	{
	    return CompareTo(obj) == 0;
	}
	
	public override int CompareTo(System.Object obj)
	{
	    UnknownEndpointI p = null;
	    
	    try
	    {
		p = (UnknownEndpointI) obj;
	    }
	    catch(System.InvalidCastException)
	    {
		return 1;
	    }
	    
	    if(this == p)
	    {
		return 0;
	    }
	    
	    if(_type < p._type)
	    {
		return -1;
	    }
	    else if(p._type < _type)
	    {
		return 1;
	    }
	    
	    if(_rawBytes.Length < p._rawBytes.Length)
	    {
		return -1;
	    }
	    else if(p._rawBytes.Length < _rawBytes.Length)
	    {
		return 1;
	    }
	    for(int i = 0; i < _rawBytes.Length; i++)
	    {
		if(_rawBytes[i] < p._rawBytes[i])
		{
		    return -1;
		}
		else if(p._rawBytes[i] < _rawBytes[i])
		{
		    return 1;
		}
	    }
	    
	    return 0;
	}
	
	private void calcHashValue()
	{
	    _hashCode = _type;
	    for(int i = 0; i < _rawBytes.Length; i++)
	    {
		_hashCode = 5 * _hashCode + _rawBytes[i];
	    }
	}
	
	private short _type;
	private byte[] _rawBytes;
	private int _hashCode;
    }

}
