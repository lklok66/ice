// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.custom14;

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

import test.Ice.custom14.Test.BoolSeqHolder;
import test.Ice.custom14.Test.ByteSeqHolder;
import test.Ice.custom14.Test.C;
import test.Ice.custom14.Test.CArrayHolder;
import test.Ice.custom14.Test.CListHolder;
import test.Ice.custom14.Test.CSeqHolder;
import test.Ice.custom14.Test.DSeqHolder;
import test.Ice.custom14.Test.DoubleSeqHolder;
import test.Ice.custom14.Test.E;
import test.Ice.custom14.Test.ESeqHolder;
import test.Ice.custom14.Test.FloatSeqHolder;
import test.Ice.custom14.Test.IntSeqHolder;
import test.Ice.custom14.Test.LongSeqHolder;
import test.Ice.custom14.Test.S;
import test.Ice.custom14.Test.SSeqHolder;
import test.Ice.custom14.Test.ShortSeqHolder;
import test.Ice.custom14.Test.StringSeqHolder;
import test.Ice.custom14.Test.StringSeqSeqHolder;
import test.Ice.custom14.Test.TestIntfPrx;
import test.Ice.custom14.Test.TestIntfPrxHelper;

public class AllTests
{
    private static void
    test(boolean b)
    {
        if(!b)
        {
            throw new RuntimeException();
        }
    }

    public static TestIntfPrx
    allTests(Ice.Communicator communicator, PrintWriter out)
    {
        out.print("testing stringToProxy... ");
        out.flush();
        String ref = "test:default -p 12010";
        Ice.ObjectPrx obj = communicator.stringToProxy(ref);
        test(obj != null);
        out.println("ok");

        out.print("testing checked cast... ");
        out.flush();
        TestIntfPrx t = TestIntfPrxHelper.checkedCast(obj);
        test(t != null);
        test(t.equals(obj));
        out.println("ok");

        out.print("testing custom sequences... ");
        out.flush();

        {
            //
            // Create a sequence of C instances, where elements 1..n simply point to element 0.
            //
            C[] seq = new C[5];
            seq[0] = new C();
            for(int i = 1; i < seq.length; i++)
            {
                seq[i] = seq[0];
            }

            //
            // Invoke each operation and verify that the returned sequences have the same
            // structure as the original.
            //
            CSeqHolder seqH = new CSeqHolder();
            C[] seqR = t.opCSeq(seq, seqH);
            test(seqR.length == seq.length);
            test(seqH.value.length == seq.length);
            for(int i = 1; i < seq.length; i++)
            {
                test(seqR[i] != null);
                test(seqR[i] == seqR[0]);
                test(seqR[i] == seqH.value[i]);
            }

            ArrayList arr = new ArrayList(Arrays.asList(seq));
            CArrayHolder arrH = new CArrayHolder();
            List arrR = t.opCArray(arr, arrH);
            test(arrR.size() == arr.size());
            test(arrH.value.size() == arr.size());
            for(int i = 1; i < arr.size(); i++)
            {
                test(arrR.get(i) != null);
                test(arrR.get(i) == arrR.get(0));
                test(arrR.get(i) == arrH.value.get(i));
            }

            LinkedList list = new LinkedList(Arrays.asList(seq));
            CListHolder listH = new CListHolder();
            List listR = t.opCList(list, listH);
            test(listR.size() == list.size());
            test(listH.value.size() == list.size());
            for(int i = 1; i < list.size(); i++)
            {
                test(listR.get(i) != null);
                test(listR.get(i) == listR.get(0));
                test(listR.get(i) == listH.value.get(i));
            }
        }

        {
            final Boolean[] seq = { Boolean.TRUE, Boolean.FALSE, Boolean.TRUE, Boolean.FALSE, Boolean.TRUE };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            BoolSeqHolder listH = new BoolSeqHolder();
            List listR = t.opBoolSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final Byte[] seq = { new Byte((byte)0), new Byte((byte)1), new Byte((byte)2), new Byte((byte)3) };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            ByteSeqHolder listH = new ByteSeqHolder();
            List listR = t.opByteSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final Short[] seq = { new Short((short)0), new Short((short)1), new Short((short)2), new Short((short)3) };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            ShortSeqHolder listH = new ShortSeqHolder();
            List listR = t.opShortSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final Integer[] seq = { new Integer(0), new Integer(1), new Integer(2), new Integer(3) };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            IntSeqHolder listH = new IntSeqHolder();
            List listR = t.opIntSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final Long[] seq = { new Long(0), new Long(1), new Long(2), new Long(3) };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            LongSeqHolder listH = new LongSeqHolder();
            List listR = t.opLongSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final Float[] seq = { new Float(0), new Float(1), new Float(2), new Float(3) };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            FloatSeqHolder listH = new FloatSeqHolder();
            List listR = t.opFloatSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final Double[] seq = { new Double(0), new Double(1), new Double(2), new Double(3) };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            DoubleSeqHolder listH = new DoubleSeqHolder();
            List listR = t.opDoubleSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final String[] seq = { "0", "1", "2", "3", "4" };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            StringSeqHolder listH = new StringSeqHolder();
            List listR = t.opStringSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            final E[] seq = { E.E1, E.E2, E.E3 };
            ArrayList list = new ArrayList(Arrays.asList(seq));
            ESeqHolder listH = new ESeqHolder();
            List listR = t.opESeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            S[] seq = new S[5];
            for(int i = 0; i < seq.length; i++)
            {
                seq[i] = new S();
                seq[i].en = E.convert(i % 3);
            }
            ArrayList list = new ArrayList(Arrays.asList(seq));
            SSeqHolder listH = new SSeqHolder();
            List listR = t.opSSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            ArrayList list = new ArrayList();
            for(int i = 0; i < 5; i++)
            {
                HashMap m = new HashMap();
                for(int j = 0; j < 4; j++)
                {
                    m.put(new Integer(j), "" + j);
                }
                list.add(m);
            }
            DSeqHolder listH = new DSeqHolder();
            List listR = t.opDSeq(list, listH);
            test(listH.value.equals(listR));
            test(listH.value.equals(list));
        }

        {
            ArrayList[] seq = new ArrayList[5];
            for(int i = 0; i < 5; i++)
            {
                final String[] arr = { "0", "1", "2", "3", "4" };
                seq[i] = new ArrayList(Arrays.asList(arr));
            }
            StringSeqSeqHolder listH = new StringSeqSeqHolder();
            List[] listR = t.opStringSeqSeq(seq, listH);
            test(Arrays.equals(listH.value, listR));
            test(Arrays.equals(listH.value, seq));
        }

        out.println("ok");

        return t;
    }
}
