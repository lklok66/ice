// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Complex;

public class ObjectFactoryI extends Ice.LocalObjectImpl implements Ice.ObjectFactory
{
    public Ice.Object
    create(String type)
    {
	if(type.equals("::Complex::MultiplyNode"))
	{
	    return new MultiplyNodeI();
	}
	if(type.equals("::Complex::AddNode"))
	{
	    return new AddNodeI();
	}
        if(type.equals("::Complex::NumberNode"))
	{
	    return new NumberNodeI();
	}

	System.err.println( "create: " + type);
	assert(false);
	return null;
    }

    public void
    destroy()
    {
	// Nothing to do
    }
}
