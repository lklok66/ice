// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

final class TcpConnector implements Connector, java.lang.Comparable
{
    final static short TYPE = 1;

    public Transceiver
    connect(int timeout)
    {
        if(_traceLevels.network >= 2)
        {
            String s = "trying to establish tcp connection to " + toString();
            _logger.trace(_traceLevels.networkCat, s);
        }

        java.nio.channels.SocketChannel fd = Network.createTcpSocket();
        Network.setBlock(fd, false);
        Network.setTcpBufSize(fd, _instance.initializationData().properties, _logger);
        Network.doConnect(fd, _addr, timeout);

        if(_traceLevels.network >= 1)
        {
            String s = "tcp connection established\n" + Network.fdToString(fd);
            _logger.trace(_traceLevels.networkCat, s);
        }

        return new TcpTransceiver(_instance, fd);
    }

    public short
    type()
    {
        return TYPE;
    }

    public String
    toString()
    {
        return Network.addrToString(_addr);
    }

    public int
    hashCode()
    {
        return _hashCode;
    }

    //
    // Only for use by TcpEndpoint
    //
    TcpConnector(Instance instance, java.net.InetSocketAddress addr, int timeout, String connectionId)
    {
        _instance = instance;
        _traceLevels = instance.traceLevels();
        _logger = instance.initializationData().logger;
        _addr = addr;
        _timeout = timeout;
        _connectionId = connectionId;

        _hashCode = _addr.getAddress().getHostAddress().hashCode();
        _hashCode = 5 * _hashCode + _addr.getPort();
        _hashCode = 5 * _hashCode + _timeout;
        _hashCode = 5 * _hashCode + _connectionId.hashCode();
    }

    //
    // Compare connectors for sorting purposes
    //
    public boolean
    equals(java.lang.Object obj)
    {
        return compareTo(obj) == 0;
    }

    public int
    compareTo(java.lang.Object obj) // From java.lang.Comparable
    {
        TcpConnector p = null;

        try
        {
            p = (TcpConnector)obj;
        }
        catch(ClassCastException ex)
        {
            try
            {
                Connector c = (Connector)obj;
                return type() < c.type() ? -1 : 1;
            }
            catch(ClassCastException ee)
            {
                assert(false);
            }
        }

        if(this == p)
        {
            return 0;
        }

        if(_timeout < p._timeout)
        {
            return -1;
        }
        else if(p._timeout < _timeout)
        {
            return 1;
        }

        if(!_connectionId.equals(p._connectionId))
        {
            return _connectionId.compareTo(p._connectionId);
        }

        if(_timeout < p._timeout)
        {
            return -1;
        }
        else if(p._timeout < _timeout)
        {
            return 1;
        }

        return Network.compareAddress(_addr, p._addr);
    } 

    private Instance _instance;
    private TraceLevels _traceLevels;
    private Ice.Logger _logger;
    private java.net.InetSocketAddress _addr;
    private int _timeout;
    private String _connectionId = "";
    private int _hashCode;
}
