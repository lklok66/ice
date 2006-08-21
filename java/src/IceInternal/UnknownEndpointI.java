// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

final class UnknownEndpointI extends EndpointI
{
    public
    UnknownEndpointI(short type, BasicStream s)
    {
        _instance = s.instance();
        _type = type;
        s.startReadEncaps();
        int sz = s.getReadEncapsSize();
        _rawBytes = s.readBlob(sz);
        s.endReadEncaps();
        calcHashValue();
    }

    //
    // Marshal the endpoint
    //
    public void
    streamWrite(BasicStream s)
    {
        s.writeShort(_type);
        s.startWriteEncaps();
        s.writeBlob(_rawBytes);
        s.endWriteEncaps();
    }

    //
    // Convert the endpoint to its string form
    //
    public String
    _toString()
    {
        return "";
    }

    //
    // Return the endpoint type
    //
    public short
    type()
    {
        return _type;
    }

    //
    // Return the timeout for the endpoint in milliseconds. 0 means
    // non-blocking, -1 means no timeout.
    //
    public int
    timeout()
    {
        return -1;
    }
    
    //
    // Return a new endpoint with a different timeout value, provided
    // that timeouts are supported by the endpoint. Otherwise the same
    // endpoint is returned.
    //
    public EndpointI
    timeout(int t)
    {
        return this;
    }

    //
    // Return a new endpoint with a different connection id.
    //
    public EndpointI
    connectionId(String connectionId)
    {
        return this;
    }
    
    //
    // Return true if the endpoints support bzip2 compress, or false
    // otherwise.
    //
    public boolean
    compress()
    {
        return false;
    }

    //
    // Return a new endpoint with a different compression value,
    // provided that compression is supported by the
    // endpoint. Otherwise the same endpoint is returned.
    //
    public EndpointI
    compress(boolean compress)
    {
	return this;
    }

    //
    // Return true if the endpoint is datagram-based.
    //
    public boolean
    datagram()
    {
        return false;
    }
    
    //
    // Return true if the endpoint is secure.
    //
    public boolean
    secure()
    {
        return false;
    }

    //
    // Return true if the endpoint type is unknown.
    //
    public boolean
    unknown()
    {
        return true;
    }
    
    //
    // Return a client side transceiver for this endpoint, or null if a
    // transceiver can only be created by a connector.
    //
    public Transceiver
    clientTransceiver()
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
    public Transceiver
    serverTransceiver(EndpointIHolder endpoint)
    {
        endpoint.value = null;
        return null;
    }

    //
    // Return a connector for this endpoint, or null if no connector
    // is available.
    //
    public Connector
    connector()
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
    public Acceptor
    acceptor(EndpointIHolder endpoint, String adapterName)
    {
        endpoint.value = null;
        return null;
    }

    //
    // Expand endpoint out in to separate endpoints for each local
    // host if endpoint was configured with no host set. This
    // only applies for ObjectAdapter endpoints.
    //
    public java.util.ArrayList
    expand()
    {
        assert(false);
	return null;
    }

    //
    // Return whether endpoint should be published in proxies
    // created by Object Adapter.
    //
    public boolean
    publish()
    {
        return false;
    }

    //
    // Check whether the endpoint is equivalent to a specific
    // Transceiver or Acceptor
    //
    public boolean
    equivalent(Transceiver transceiver)
    {
        return false;
    }

    public boolean
    equivalent(Acceptor acceptor)
    {
        return false;
    }

    public int
    hashCode()
    {
        return _hashCode;
    }
    
    //
    // Compare endpoints for sorting purposes
    //
    public boolean
    equals(java.lang.Object obj)
    {
        return compareTo(obj) == 0;
    }

    public int
    compareTo(java.lang.Object obj) // From java.lang.Comparable
    {
        UnknownEndpointI p = null;

        try
        {
            p = (UnknownEndpointI)obj;
        }
        catch(ClassCastException ex)
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

        if(_rawBytes.length < p._rawBytes.length)
        {
            return -1;
        }
        else if(p._rawBytes.length < _rawBytes.length)
        {
            return 1;
        }
        for(int i = 0; i < _rawBytes.length; i++)
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

    private void
    calcHashValue()
    {
        _hashCode = _type;
        for(int i = 0; i < _rawBytes.length; i++)
        {
            _hashCode = 5 * _hashCode + _rawBytes[i];
        }
    }

    private Instance _instance;
    private short _type;
    private byte[] _rawBytes;
    private int _hashCode;
}
