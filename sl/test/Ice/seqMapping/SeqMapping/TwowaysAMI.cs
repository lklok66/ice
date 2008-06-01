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
using System.Collections.Generic;
using Test;

public class TwowaysAMI
{
    private static void test(bool b)
    {
        if(!b)
        {
            throw new System.SystemException();
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

    private class AMI_MyClass_opAByteSI 
    {
        public AMI_MyClass_opAByteSI(byte[] i)
        {
            _i = i;
        }

        public void response(byte[] r, byte[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private byte[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLByteSI 
    {
        public AMI_MyClass_opLByteSI(List<byte> i)
        {
            _i = i;
        }

        public void response(List<byte> r, List<byte> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<byte> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCByteSI 
    {
        public AMI_MyClass_opCByteSI(CByteS i)
        {
            _i = i;
        }

        public void response(CByteS r, CByteS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CByteS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opABoolSI 
    {
        public AMI_MyClass_opABoolSI(bool[] i)
        {
            _i = i;
        }

        public void response(bool[] r, bool[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private bool[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLBoolSI 
    {
        public AMI_MyClass_opLBoolSI(List<bool> i)
        {
            _i = i;
        }

        public void response(List<bool> r, List<bool> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<bool> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCBoolSI 
    {
        public AMI_MyClass_opCBoolSI(CBoolS i)
        {
            _i = i;
        }

        public void response(CBoolS r, CBoolS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CBoolS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAShortSI 
    {
        public AMI_MyClass_opAShortSI(short[] i)
        {
            _i = i;
        }

        public void response(short[] r, short[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private short[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLShortSI 
    {
        public AMI_MyClass_opLShortSI(List<short> i)
        {
            _i = i;
        }

        public void response(List<short> r, List<short> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<short> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCShortSI 
    {
        public AMI_MyClass_opCShortSI(CShortS i)
        {
            _i = i;
        }

        public void response(CShortS r, CShortS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CShortS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAIntSI 
    {
        public AMI_MyClass_opAIntSI(int[] i)
        {
            _i = i;
        }

        public void response(int[] r, int[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private int[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLIntSI 
    {
        public AMI_MyClass_opLIntSI(List<int> i)
        {
            _i = i;
        }

        public void response(List<int> r, List<int> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<int> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCIntSI 
    {
        public AMI_MyClass_opCIntSI(CIntS i)
        {
            _i = i;
        }

        public void response(CIntS r, CIntS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CIntS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opALongSI 
    {
        public AMI_MyClass_opALongSI(long[] i)
        {
            _i = i;
        }

        public void response(long[] r, long[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private long[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLLongSI 
    {
        public AMI_MyClass_opLLongSI(List<long> i)
        {
            _i = i;
        }

        public void response(List<long> r, List<long> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<long> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCLongSI 
    {
        public AMI_MyClass_opCLongSI(CLongS i)
        {
            _i = i;
        }

        public void response(CLongS r, CLongS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CLongS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAFloatSI 
    {
        public AMI_MyClass_opAFloatSI(float[] i)
        {
            _i = i;
        }

        public void response(float[] r, float[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private float[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLFloatSI 
    {
        public AMI_MyClass_opLFloatSI(List<float> i)
        {
            _i = i;
        }

        public void response(List<float> r, List<float> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<float> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCFloatSI 
    {
        public AMI_MyClass_opCFloatSI(CFloatS i)
        {
            _i = i;
        }

        public void response(CFloatS r, CFloatS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CFloatS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opADoubleSI 
    {
        public AMI_MyClass_opADoubleSI(double[] i)
        {
            _i = i;
        }

        public void response(double[] r, double[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private double[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLDoubleSI 
    {
        public AMI_MyClass_opLDoubleSI(List<double> i)
        {
            _i = i;
        }

        public void response(List<double> r, List<double> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<double> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCDoubleSI 
    {
        public AMI_MyClass_opCDoubleSI(CDoubleS i)
        {
            _i = i;
        }

        public void response(CDoubleS r, CDoubleS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CDoubleS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAStringSI 
    {
        public AMI_MyClass_opAStringSI(string[] i)
        {
            _i = i;
        }

        public void response(string[] r, string[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private string[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLStringSI 
    {
        public AMI_MyClass_opLStringSI(List<string> i)
        {
            _i = i;
        }

        public void response(List<string> r, List<string> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<string> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCStringSI 
    {
        public AMI_MyClass_opCStringSI(CStringS i)
        {
            _i = i;
        }

        public void response(CStringS r, CStringS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CStringS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAObjectSI 
    {
        public AMI_MyClass_opAObjectSI(Ice.Object[] i)
        {
            _i = i;
        }

        public void response(Ice.Object[] r, Ice.Object[] o)
        {
            System.Collections.IEnumerator eo = o.GetEnumerator();
            System.Collections.IEnumerator er = r.GetEnumerator();
            foreach(CV obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private Ice.Object[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLObjectSI 
    {
        public AMI_MyClass_opLObjectSI(List<Ice.Object> i)
        {
            _i = i;
        }

        public void response(List<Ice.Object> r, List<Ice.Object> o)
        {
            IEnumerator<Ice.Object> eo = o.GetEnumerator();
            IEnumerator<Ice.Object> er = r.GetEnumerator();
            foreach(CV obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<Ice.Object> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCObjectSI 
    {
        public AMI_MyClass_opCObjectSI(CObjectS i)
        {
            _i = i;
        }

        public void response(CObjectS r, CObjectS o)
        {
            IEnumerator<Ice.Object> eo = (IEnumerator<Ice.Object>)o.GetEnumerator();
            IEnumerator<Ice.Object> er = (IEnumerator<Ice.Object>)r.GetEnumerator();
            foreach(CV obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CObjectS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAObjectPrxSI 
    {
        public AMI_MyClass_opAObjectPrxSI(Ice.ObjectPrx[] i)
        {
            _i = i;
        }

        public void response(Ice.ObjectPrx[] r, Ice.ObjectPrx[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private Ice.ObjectPrx[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLObjectPrxSI 
    {
        public AMI_MyClass_opLObjectPrxSI(List<Ice.ObjectPrx> i)
        {
            _i = i;
        }

        public void response(List<Ice.ObjectPrx> r, List<Ice.ObjectPrx> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<Ice.ObjectPrx> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCObjectPrxSI 
    {
        public AMI_MyClass_opCObjectPrxSI(CObjectPrxS i)
        {
            _i = i;
        }

        public void response(CObjectPrxS r, CObjectPrxS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CObjectPrxS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAStructSI 
    {
        public AMI_MyClass_opAStructSI(S[] i)
        {
            _i = i;
        }

        public void response(S[] r, S[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private S[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLStructSI 
    {
        public AMI_MyClass_opLStructSI(List<S> i)
        {
            _i = i;
        }

        public void response(List<S> r, List<S> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<S> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCStructSI 
    {
        public AMI_MyClass_opCStructSI(CStructS i)
        {
            _i = i;
        }

        public void response(CStructS r, CStructS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CStructS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opACVSI 
    {
        public AMI_MyClass_opACVSI(CV[] i)
        {
            _i = i;
        }

        public void response(CV[] r, CV[] o)
        {
            System.Collections.IEnumerator eo = o.GetEnumerator();
            System.Collections.IEnumerator er = r.GetEnumerator();
            foreach(CV obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CV[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLCVSI 
    {
        public AMI_MyClass_opLCVSI(List<CV> i)
        {
            _i = i;
        }

        public void response(List<CV> r, List<CV> o)
        {
            IEnumerator<CV> eo = o.GetEnumerator();
            IEnumerator<CV> er = r.GetEnumerator();
            foreach(CV obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<CV> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCCVSI 
    {
        public AMI_MyClass_opCCVSI(CCVS i)
        {
            _i = i;
        }

        public void response(CCVS r, CCVS o)
        {
            IEnumerator<CV> eo = (IEnumerator<CV>)o.GetEnumerator();
            IEnumerator<CV> er = (IEnumerator<CV>)r.GetEnumerator();
            foreach(CV obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CCVS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opACVPrxSI 
    {
        public AMI_MyClass_opACVPrxSI(CVPrx[] i)
        {
            _i = i;
        }

        public void response(CVPrx[] r, CVPrx[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CVPrx[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLCVPrxSI 
    {
        public AMI_MyClass_opLCVPrxSI(List<CVPrx> i)
        {
            _i = i;
        }

        public void response(List<CVPrx> r, List<CVPrx> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<CVPrx> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCCVPrxSI 
    {
        public AMI_MyClass_opCCVPrxSI(CCVPrxS i)
        {
            _i = i;
        }

        public void response(CCVPrxS r, CCVPrxS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CCVPrxS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opACRSI 
    {
        public AMI_MyClass_opACRSI(CR[] i)
        {
            _i = i;
        }

        public void response(CR[] r, CR[] o)
        {
            System.Collections.IEnumerator eo = o.GetEnumerator();
            System.Collections.IEnumerator er = r.GetEnumerator();
            foreach(CR obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.v.i == ((CR)eo.Current).v.i);
                test(obj.v.i == ((CR)er.Current).v.i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CR[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLCRSI 
    {
        public AMI_MyClass_opLCRSI(List<CR> i)
        {
            _i = i;
        }

        public void response(List<CR> r, List<CR> o)
        {
            IEnumerator<CR> eo = o.GetEnumerator();
            IEnumerator<CR> er = r.GetEnumerator();
            foreach(CR obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.v.i == ((CR)eo.Current).v.i);
                test(obj.v.i == ((CR)er.Current).v.i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<CR> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCCRSI 
    {
        public AMI_MyClass_opCCRSI(CCRS i)
        {
            _i = i;
        }

        public void response(CCRS r, CCRS o)
        {
            IEnumerator<CR> eo = (IEnumerator<CR>)o.GetEnumerator();
            IEnumerator<CR> er = (IEnumerator<CR>)r.GetEnumerator();
            foreach(CR obj in _i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.v.i == ((CR)eo.Current).v.i);
                test(obj.v.i == ((CR)er.Current).v.i);
            }
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CCRS _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opAEnSI 
    {
        public AMI_MyClass_opAEnSI(En[] i)
        {
            _i = i;
        }

        public void response(En[] r, En[] o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private En[] _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opLEnSI 
    {
        public AMI_MyClass_opLEnSI(List<En> i)
        {
            _i = i;
        }

        public void response(List<En> r, List<En> o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private List<En> _i;
        private Callback callback = new Callback();
    }

    private class AMI_MyClass_opCEnSI
    {
        public AMI_MyClass_opCEnSI(CEnS i)
        {
            _i = i;
        }

        public void response(CEnS r, CEnS o)
        {
            test(Ice.CollectionComparer.Equals(_i, o));
            test(Ice.CollectionComparer.Equals(_i, r));
            callback.called();
        }

        public void exception(Ice.Exception ex)
        {
            test(false);
        }

        public virtual bool check()
        {
            return callback.check();
        }

        private CEnS _i;
        private Callback callback = new Callback();
    }

    static int _length = 100;

    internal static void twowaysAMI(Ice.Communicator communicator, Test.MyClassPrx p)
    {
        {
            byte[] i = new byte[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (byte)c;
            }

            AMI_MyClass_opAByteSI cb = new AMI_MyClass_opAByteSI(i);
            p.opAByteS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<byte> i = new List<byte>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((byte)c);
            }

            AMI_MyClass_opLByteSI cb = new AMI_MyClass_opLByteSI(i);
            p.opLByteS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CByteS i = new CByteS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((byte)c);
            }

            AMI_MyClass_opCByteSI cb = new AMI_MyClass_opCByteSI(i);
            p.opCByteS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            bool[] i = new bool[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = c % 1 == 1;
            }

            AMI_MyClass_opABoolSI cb = new AMI_MyClass_opABoolSI(i);
            p.opABoolS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<bool> i = new List<bool>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c % 1 == 1);
            }

            AMI_MyClass_opLBoolSI cb = new AMI_MyClass_opLBoolSI(i);
            p.opLBoolS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CBoolS i = new CBoolS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c % 1 == 1);
            }

            AMI_MyClass_opCBoolSI cb = new AMI_MyClass_opCBoolSI(i);
            p.opCBoolS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            short[] i = new short[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (short)c;
            }

            AMI_MyClass_opAShortSI cb = new AMI_MyClass_opAShortSI(i);
            p.opAShortS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<short> i = new List<short>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((short)c);
            }

            AMI_MyClass_opLShortSI cb = new AMI_MyClass_opLShortSI(i);
            p.opLShortS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CShortS i = new CShortS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((short)c);
            }

            AMI_MyClass_opCShortSI cb = new AMI_MyClass_opCShortSI(i);
            p.opCShortS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            int[] i = new int[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (int)c;
            }

            AMI_MyClass_opAIntSI cb = new AMI_MyClass_opAIntSI(i);
            p.opAIntS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<int> i = new List<int>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((int)c);
            }

            AMI_MyClass_opLIntSI cb = new AMI_MyClass_opLIntSI(i);
            p.opLIntS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CIntS i = new CIntS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((int)c);
            }

            AMI_MyClass_opCIntSI cb = new AMI_MyClass_opCIntSI(i);
            p.opCIntS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            long[] i = new long[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (long)c;
            }

            AMI_MyClass_opALongSI cb = new AMI_MyClass_opALongSI(i);
            p.opALongS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<long> i = new List<long>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((long)c);
            }

            AMI_MyClass_opLLongSI cb = new AMI_MyClass_opLLongSI(i);
            p.opLLongS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CLongS i = new CLongS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((long)c);
            }

            AMI_MyClass_opCLongSI cb = new AMI_MyClass_opCLongSI(i);
            p.opCLongS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            float[] i = new float[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (float)c;
            }

            AMI_MyClass_opAFloatSI cb = new AMI_MyClass_opAFloatSI(i);
            p.opAFloatS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<float> i = new List<float>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((float)c);
            }

            AMI_MyClass_opLFloatSI cb = new AMI_MyClass_opLFloatSI(i);
            p.opLFloatS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CFloatS i = new CFloatS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((float)c);
            }

            AMI_MyClass_opCFloatSI cb = new AMI_MyClass_opCFloatSI(i);
            p.opCFloatS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            double[] i = new double[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (double)c;
            }

            AMI_MyClass_opADoubleSI cb = new AMI_MyClass_opADoubleSI(i);
            p.opADoubleS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<double> i = new List<double>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((double)c);
            }

            AMI_MyClass_opLDoubleSI cb = new AMI_MyClass_opLDoubleSI(i);
            p.opLDoubleS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CDoubleS i = new CDoubleS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((double)c);
            }

            AMI_MyClass_opCDoubleSI cb = new AMI_MyClass_opCDoubleSI(i);
            p.opCDoubleS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            string[] i = new string[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = c.ToString();
            }

            AMI_MyClass_opAStringSI cb = new AMI_MyClass_opAStringSI(i);
            p.opAStringS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<string> i = new List<string>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c.ToString());
            }

            AMI_MyClass_opLStringSI cb = new AMI_MyClass_opLStringSI(i);
            p.opLStringS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CStringS i = new CStringS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c.ToString());
            }

            AMI_MyClass_opCStringSI cb = new AMI_MyClass_opCStringSI(i);
            p.opCStringS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

/*
        {
            Ice.Object[] i = new Ice.Object[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = new CV(c);
            }

            AMI_MyClass_opAObjectSI cb = new AMI_MyClass_opAObjectSI(i);
            p.opAObjectS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<Ice.Object> i = new List<Ice.Object>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }

            AMI_MyClass_opLObjectSI cb = new AMI_MyClass_opLObjectSI(i);
            p.opLObjectS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CObjectS i = new CObjectS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }

            AMI_MyClass_opCObjectSI cb = new AMI_MyClass_opCObjectSI(i);
            p.opCObjectS_async(cb.response, cb.exception, i);
            test(cb.check());
        }
*/

        {
            Ice.ObjectPrx[] i = new Ice.ObjectPrx[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = communicator.stringToProxy(c.ToString());
            }

            AMI_MyClass_opAObjectPrxSI cb = new AMI_MyClass_opAObjectPrxSI(i);
            p.opAObjectPrxS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<Ice.ObjectPrx> i = new List<Ice.ObjectPrx>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(communicator.stringToProxy(c.ToString()));
            }

            AMI_MyClass_opLObjectPrxSI cb = new AMI_MyClass_opLObjectPrxSI(i);
            p.opLObjectPrxS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CObjectPrxS i = new CObjectPrxS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(communicator.stringToProxy(c.ToString()));
            }

            AMI_MyClass_opCObjectPrxSI cb = new AMI_MyClass_opCObjectPrxSI(i);
            p.opCObjectPrxS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            S[] i = new S[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c].i = c;
            }

            AMI_MyClass_opAStructSI cb = new AMI_MyClass_opAStructSI(i);
            p.opAStructS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<S> i = new List<S>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new S(c));
            }

            AMI_MyClass_opLStructSI cb = new AMI_MyClass_opLStructSI(i);
            p.opLStructS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CStructS i = new CStructS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new S(c));
            }

            AMI_MyClass_opCStructSI cb = new AMI_MyClass_opCStructSI(i);
            p.opCStructS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

/*
        {
            CV[] i = new CV[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = new CV(c);
            }

            AMI_MyClass_opACVSI cb = new AMI_MyClass_opACVSI(i);
            p.opACVS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<CV> i = new List<CV>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }

            AMI_MyClass_opLCVSI cb = new AMI_MyClass_opLCVSI(i);
            p.opLCVS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CCVS i = new CCVS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }

            AMI_MyClass_opCCVSI cb = new AMI_MyClass_opCCVSI(i);
            p.opCCVS_async(cb.response, cb.exception, i);
            test(cb.check());
        }
*/

        {
            CVPrx[] i = new CVPrx[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = CVPrxHelper.uncheckedCast(communicator.stringToProxy(c.ToString()));
            }

            AMI_MyClass_opACVPrxSI cb = new AMI_MyClass_opACVPrxSI(i);
            p.opACVPrxS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<CVPrx> i = new List<CVPrx>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(CVPrxHelper.uncheckedCast(communicator.stringToProxy(c.ToString())));
            }

            AMI_MyClass_opLCVPrxSI cb = new AMI_MyClass_opLCVPrxSI(i);
            p.opLCVPrxS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CCVPrxS i = new CCVPrxS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(CVPrxHelper.uncheckedCast(communicator.stringToProxy(c.ToString())));
            }

            AMI_MyClass_opCCVPrxSI cb = new AMI_MyClass_opCCVPrxSI(i);
            p.opCCVPrxS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

/*
        {
            CR[] i = new CR[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = new CR(new CV(c));
            }

            AMI_MyClass_opACRSI cb = new AMI_MyClass_opACRSI(i);
            p.opACRS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<CR> i = new List<CR>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CR(new CV(c)));
            }

            AMI_MyClass_opLCRSI cb = new AMI_MyClass_opLCRSI(i);
            p.opLCRS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CCRS i = new CCRS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CR(new CV(c)));
            }

            AMI_MyClass_opCCRSI cb = new AMI_MyClass_opCCRSI(i);
            p.opCCRS_async(cb.response, cb.exception, i);
            test(cb.check());
        }
*/

        {
            En[] i = new En[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (En)(c % 3);
            }

            AMI_MyClass_opAEnSI cb = new AMI_MyClass_opAEnSI(i);
            p.opAEnS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            List<En> i = new List<En>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((En)(c % 3));
            }

            AMI_MyClass_opLEnSI cb = new AMI_MyClass_opLEnSI(i);
            p.opLEnS_async(cb.response, cb.exception, i);
            test(cb.check());
        }

        {
            CEnS i = new CEnS();
            for(int c = 0; c < _length; ++c)
            {
                i.Add((En)(c % 3));
            }

            AMI_MyClass_opCEnSI cb = new AMI_MyClass_opCEnSI(i);
            p.opCEnS_async(cb.response, cb.exception, i);
            test(cb.check());
        }
    }
}
