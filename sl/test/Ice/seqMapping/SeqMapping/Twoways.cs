// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Collections.Generic;
using Test;

class Twoways
{
    private static void test(bool b)
    {
        if(!b)
        {
            throw new Exception();
        }
    }

    static int _length = 100;

    internal static void twoways(Ice.Communicator communicator, Test.MyClassPrx p)
    {
        {
            byte[] i = new byte[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (byte)c;
            }
            byte[] o;
            byte[] r;

            r = p.opAByteS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<byte> i = new List<byte>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((byte)c);
            }
            List<byte> o;
            List<byte> r;

            r = p.opLByteS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CByteS i = new CByteS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((byte)c);
            }
            CByteS o;
            CByteS r;

            r = p.opCByteS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            bool[] i = new bool[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = c % 1 == 1;
            }
            bool[] o;
            bool[] r;

            r = p.opABoolS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<bool> i = new List<bool>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c % 1 == 1);
            }
            List<bool> o;
            List<bool> r;

            r = p.opLBoolS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CBoolS i = new CBoolS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c % 1 == 1);
            }
            CBoolS o;
            CBoolS r;

            r = p.opCBoolS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            short[] i = new short[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (short)c;
            }
            short[] o;
            short[] r;

            {
                r = p.opAShortS(i, out o);
            }
            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<short> i = new List<short>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((short)c);
            }
            List<short> o;
            List<short> r;

            r = p.opLShortS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CShortS i = new CShortS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((short)c);
            }
            CShortS o;
            CShortS r;

            r = p.opCShortS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            int[] i = new int[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (int)c;
            }
            int[] o;
            int[] r;

            r = p.opAIntS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<int> i = new List<int>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((int)c);
            }
            List<int> o;
            List<int> r;

            r = p.opLIntS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CIntS i = new CIntS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((int)c);
            }
            CIntS o;
            CIntS r;

            r = p.opCIntS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            long[] i = new long[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (long)c;
            }
            long[] o;
            long[] r;

            r = p.opALongS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<long> i = new List<long>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((long)c);
            }
            List<long> o;
            List<long> r;

            r = p.opLLongS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CLongS i = new CLongS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((long)c);
            }
            CLongS o;
            CLongS r;

            r = p.opCLongS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            float[] i = new float[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (float)c;
            }
            float[] o;
            float[] r;

            r = p.opAFloatS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<float> i = new List<float>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((float)c);
            }
            List<float> o;
            List<float> r;

            r = p.opLFloatS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CFloatS i = new CFloatS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((float)c);
            }
            CFloatS o;
            CFloatS r;

            r = p.opCFloatS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            double[] i = new double[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (double)c;
            }
            double[] o;
            double[] r;

            r = p.opADoubleS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<double> i = new List<double>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((double)c);
            }
            List<double> o;
            List<double> r;

            r = p.opLDoubleS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CDoubleS i = new CDoubleS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((double)c);
            }
            CDoubleS o;
            CDoubleS r;

            r = p.opCDoubleS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            string[] i = new string[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = c.ToString();
            }
            string[] o;
            string[] r;

            r = p.opAStringS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<string> i = new List<string>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c.ToString());
            }
            List<string> o;
            List<string> r;

            r = p.opLStringS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CStringS i = new CStringS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c.ToString());
            }
            CStringS o;
            CStringS r;

            r = p.opCStringS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }


        {
            Ice.Object[] i = new CV[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = new CV(c);
            }
            Ice.Object[] o;
            Ice.Object[] r;

            r = p.opAObjectS(i, out o);

            System.Collections.IEnumerator eo = o.GetEnumerator();
            System.Collections.IEnumerator er = r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
        }

        {
            List<Ice.Object> i = new List<Ice.Object>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }
            List<Ice.Object> o;
            List<Ice.Object> r;

            r = p.opLObjectS(i, out o);

            IEnumerator<Ice.Object> eo = o.GetEnumerator();
            IEnumerator<Ice.Object> er = r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
        }

        {
            CObjectS i = new CObjectS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }
            CObjectS o;
            CObjectS r;

            r = p.opCObjectS(i, out o);

            IEnumerator<Ice.Object> eo = (IEnumerator<Ice.Object>)o.GetEnumerator();
            IEnumerator<Ice.Object> er = (IEnumerator<Ice.Object>)r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
        }

        {
            Ice.ObjectPrx[] i = new Ice.ObjectPrx[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = communicator.stringToProxy(c.ToString());
            }
            Ice.ObjectPrx[] o;
            Ice.ObjectPrx[] r;

            r = p.opAObjectPrxS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<Ice.ObjectPrx> i = new List<Ice.ObjectPrx>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(communicator.stringToProxy(c.ToString()));
            }
            List<Ice.ObjectPrx> o;
            List<Ice.ObjectPrx> r;

            r = p.opLObjectPrxS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CObjectPrxS i = new CObjectPrxS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(communicator.stringToProxy(c.ToString()));
            }
            CObjectPrxS o;
            CObjectPrxS r;

            r = p.opCObjectPrxS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            S[] i = new S[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c].i = c;
            }
            S[] o;
            S[] r;

            r = p.opAStructS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<S> i = new List<S>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new S(c));
            }
            List<S> o;
            List<S> r;

            r = p.opLStructS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CStructS i = new CStructS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new S(c));
            }
            CStructS o;
            CStructS r;

            r = p.opCStructS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CV[] i = new CV[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = new CV(c);
            }
            CV[] o;
            CV[] r;

            r = p.opACVS(i, out o);

            System.Collections.IEnumerator eo = o.GetEnumerator();
            System.Collections.IEnumerator er = r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == ((CV)eo.Current).i);
                test(obj.i == ((CV)er.Current).i);
            }
        }

        {
            List<CV> i = new List<CV>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }
            List<CV> o;
            List<CV> r;

            r = p.opLCVS(i, out o);

            IEnumerator<CV> eo = o.GetEnumerator();
            IEnumerator<CV> er = r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == eo.Current.i);
                test(obj.i == er.Current.i);
            }
        }

        {
            CCVS i = new CCVS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }
            CCVS o;
            CCVS r;

            r = p.opCCVS(i, out o);

            IEnumerator<CV> eo = (IEnumerator<CV>)o.GetEnumerator();
            IEnumerator<CV> er = (IEnumerator<CV>)r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.i == eo.Current.i);
                test(obj.i == er.Current.i);
            }
        }

        {
            CR[] i = new CR[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = new CR(new CV(c));
            }
            CR[] o;
            CR[] r;

            r = p.opACRS(i, out o);

            System.Collections.IEnumerator eo = o.GetEnumerator();
            System.Collections.IEnumerator er = r.GetEnumerator();
            foreach(CR obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.v.i == ((CR)eo.Current).v.i);
                test(obj.v.i == ((CR)er.Current).v.i);
            }
        }

        {
            List<CR> i = new List<CR>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CR(new CV(c)));
            }
            List<CR> o;
            List<CR> r;

            r = p.opLCRS(i, out o);

            IEnumerator<CR> eo = o.GetEnumerator();
            IEnumerator<CR> er = r.GetEnumerator();
            foreach(CR obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.v.i == eo.Current.v.i);
                test(obj.v.i == er.Current.v.i);
            }
        }

        {
            CCRS i = new CCRS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CR(new CV(c)));
            }
            CCRS o;
            CCRS r;

            r = p.opCCRS(i, out o);

            IEnumerator<CR> eo = (IEnumerator<CR>)o.GetEnumerator();
            IEnumerator<CR> er = (IEnumerator<CR>)r.GetEnumerator();
            foreach(CR obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                test(obj.v.i == eo.Current.v.i);
                test(obj.v.i == er.Current.v.i);
            }
        }

        {
            En[] i = new En[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = (En)(c % 3);
            }
            En[] o;
            En[] r;

            r = p.opAEnS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<En> i = new List<En>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((En)(c % 3));
            }
            List<En> o;
            List<En> r;

            r = p.opLEnS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CEnS i = new CEnS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add((En)(c % 3));
            }
            CEnS o;
            CEnS r;

            r = p.opCEnS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CVPrx[] i = new CVPrx[_length];
            for(int c = 0; c < _length; ++c)
            {
                i[c] = CVPrxHelper.uncheckedCast(communicator.stringToProxy(c.ToString()));
            }
            CVPrx[] o;
            CVPrx[] r;

            r = p.opACVPrxS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            List<CVPrx> i = new List<CVPrx>(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(CVPrxHelper.uncheckedCast(communicator.stringToProxy(c.ToString())));
            }
            List<CVPrx> o;
            List<CVPrx> r;

            r = p.opLCVPrxS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            CCVPrxS i = new CCVPrxS(_length);
            for(int c = 0; c < _length; ++c)
            {
                i.Add(CVPrxHelper.uncheckedCast(communicator.stringToProxy(c.ToString())));
            }
            CCVPrxS o;
            CCVPrxS r;

            r = p.opCCVPrxS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            Custom<int> i = new Custom<int>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(c);
            }
            Custom<int> o;
            Custom<int> r;

            r = p.opCustomIntS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            Custom<CV> i = new Custom<CV>();
            for(int c = 0; c < _length; ++c)
            {
                i.Add(new CV(c));
            }
            i.Add(null);
            Custom<CV> o;
            Custom<CV> r;

            r = p.opCustomCVS(i, out o);

            IEnumerator<CV> eo = (IEnumerator<CV>)o.GetEnumerator();
            IEnumerator<CV> er = (IEnumerator<CV>)r.GetEnumerator();
            foreach(CV obj in i)
            {
                eo.MoveNext();
                er.MoveNext();
                if(obj == null)
                {
                    test(eo.Current == null);
                    test(er.Current == null);
                }
                else
                {
                    test(obj.i == eo.Current.i);
                    test(obj.i == er.Current.i);
                }
            }
        }

        {
            Custom<Custom<int>> i = new Custom<Custom<int>>();
            for(int c = 0; c < _length; ++c)
            {
                Custom<int> inner = new Custom<int>();
                for(int j = 0; j < c; ++j)
                {
                    inner.Add(j);
                }
                i.Add(inner);
            }
            Custom<Custom<int>> o;
            Custom<Custom<int>> r;

            r = p.opCustomIntSS(i, out o);

            test(Ice.CollectionComparer.Equals(i, o));
            test(Ice.CollectionComparer.Equals(i, r));
        }

        {
            Custom<Custom<CV>> i = new Custom<Custom<CV>>();
            for(int c = 0; c < 10; ++c)
            {
                Custom<CV> inner = new Custom<CV>();
                for(int j = 0; j < c; ++j)
                {
                    inner.Add(new CV(j));
                }
                i.Add(inner);
            }
            Custom<Custom<CV>> o;
            Custom<Custom<CV>> r;

            r = p.opCustomCVSS(i, out o);

            IEnumerator<Custom<CV>> eo = (IEnumerator<Custom<CV>>)o.GetEnumerator();
            IEnumerator<Custom<CV>> er = (IEnumerator<Custom<CV>>)r.GetEnumerator();
            foreach(Custom<CV> s in i)
            {
                eo.MoveNext();
                er.MoveNext();
                IEnumerator<CV> io = (IEnumerator<CV>)eo.Current.GetEnumerator();
                IEnumerator<CV> ir = (IEnumerator<CV>)er.Current.GetEnumerator();
                foreach(CV obj in s)
                {
                    io.MoveNext();
                    ir.MoveNext();
                    if(obj == null)
                    {
                        test(io.Current == null);
                        test(ir.Current == null);
                    }
                    else
                    {
                        test(obj.i == io.Current.i);
                        test(obj.i == ir.Current.i);
                    }
                }
            }
        }
    }
}
