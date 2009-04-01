// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceSSL;

import IceInternal.TraceLevels;

final class ConnectorI implements IceInternal.Connector, java.lang.Comparable
{
    public IceInternal.Transceiver
    connect()
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

        if(_traceLevels.network >= 2)
        {
            String s = "trying to establish ssl connection to " + toString();
            _logger.trace(_traceLevels.networkCat, s);
        }

        return new TransceiverI(_instance, _addr);
    }


    public short
    type()
    {
        return EndpointI.TYPE;
    }

    public String
    toString()
    {
        return IceInternal.Network.addrToString(_addr);
    }

    public int
    hashCode()
    {
        return _hashCode;
    }

    //
    // Only for use by EndpointI.
    //
    ConnectorI(Instance instance, java.net.InetSocketAddress addr, int timeout, String connectionId)
    {
        _instance = instance;
        _traceLevels = instance.traceLevels();
        _logger = instance.communicator().getLogger();
        _host = addr.getHostName();
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
        ConnectorI p = null;

        try
        {
            p = (ConnectorI)obj;
        }
        catch(ClassCastException ex)
        {
            try
            {
                IceInternal.Connector c = (IceInternal.Connector)obj;
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

        return IceInternal.Network.compareAddress(_addr, p._addr);
    }

    protected synchronized void
    finalize()
        throws Throwable
    {
        super.finalize();
    }

    private Instance _instance;
    private TraceLevels _traceLevels;
    private Ice.Logger _logger;
    private String _host;
    private java.net.InetSocketAddress _addr;
    private int _timeout;
    private String _connectionId;
    private int _hashCode;
}
