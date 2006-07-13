// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;

public sealed class MyDerivedClassI : Test.MyDerivedClass
{
    private static void test(bool b)
    {
	if (!b)
	{
	    throw new Exception();
	}
    }

    public MyDerivedClassI(Ice.ObjectAdapter adapter, Ice.Identity identity)
    {
	_adapter = adapter;
	_identity = identity;
    }
    
    public override void shutdown(Ice.Current current)
    {
	_adapter.getCommunicator().shutdown();
    }
    
    public override void opVoid(Ice.Current current)
    {
    }
    
    public override void opSleep(int duration, Ice.Current current)
    {
	System.Threading.Thread.Sleep(duration);
    }

    public override bool opBool(bool p1, bool p2, out bool p3, Ice.Current current)
    {
	p3 = p1;
	return p2;
    }
    
    public override Test.BoolS opBoolS(Test.BoolS p1, Test.BoolS p2, out Test.BoolS p3, Ice.Current current)
    {
	p3 = new Test.BoolS();
	p3.AddRange(p1);
	p3.AddRange(p2);
	Test.BoolS r = new Test.BoolS();
	for(int i = 0; i < p1.Count; i++)
	{
	    r.Add(p1[p1.Count - (i + 1)]);
	}
	return r;
    }
    
    public override Test.BoolSS opBoolSS(Test.BoolSS p1, Test.BoolSS p2, out Test.BoolSS p3, Ice.Current current)
    {
	p3 = new Test.BoolSS();
	p3.AddRange(p1);
	p3.AddRange(p2);
	Test.BoolSS r = new Test.BoolSS();
	for(int i = 0; i < p1.Count; i++)
	{
	    r.Add(p1[p1.Count - (i + 1)]);
	}
	return r;
    }
    
    public override byte opByte(byte p1, byte p2, out byte p3, Ice.Current current)
    {
	p3 = (byte)(p1 ^ p2);
	return p1;
    }
    
    public override Test.ByteBoolD opByteBoolD(Test.ByteBoolD p1, Test.ByteBoolD p2, out Test.ByteBoolD p3,
                                               Ice.Current current)
    {
	p3 = p1;
	Test.ByteBoolD r = new Test.ByteBoolD();
	r.AddRange(p1);
	r.AddRange(p2);
	return r;
    }
    
    public override Test.ByteS opByteS(Test.ByteS p1, Test.ByteS p2, out Test.ByteS p3, Ice.Current current)
    {
	p3 = new Test.ByteS();
	for(int i = 0; i < p1.Count; i++)
	{
	    p3.Add(p1[p1.Count - (i + 1)]);
	}
	
	Test.ByteS r = new Test.ByteS(p1.ToArray());
	r.AddRange(p2);
	return r;
    }
    
    public override Test.ByteSS opByteSS(Test.ByteSS p1, Test.ByteSS p2, out Test.ByteSS p3, Ice.Current current)
    {
	p3 = new Test.ByteSS();
	for(int i = 0; i < p1.Count; i++)
	{
	    p3.Add(p1[p1.Count - (i + 1)]);
	}
	
	Test.ByteSS r = new Test.ByteSS();
	r.AddRange(p1);
	r.AddRange(p2);
	return r;
    }
    
    public override double opFloatDouble(float p1, double p2, out float p3, out double p4, Ice.Current current)
    {
	p3 = p1;
	p4 = p2;
	return p2;
    }
    
    public override Test.DoubleS opFloatDoubleS(Test.FloatS p1, Test.DoubleS p2,
                                                out Test.FloatS p3, out Test.DoubleS p4,
				                Ice.Current current)
    {
	p3 = p1;
	p4 = new Test.DoubleS();
	for(int i = 0; i < p2.Count; i++)
	{
	    p4.Add(p2[p2.Count - (i + 1)]);
	}
	Test.DoubleS r = new Test.DoubleS();
	r.AddRange(p2);
	for(int i = 0; i < p1.Count; i++)
	{
	    r.Add(p1[i]);
	}
	return r;
    }
    
    public override Test.DoubleSS opFloatDoubleSS(Test.FloatSS p1, Test.DoubleSS p2,
						  out Test.FloatSS p3, out Test.DoubleSS p4,
                                                  Ice.Current current)
    {
	p3 = p1;
	p4 = new Test.DoubleSS();
	for(int i = 0; i < p2.Count; i++)
	{
	    p4.Add(p2[p2.Count - (i + 1)]);
	}
	Test.DoubleSS r = new Test.DoubleSS();
	r.AddRange(p2);
	r.AddRange(p2);
	return r;
    }
    
    public override Test.LongFloatD opLongFloatD(Test.LongFloatD p1, Test.LongFloatD p2, out Test.LongFloatD p3,
                                                 Ice.Current current)
    {
	p3 = p1;
	Test.LongFloatD r = new Test.LongFloatD();
	r.AddRange(p1);
	r.AddRange(p2);
	return r;
    }
    
    public override Test.MyClassPrx opMyClass(Test.MyClassPrx p1, out Test.MyClassPrx p2, out Test.MyClassPrx p3,
                                              Ice.Current current)
    {
	p2 = p1;
	p3 = Test.MyClassPrxHelper.uncheckedCast(_adapter.createProxy(
						_adapter.getCommunicator().stringToIdentity("noSuchIdentity")));
	return Test.MyClassPrxHelper.uncheckedCast(_adapter.createProxy(_identity));
    }
    
