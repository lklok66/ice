// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

public sealed class ObjectFactoryI : Ice.ObjectFactory
{
    public Ice.Object create(string type)
    {
        if(type.Equals("::Test::B"))
        {
            return new BI();
        }
        else if(type.Equals("::Test::C"))
        {
            return new CI();
        }
        else if(type.Equals("::Test::D"))
        {
            return new DI();
        }
        else if(type.Equals("::Test::E"))
        {
            return new EI();
        }
        else if(type.Equals("::Test::F"))
        {
            return new FI();
        }
        else if(type.Equals("::Test::H"))
        {
            return new HI();
        }
        else if(type.Equals("::Test::I"))
        {
            return new II();
        }
        else if(type.Equals("::Test::J"))
        {
            return new JI();
        }
        return null;
    }
    
    public void destroy()
    {
    }
}
