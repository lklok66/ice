// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Diagnostics;
using System.Threading;
using Test;

public class AllTests
{
    private static void test(bool b)
    {
        if(!b)
        {
            throw new System.Exception();
        }
    }
    
    private class Callback
    {
        internal Callback()
        {
            _called = false;
        }
        
        public virtual bool check()
        {
            int cnt = 0;
            do
            {
                lock(this)
                {
                    if(_called)
                    {
                        return true;
                    }
                }
                Thread.Sleep(100);
            }
            while(++cnt < 50);

            return false; // Must be timeout
        }
        
        public virtual void called()
        {
            lock(this)
            {
                Debug.Assert(!_called);
                _called = true;
            }
        }
        
        private bool _called;
    }
    
    private class AMI_Thrower_throwAasAI
    {
        public AMI_Thrower_throwAasAI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(A ex)
            {
                AllTests.test(ex.aMem == 1);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwAasAObjectNotExistI
    {
        public AMI_Thrower_throwAasAObjectNotExistI(Ice.Communicator comm)
        {
            InitBlock(comm);
        }
        private void InitBlock(Ice.Communicator comm)
        {
            callback = new Callback();
            communicator = comm;
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.ObjectNotExistException ex)
            {
                Ice.Identity id = communicator.stringToIdentity("does not exist");
                AllTests.test(ex.id.Equals(id));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Ice.Communicator communicator;
        private Callback callback;
    }
    
    private class AMI_Thrower_throwAasAFacetNotExistI
    {
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.FacetNotExistException ex)
            {
                AllTests.test(ex.facet.Equals("no such facet"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback = new Callback();
    }
    
    private class AMI_Thrower_throwAorDasAorDI
    {
        public AMI_Thrower_throwAorDasAorDI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(A ex)
            {
                AllTests.test(ex.aMem == 1);
            }
            catch(D ex)
            {
                AllTests.test(ex.dMem == - 1);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwBasAI
    {
        public AMI_Thrower_throwBasAI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(B ex)
            {
                AllTests.test(ex.aMem == 1);
                AllTests.test(ex.bMem == 2);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwCasAI
    {
        public AMI_Thrower_throwCasAI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(C ex)
            {
                AllTests.test(ex.aMem == 1);
                AllTests.test(ex.bMem == 2);
                AllTests.test(ex.cMem == 3);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwBasBI
    {
        public AMI_Thrower_throwBasBI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(B ex)
            {
                AllTests.test(ex.aMem == 1);
                AllTests.test(ex.bMem == 2);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwCasBI
    {
        public AMI_Thrower_throwCasBI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(C ex)
            {
                AllTests.test(ex.aMem == 1);
                AllTests.test(ex.bMem == 2);
                AllTests.test(ex.cMem == 3);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwCasCI
    {
        public AMI_Thrower_throwCasCI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(C ex)
            {
                AllTests.test(ex.aMem == 1);
                AllTests.test(ex.bMem == 2);
                AllTests.test(ex.cMem == 3);
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwUndeclaredAI
    {
        public AMI_Thrower_throwUndeclaredAI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.UnknownUserException)
            {
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwUndeclaredBI
    {
        public AMI_Thrower_throwUndeclaredBI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.UnknownUserException)
            {
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwUndeclaredCI
    {
        public AMI_Thrower_throwUndeclaredCI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.UnknownUserException)
            {
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwLocalExceptionI
    {
        public AMI_Thrower_throwLocalExceptionI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.UnknownLocalException)
            {
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_Thrower_throwNonIceExceptionI
    {
        public AMI_Thrower_throwNonIceExceptionI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.UnknownException)
            {
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    private class AMI_WrongOperation_noSuchOperationI
    {
        public AMI_WrongOperation_noSuchOperationI()
        {
            InitBlock();
        }
        private void InitBlock()
        {
            callback = new Callback();
        }
        public void response()
        {
            AllTests.test(false);
        }
        
        public void exception(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Ice.OperationNotExistException ex)
            {
                AllTests.test(ex.operation.Equals("noSuchOperation"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }
        
        public virtual bool check()
        {
            return callback.check();
        }
        
        private Callback callback;
    }
    
    public static ThrowerPrx allTests(Ice.Communicator communicator, bool collocated)
    {
        {
            Console.Write("testing object factory registration exception... ");
            Ice.ObjectFactory of = new ObjectFactoryI();
            communicator.addObjectFactory(of, "::x");
            try
            {
                communicator.addObjectFactory(of, "::x");
                test(false);
            }
            catch(Ice.AlreadyRegisteredException)
            {
            }
            Console.WriteLine("ok");
        }
        
        Console.Write("testing stringToProxy... ");
        Console.Out.Flush();
        String @ref = "thrower:default -p 12010 -t 2000";
        Ice.ObjectPrx @base = communicator.stringToProxy(@ref);
        test(@base != null);
        Console.WriteLine("ok");
        
        Console.Write("testing checked cast... ");
        Console.Out.Flush();
        ThrowerPrx thrower = ThrowerPrxHelper.checkedCast(@base);
        test(thrower != null);
        test(thrower.Equals(@base));
        Console.WriteLine("ok");
        
        Console.Write("catching exact types... ");
        Console.Out.Flush();
        
        try
        {
            thrower.throwAasA(1);
            test(false);
        }
        catch(A ex)
        {
            test(ex.aMem == 1);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwAorDasAorD(1);
            test(false);
        }
        catch(A ex)
        {
            test(ex.aMem == 1);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwAorDasAorD(- 1);
            test(false);
        }
        catch(D ex)
        {
            test(ex.dMem == - 1);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwBasB(1, 2);
            test(false);
        }
        catch(B ex)
        {
            test(ex.aMem == 1);
            test(ex.bMem == 2);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwCasC(1, 2, 3);
            test(false);
        }
        catch(C ex)
        {
            test(ex.aMem == 1);
            test(ex.bMem == 2);
            test(ex.cMem == 3);
        }
        catch(Exception)
        {
            test(false);
        }
        
        Console.WriteLine("ok");
        
        Console.Write("catching base types... ");
        Console.Out.Flush();
        
        try
        {
            thrower.throwBasB(1, 2);
            test(false);
        }
        catch(A ex)
        {
            test(ex.aMem == 1);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwCasC(1, 2, 3);
            test(false);
        }
        catch(B ex)
        {
            test(ex.aMem == 1);
            test(ex.bMem == 2);
        }
        catch(Exception)
        {
            test(false);
        }
        
        Console.WriteLine("ok");
        
        Console.Write("catching derived types... ");
        Console.Out.Flush();
        
        try
        {
            thrower.throwBasA(1, 2);
            test(false);
        }
        catch(B ex)
        {
            test(ex.aMem == 1);
            test(ex.bMem == 2);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwCasA(1, 2, 3);
            test(false);
        }
        catch(C ex)
        {
            test(ex.aMem == 1);
            test(ex.bMem == 2);
            test(ex.cMem == 3);
        }
        catch(Exception)
        {
            test(false);
        }
        
        try
        {
            thrower.throwCasB(1, 2, 3);
            test(false);
        }
        catch(C ex)
        {
            test(ex.aMem == 1);
            test(ex.bMem == 2);
            test(ex.cMem == 3);
        }
        catch(Exception)
        {
            test(false);
        }
        
        Console.WriteLine("ok");
        
        if(thrower.supportsUndeclaredExceptions())
        {
            Console.Write("catching unknown user exception... ");
            Console.Out.Flush();
            
            try
            {
                thrower.throwUndeclaredA(1);
                test(false);
            }
            catch(A ex)
            {
                //
                // We get the original exception with collocation
                // optimization.
                //
                test(collocated);
                test(ex.aMem == 1);
            }
            catch(Ice.UnknownUserException)
            {
                //
                // We get an unknown user exception without collocation
                // optimization.
                //
                test(!collocated);
            }
            catch(Exception)
            {
                test(false);
            }
            
            try
            {
                thrower.throwUndeclaredB(1, 2);
                test(false);
            }
            catch(B ex)
            {
                //
                // We get the original exception with collocation
                // optimization.
                //
                test(collocated);
                test(ex.aMem == 1);
                test(ex.bMem == 2);
            }
            catch(Ice.UnknownUserException)
            {
                //
                // We get an unknown user exception without collocation
                // optimization.
                //
                test(!collocated);
            }
            catch(Exception)
            {
                test(false);
            }
            
            try
            {
                thrower.throwUndeclaredC(1, 2, 3);
                test(false);
            }
            catch(C ex)
            {
                //
                // We get the original exception with collocation
                // optimization.
                //
                test(collocated);
                test(ex.aMem == 1);
                test(ex.bMem == 2);
                test(ex.cMem == 3);
            }
            catch(Ice.UnknownUserException)
            {
                //
                // We get an unknown user exception without collocation
                // optimization.
                //
                test(!collocated);
            }
            catch(Exception)
            {
                test(false);
            }
            
            Console.WriteLine("ok");
        }
        
        Console.Write("catching object not exist exception... ");
        Console.Out.Flush();
        
        {
            Ice.Identity id = communicator.stringToIdentity("does not exist");
            try
            {
                ThrowerPrx thrower2 = ThrowerPrxHelper.uncheckedCast(thrower.ice_identity(id));
                thrower2.ice_ping();
                test(false);
            }
            catch(Ice.ObjectNotExistException ex)
            {
                test(ex.id.Equals(id));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        
        Console.WriteLine("ok");
        
        Console.Write("catching facet not exist exception... ");
        Console.Out.Flush();
        
        try
        {
            ThrowerPrx thrower2 = ThrowerPrxHelper.uncheckedCast(thrower, "no such facet");
            try
            {
                thrower2.ice_ping();
                test(false);
            }
            catch(Ice.FacetNotExistException ex)
            {
                test(ex.facet.Equals("no such facet"));
            }
        }
        catch(Exception)
        {
            test(false);
        }
        
        Console.WriteLine("ok");
        
        Console.Write("catching operation not exist exception... ");
        Console.Out.Flush();
        
        try
        {
            WrongOperationPrx thrower2 = WrongOperationPrxHelper.uncheckedCast(thrower);
            thrower2.noSuchOperation();
            test(false);
        }
        catch(Ice.OperationNotExistException ex)
        {
            test(ex.operation.Equals("noSuchOperation"));
        }
        catch(Exception)
        {
            test(false);
        }
        
        Console.WriteLine("ok");

        Console.Write("catching unknown local exception... ");
        Console.Out.Flush();
        
        try
        {
            thrower.throwLocalException();
            test(false);
        }
        catch(Ice.TimeoutException)
        {
            //
            // We get the original exception with collocation
            // optimization.
            //
            test(collocated);
        }
        catch(Ice.UnknownLocalException)
        {
            //
            // We get the an unknown local exception without
            // collocation optimization.
            //
            test(!collocated);
        }
        catch(Exception)
        {
            test(false);
        }
        
        Console.WriteLine("ok");

        Console.Write("catching unknown non-Ice exception... ");
        Console.Out.Flush();
        
        try
        {
            thrower.throwNonIceException();
            test(false);
        }
        catch(Ice.UnknownException)
        {
            //
            // We get the unknown exception without collocation
            // optimization.
            //
            test(!collocated);
        }
        catch(System.Exception)
        {
            //
            // We get the original exception with collocation
            // optimization.
            //
            test(collocated);
        }
        
        Console.WriteLine("ok");
        
        if(!collocated)
        {
            Console.Write("catching exact types with AMI... ");
            Console.Out.Flush();
            
            {
                AMI_Thrower_throwAasAI cb = new AMI_Thrower_throwAasAI();
                thrower.throwAasA_async(cb.response, cb.exception, 1);
                test(cb.check());
            }
            
            {
                AMI_Thrower_throwAorDasAorDI cb = new AMI_Thrower_throwAorDasAorDI();
                thrower.throwAorDasAorD_async(cb.response, cb.exception, 1);
                test(cb.check());
            }
            
            {
                AMI_Thrower_throwAorDasAorDI cb = new AMI_Thrower_throwAorDasAorDI();
                thrower.throwAorDasAorD_async(cb.response, cb.exception, - 1);
                test(cb.check());
            }
            
            {
                AMI_Thrower_throwBasBI cb = new AMI_Thrower_throwBasBI();
                thrower.throwBasB_async(cb.response, cb.exception, 1, 2);
                test(cb.check());
            }
            
            {
                AMI_Thrower_throwCasCI cb = new AMI_Thrower_throwCasCI();
                thrower.throwCasC_async(cb.response, cb.exception, 1, 2, 3);
                test(cb.check());
            }
            
            Console.WriteLine("ok");
            
            Console.Write("catching derived types... ");
            Console.Out.Flush();
            
            {
                AMI_Thrower_throwBasAI cb = new AMI_Thrower_throwBasAI();
                thrower.throwBasA_async(cb.response, cb.exception, 1, 2);
                test(cb.check());
            }
            
            {
                AMI_Thrower_throwCasAI cb = new AMI_Thrower_throwCasAI();
                thrower.throwCasA_async(cb.response, cb.exception, 1, 2, 3);
                test(cb.check());
            }
            
            {
                AMI_Thrower_throwCasBI cb = new AMI_Thrower_throwCasBI();
                thrower.throwCasB_async(cb.response, cb.exception, 1, 2, 3);
                test(cb.check());
            }
            
            Console.WriteLine("ok");
            
            if(thrower.supportsUndeclaredExceptions())
            {
                Console.Write("catching unknown user exception with AMI... ");
                Console.Out.Flush();
                
                {
                    AMI_Thrower_throwUndeclaredAI cb = new AMI_Thrower_throwUndeclaredAI();
                    thrower.throwUndeclaredA_async(cb.response, cb.exception, 1);
                    test(cb.check());
                }
                
                {
                    AMI_Thrower_throwUndeclaredBI cb = new AMI_Thrower_throwUndeclaredBI();
                    thrower.throwUndeclaredB_async(cb.response, cb.exception, 1, 2);
                    test(cb.check());
                }
                
                {
                    AMI_Thrower_throwUndeclaredCI cb = new AMI_Thrower_throwUndeclaredCI();
                    thrower.throwUndeclaredC_async(cb.response, cb.exception, 1, 2, 3);
                    test(cb.check());
                }
                
                Console.WriteLine("ok");
            }
            
            Console.Write("catching object not exist exception with AMI... ");
            Console.Out.Flush();
            
            {
                Ice.Identity id = communicator.stringToIdentity("does not exist");
                ThrowerPrx thrower2 = ThrowerPrxHelper.uncheckedCast(thrower.ice_identity(id));
                AMI_Thrower_throwAasAObjectNotExistI cb = new AMI_Thrower_throwAasAObjectNotExistI(communicator);
                thrower2.throwAasA_async(cb.response, cb.exception, 1);
                test(cb.check());
            }
            
            Console.WriteLine("ok");
            
            Console.Write("catching facet not exist exception with AMI... ");
            Console.Out.Flush();
            
            try
            {
                ThrowerPrx thrower2 = ThrowerPrxHelper.uncheckedCast(thrower, "no such facet");
                {
                    AMI_Thrower_throwAasAFacetNotExistI cb = new AMI_Thrower_throwAasAFacetNotExistI();
                    thrower2.throwAasA_async(cb.response, cb.exception, 1);
                    test(cb.check());
                }
            }
            catch(Exception)
            {
                test(false);
            }
            
            Console.WriteLine("ok");
            
            Console.Write("catching operation not exist exception with AMI... ");
            Console.Out.Flush();
            
            {
                AMI_WrongOperation_noSuchOperationI cb = new AMI_WrongOperation_noSuchOperationI();
                WrongOperationPrx thrower2 = WrongOperationPrxHelper.uncheckedCast(thrower);
                thrower2.noSuchOperation_async(cb.response, cb.exception);
                test(cb.check());
            }
            
            Console.WriteLine("ok");
            
            Console.Write("catching unknown local exception with AMI... ");
            Console.Out.Flush();
            
            {
                AMI_Thrower_throwLocalExceptionI cb = new AMI_Thrower_throwLocalExceptionI();
                thrower.throwLocalException_async(cb.response, cb.exception);
                test(cb.check());
            }
            
            Console.WriteLine("ok");
            
            Console.Write("catching unknown non-Ice exception with AMI... ");
            Console.Out.Flush();
            
            AMI_Thrower_throwNonIceExceptionI cb2 = new AMI_Thrower_throwNonIceExceptionI();
            thrower.throwNonIceException_async(cb2.response, cb2.exception);
            test(cb2.check());
            
            Console.WriteLine("ok");
        }
        
        return thrower;
    }
}
