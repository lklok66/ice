// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceSSL;

final class EndpointFactoryI implements IceInternal.EndpointFactory
{
    EndpointFactoryI(Instance instance)
    {
	_instance = instance;
    }

    public short
    type()
    {
	return EndpointI.TYPE;
    }

    public String
    protocol()
    {
	return "ssl";
    }

    public IceInternal.EndpointI
    create(String str)
    {
	return new EndpointI(_instance, str);
    }

    public IceInternal.EndpointI
    read(IceInternal.BasicStream s)
    {
	return new EndpointI(_instance, s);
    }

    public void
    destroy()
    {
	_instance = null;
    }

    private Instance _instance;
}
