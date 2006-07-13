// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System.Diagnostics;

class ObjectFactory : Ice.LocalObjectImpl, Ice.ObjectFactory
{
    public virtual Ice.Object create(string type)
    {
        if(type.Equals("::Demo::Printer"))
        {
            return new PrinterI();
        }
        
        if(type.Equals("::Demo::DerivedPrinter"))
        {
            return new DerivedPrinterI();
        }
        
        Debug.Assert(false);
        return null;
    }
    
    public virtual void  destroy()
    {
        // Nothing to do
    }
}
