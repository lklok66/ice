// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

public final class MyDerivedClassI extends Test.MyDerivedClass
{
    public
    MyDerivedClassI()
    {
    }

    public void
    shutdown(Ice.Current c)
    {
        c.adapter.getCommunicator().shutdown();
    }

    public java.util.Hashtable
    getContext(Ice.Current current)
    {
        return _ctx;
    }

    public boolean
    ice_isA(String s, Ice.Current current)
    {
        _ctx = current.ctx;
        return super.ice_isA(s, current);
    }

    private java.util.Hashtable _ctx;
}