    public override Test.MyEnum opMyEnum(Test.MyEnum p1, out Test.MyEnum p2, Ice.Current current)
    {
	p2 = p1;
	return Test.MyEnum.enum3;
    }
    
    public override Test.ShortIntD opShortIntD(Test.ShortIntD p1, Test.ShortIntD p2, out Test.ShortIntD p3,
                                               Ice.Current current)
    {
	p3 = p1;
	Test.ShortIntD r = new Test.ShortIntD();
	r.AddRange(p1);
	r.AddRange(p2);
	return r;
    }
    
    public override long opShortIntLong(short p1, int p2, long p3, out short p4, out int p5, out long p6,
                                        Ice.Current current)
    {
	p4 = p1;
	p5 = p2;
	p6 = p3;
	return p3;
    }
    
    public override Test.LongS opShortIntLongS(Test.ShortS p1, Test.IntS p2, Test.LongS p3,
                                               out Test.ShortS p4, out Test.IntS p5, out Test.LongS p6,
					       Ice.Current current)
    {
	p4 = p1;
	p5 = new Test.IntS();
	for(int i = 0; i < p2.Count; i++)
	{
	    p5.Add(p2[p2.Count - (i + 1)]);
	}
	p6 = new Test.LongS();
	p6.AddRange(p3);
	p6.AddRange(p3);
	return p3;
    }
    
    public override Test.LongSS opShortIntLongSS(Test.ShortSS p1, Test.IntSS p2, Test.LongSS p3,
                                                 out Test.ShortSS p4, out Test.IntSS p5, out Test.LongSS p6,
						 Ice.Current current)
    {
	p4 = p1;
	p5 = new Test.IntSS();
	for(int i = 0; i < p2.Count; i++)
	{
	    p5.Add(p2[p2.Count - (i + 1)]);
	}
	p6 = new Test.LongSS();
	p6.AddRange(p3);
	p6.AddRange(p3);
	return p3;
    }
    
    public override string opString(string p1, string p2, out string p3, Ice.Current current)
    {
	p3 = p2 + " " + p1;
	return p1 + " " + p2;
    }
    
    public override Test.StringMyEnumD opStringMyEnumD(Test.StringMyEnumD p1, Test.StringMyEnumD p2,
                                                       out Test.StringMyEnumD p3, Ice.Current current)
    {
	p3 = p1;
	Test.StringMyEnumD r = new Test.StringMyEnumD();
	r.AddRange(p1);
	r.AddRange(p2);
	return r;
    }
    
    public override Test.IntS opIntS(Test.IntS s, Ice.Current current)
    {
	Test.IntS r = new Test.IntS();
	for(int i = 0; i < s.Count; ++i)
	{
	    r.Add(-s[i]);
	}
	return r;
    }
    
    public override void opByteSOneway(Test.ByteS s, Ice.Current current)
    {
    }

    public override Ice.Context opContext(Ice.Current current)
    {
	return current.ctx == null ? new Ice.Context() : (Ice.Context)current.ctx.Clone();
    }
    
    public override void opDoubleMarshaling(double p1, Test.DoubleS p2, Ice.Current current)
    {
	double d = 1278312346.0 / 13.0;
	test(p1 == d);
	for(int i = 0; i < p2.Count; ++i)
	{
	    test(p2[i] == d);
	}
    }

    public override Test.StringS opStringS(Test.StringS p1, Test.StringS p2, out Test.StringS p3, Ice.Current current)
    {
	p3 = new Test.StringS();
	p3.AddRange(p1);
	p3.AddRange(p2);
	
	Test.StringS r = new Test.StringS();
	for(int i = 0; i < p1.Count; i++)
	{
	    r.Add(p1[p1.Count - (i + 1)]);
	}
	return r;
    }
    
    public override Test.StringSS opStringSS(Test.StringSS p1, Test.StringSS p2, out Test.StringSS p3,
                                             Ice.Current current)
    {
	p3 = new Test.StringSS();
	p3.AddRange(p1);
	p3.AddRange(p2);
	
	Test.StringSS r = new Test.StringSS();
	for(int i = 0; i < p2.Count; i++)
	{
	    r.Add(p2[p2.Count - (i + 1)]);
	}
	return r;
    }

    public override Test.StringSS[] opStringSSS(Test.StringSS[] p1, Test.StringSS[] p2, out Test.StringSS[] p3,
                                                Ice.Current current)
    {
        p3 = new Test.StringSS[p1.Length + p2.Length];
        p1.CopyTo(p3, 0);
        p2.CopyTo(p3, p1.Length);
	
        Test.StringSS[] r = new Test.StringSS[p2.Length];
        for(int i = 0; i < p2.Length; i++)
        {
            r[i] = p2[p2.Length - (i + 1)];
        }
        return r;
    }
     
    public override Test.StringStringD opStringStringD(Test.StringStringD p1, Test.StringStringD p2,
                                                       out Test.StringStringD p3, Ice.Current current)
    {
	p3 = p1;
	Test.StringStringD r = new Test.StringStringD();
	r.AddRange(p1);
	r.AddRange(p2);
	return r;
    }
    
    public override Test.Structure opStruct(Test.Structure p1, Test.Structure p2,
                                            out Test.Structure p3, Ice.Current current)
    {
	p3 = p1;
	p3.s.s = "a new string";
	return p2;
    }
    
    public override void opDerived(Ice.Current current)
    {
    }
    
    private Ice.ObjectAdapter _adapter;
    private Ice.Identity _identity;
}
