// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Diagnostics;
using System.Threading;
using IceInternal;
using Test;

public sealed class TestI : _TestIntfDisp
{
    public TestI(Ice.ObjectAdapter adapter)
    {
	lock(this)
	{
	    _adapter = adapter;
	    _p = Process.GetCurrentProcess();
	    _pid = _p.Id;
	}
    }
    
    private void commitSuicide()
    {
	if(AssemblyUtil._platform == AssemblyUtil.Platform.NonWindows
	   && AssemblyUtil._runtime == AssemblyUtil.Runtime.Mono)
	{
	    ProcessStartInfo info = new ProcessStartInfo("/usr/bin/kill");
	    info.CreateNoWindow = true;
	    info.Arguments = "-s 9 " + _pid;
	    Process.Start(info);
	}
	else
	{
	    _p.Kill();
	}
	Thread.Sleep(5000); // Give other threads time to die.
    }

    public override void abort(Ice.Current current)
    {
	commitSuicide();
    }
    
    public override void idempotentAbort(Ice.Current current)
    {
	commitSuicide();
    }
    
    public override void nonmutatingAbort(Ice.Current current)
    {
	commitSuicide();
    }
    
    public override int pid(Ice.Current current)
    {
	lock(this)
	{
	    return _pid;
	}
    }
    
    public override void shutdown(Ice.Current current)
    {
        _adapter.getCommunicator().shutdown();
    }
    
    private Ice.ObjectAdapter _adapter;
    private Process _p;
    private int _pid;
}
