// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.exceptions;

import test.Ice.exceptionsAMD.Test.A;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_shutdown;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_supportsAssertException;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_supportsUndeclaredExceptions;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwAasA;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwAorDasAorD;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwAssertException;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwBasA;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwBasB;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwCasA;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwCasB;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwCasC;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwLocalException;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwNonIceException;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwUndeclaredA;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwUndeclaredB;
import test.Ice.exceptionsAMD.Test.AMD_Thrower_throwUndeclaredC;
import test.Ice.exceptionsAMD.Test.B;
import test.Ice.exceptionsAMD.Test.C;
import test.Ice.exceptionsAMD.Test.D;
import test.Ice.exceptionsAMD.Test._ThrowerDisp;

public final class AMDThrowerI extends _ThrowerDisp
{
    public
    AMDThrowerI(Ice.ObjectAdapter adapter)
    {
        _adapter = adapter;
    }

    public void
    shutdown_async(AMD_Thrower_shutdown cb, Ice.Current current)
    {
        _adapter.getCommunicator().shutdown();
        cb.ice_response();
    }

    public void
    supportsUndeclaredExceptions_async(AMD_Thrower_supportsUndeclaredExceptions cb, Ice.Current current)
    {
        cb.ice_response(true);
    }

    public void
    supportsAssertException_async(AMD_Thrower_supportsAssertException cb, Ice.Current current)
    {
        cb.ice_response(true);
    }

    public void
    throwAasA_async(AMD_Thrower_throwAasA cb, int a, Ice.Current current)
        throws A
    {
        A ex = new A();
        ex.aMem = a;
        cb.ice_exception(ex);
    }

    public void
    throwAorDasAorD_async(AMD_Thrower_throwAorDasAorD cb, int a, Ice.Current current)
        throws A,
               D
    {
        if(a > 0)
        {
            A ex = new A();
            ex.aMem = a;
            cb.ice_exception(ex);
        }
        else
        {
            D ex = new D();
            ex.dMem = a;
            cb.ice_exception(ex);
        }
    }

    public void
    throwBasA_async(AMD_Thrower_throwBasA cb, int a, int b, Ice.Current current)
        throws A
    {
        B ex = new B();
        ex.aMem = a;
        ex.bMem = b;
        throw ex;
        //cb.ice_exception(ex);
    }

    public void
    throwBasB_async(AMD_Thrower_throwBasB cb, int a, int b, Ice.Current current)
        throws B
    {
        B ex = new B();
        ex.aMem = a;
        ex.bMem = b;
        throw ex;
        //cb.ice_exception(ex);
    }

    public void
    throwCasA_async(AMD_Thrower_throwCasA cb, int a, int b, int c, Ice.Current current)
        throws A
    {
        C ex = new C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    }

    public void
    throwCasB_async(AMD_Thrower_throwCasB cb, int a, int b, int c, Ice.Current current)
        throws B
    {
        C ex = new C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    }

    public void
    throwCasC_async(AMD_Thrower_throwCasC cb, int a, int b, int c, Ice.Current current)
        throws C
    {
        C ex = new C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    }

    public void
    throwUndeclaredA_async(AMD_Thrower_throwUndeclaredA cb, int a, Ice.Current current)
    {
        A ex = new A();
        ex.aMem = a;
        cb.ice_exception(ex);
    }

    public void
    throwUndeclaredB_async(AMD_Thrower_throwUndeclaredB cb, int a, int b, Ice.Current current)
    {
        B ex = new B();
        ex.aMem = a;
        ex.bMem = b;
        cb.ice_exception(ex);
    }

    public void
    throwUndeclaredC_async(AMD_Thrower_throwUndeclaredC cb, int a, int b, int c, Ice.Current current)
    {
        C ex = new C();
        ex.aMem = a;
        ex.bMem = b;
        ex.cMem = c;
        cb.ice_exception(ex);
    }

    public void
    throwLocalException_async(AMD_Thrower_throwLocalException cb, Ice.Current current)
    {
        cb.ice_exception(new Ice.TimeoutException());
    }

    public void
    throwNonIceException_async(AMD_Thrower_throwNonIceException cb, Ice.Current current)
    {
        throw new RuntimeException();
    }

    public void
    throwAssertException_async(AMD_Thrower_throwAssertException cb, Ice.Current current)
    {
        throw new java.lang.AssertionError();
    }

    private Ice.ObjectAdapter _adapter;
}
