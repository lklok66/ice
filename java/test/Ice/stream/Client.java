// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.stream;

import java.io.PrintWriter;

import test.Ice.stream.Test.BoolSHelper;
import test.Ice.stream.Test.ByteSHelper;
import test.Ice.stream.Test.DoubleSHelper;
import test.Ice.stream.Test.FloatSHelper;
import test.Ice.stream.Test.IntSHelper;
import test.Ice.stream.Test.LongSHelper;
import test.Ice.stream.Test.MyClass;
import test.Ice.stream.Test.MyClassPrxHelper;
import test.Ice.stream.Test.MyClassSHelper;
import test.Ice.stream.Test.MyEnum;
import test.Ice.stream.Test.MyEnumSHelper;
import test.Ice.stream.Test.MyInterface;
import test.Ice.stream.Test.MyInterfaceHelper;
import test.Ice.stream.Test.MyInterfaceHolder;
import test.Ice.stream.Test.ShortSHelper;
import test.Ice.stream.Test.SmallStruct;
import test.Ice.stream.Test.StringSHelper;
import test.Ice.stream.Test._MyInterfaceDisp;

public class Client extends test.Util.Application
{
    private static void
    test(boolean b)
    {
        if(!b)
        {
            throw new RuntimeException();
        }
    }

    private static class TestObjectWriter extends Ice.ObjectWriter
    {
        TestObjectWriter(MyClass obj)
        {
            this.obj = obj;
        }

        public void
        write(Ice.OutputStream out)
        {
            obj.__write(out);
            called = true;
        }

        MyClass obj;
        boolean called = false;
    }

    private static class TestObjectReader extends Ice.ObjectReader
    {
        public void
        read(Ice.InputStream in, boolean rid)
        {
            obj = new MyClass();
            obj.__read(in, rid);
            called = true;
        }

        MyClass obj;
        boolean called = false;
    }

    private static class TestObjectFactory implements Ice.ObjectFactory
    {
        public Ice.Object
        create(String type)
        {
            assert(type.equals(MyClass.ice_staticId()));
            return new TestObjectReader();
        }

        public void
        destroy()
        {
        }
    }

    private static class MyInterfaceI extends _MyInterfaceDisp
    {
    };

    private static class MyInterfaceFactory implements Ice.ObjectFactory
    {
        public Ice.Object
        create(String type)
        {
            assert(type.equals(_MyInterfaceDisp.ice_staticId()));
            return new MyInterfaceI();
        }
        
        public void
        destroy()
        {
        }
    }

    private static class TestReadObjectCallback implements Ice.ReadObjectCallback
    {
        public void
        invoke(Ice.Object obj)
        {
            this.obj = obj;
        }

        Ice.Object obj;
    }

    private static class MyClassFactoryWrapper implements Ice.ObjectFactory
    {
        MyClassFactoryWrapper()
        {
            _factory = MyClass.ice_factory();
        }

        public Ice.Object
        create(String type)
        {
            return _factory.create(type);
        }

        public void
        destroy()
        {
        }

        void
        setFactory(Ice.ObjectFactory factory)
        {
            _factory = factory;
        }

        private Ice.ObjectFactory _factory;
    }

    public int
    run(String[] args)
    {
    	Ice.Communicator communicator = communicator();
        MyClassFactoryWrapper factoryWrapper = new MyClassFactoryWrapper();
        communicator.addObjectFactory(factoryWrapper, MyClass.ice_staticId());
        communicator.addObjectFactory(new MyInterfaceFactory(), _MyInterfaceDisp.ice_staticId());

        Ice.InputStream in;
        Ice.OutputStream out;

        PrintWriter printWriter = getWriter();
        printWriter.print("testing primitive types... ");
        printWriter.flush();

        {
            byte[] data = new byte[0];
            in = Ice.Util.createInputStream(communicator, data);
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.startEncapsulation();
            out.writeBool(true);
            out.endEncapsulation();
            byte[] data = out.finished();
            out.destroy();

            in = Ice.Util.createInputStream(communicator, data);
            in.startEncapsulation();
            test(in.readBool());
            in.endEncapsulation();
            in.destroy();
        }

        {
            byte[] data = new byte[0];
            in = Ice.Util.createInputStream(communicator, data);
            try
            {
                in.readBool();
                test(false);
            }
            catch(Ice.UnmarshalOutOfBoundsException ex)
            {
            }
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeBool(true);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readBool());
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeByte((byte)1);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readByte() == (byte)1);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeShort((short)2);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readShort() == (short)2);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeInt(3);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readInt() == 3);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeLong(4);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readLong() == 4);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeFloat((float)5.0);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readFloat() == (float)5.0);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeDouble(6.0);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readDouble() == 6.0);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            out.writeString("hello world");
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(in.readString().equals("hello world"));
            out.destroy();
            in.destroy();
        }

        printWriter.println("ok");

        printWriter.print("testing constructed types... ");
        printWriter.flush();

        {
            out = Ice.Util.createOutputStream(communicator);
            MyEnum.enum3.ice_write(out);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            test(MyEnum.ice_read(in) == MyEnum.enum3);
            out.destroy();
            in.destroy();
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            SmallStruct s = new SmallStruct();
            s.bo = true;
            s.by = (byte)1;
            s.sh = (short)2;
            s.i = 3;
            s.l = 4;
            s.f = (float)5.0;
            s.d = 6.0;
            s.str = "7";
            s.e = MyEnum.enum2;
            s.p = MyClassPrxHelper.uncheckedCast(communicator.stringToProxy("test:default"));
            s.ice_write(out);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            SmallStruct s2 = new SmallStruct();
            s2.ice_read(in);
            test(s2.equals(s));
            out.destroy();
            in.destroy();
        }

        {
            final boolean[] arr =
            {
                true,
                false,
                true,
                false
            };
            out = Ice.Util.createOutputStream(communicator);
            BoolSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            boolean[] arr2 = BoolSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final byte[] arr =
            {
                (byte)0x01,
                (byte)0x11,
                (byte)0x12,
                (byte)0x22
            };
            out = Ice.Util.createOutputStream(communicator);
            ByteSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            byte[] arr2 = ByteSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final short[] arr =
            {
                (short)0x01,
                (short)0x11,
                (short)0x12,
                (short)0x22
            };
            out = Ice.Util.createOutputStream(communicator);
            ShortSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            short[] arr2 = ShortSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final int[] arr =
            {
                0x01,
                0x11,
                0x12,
                0x22
            };
            out = Ice.Util.createOutputStream(communicator);
            IntSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            int[] arr2 = IntSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final long[] arr =
            {
                0x01,
                0x11,
                0x12,
                0x22
            };
            out = Ice.Util.createOutputStream(communicator);
            LongSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            long[] arr2 = LongSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final float[] arr =
            {
                (float)1,
                (float)2,
                (float)3,
                (float)4
            };
            out = Ice.Util.createOutputStream(communicator);
            FloatSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            float[] arr2 = FloatSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final double[] arr =
            {
                (double)1,
                (double)2,
                (double)3,
                (double)4
            };
            out = Ice.Util.createOutputStream(communicator);
            DoubleSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            double[] arr2 = DoubleSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final String[] arr =
            {
                "string1",
                "string2",
                "string3",
                "string4"
            };
            out = Ice.Util.createOutputStream(communicator);
            StringSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            String[] arr2 = StringSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            final MyEnum[] arr =
            {
                MyEnum.enum3,
                MyEnum.enum2,
                MyEnum.enum1,
                MyEnum.enum2
            };
            out = Ice.Util.createOutputStream(communicator);
            MyEnumSHelper.write(out, arr);
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            MyEnum[] arr2 = MyEnumSHelper.read(in);
            test(java.util.Arrays.equals(arr2, arr));
            out.destroy();
            in.destroy();
        }

        {
            MyClass[] arr = new MyClass[4];
            for(int i = 0; i < arr.length; ++i)
            {
                arr[i] = new MyClass();
                arr[i].c = arr[i];
                arr[i].o = arr[i];
                arr[i].s = new SmallStruct();
                arr[i].s.e = MyEnum.enum2;
                arr[i].seq1 = new boolean[] { true, false, true, false };
                arr[i].seq2 = new byte[] { (byte)1, (byte)2, (byte)3, (byte)4 };
                arr[i].seq3 = new short[] { (short)1, (short)2, (short)3, (short)4 };
                arr[i].seq4 = new int[] { 1, 2, 3, 4 };
                arr[i].seq5 = new long[] { 1, 2, 3, 4 };
                arr[i].seq6 = new float[] { (float)1, (float)2, (float)3, (float)4 };
                arr[i].seq7 = new double[] { (double)1, (double)2, (double)3, (double)4 };
                arr[i].seq8 = new String[] { "string1", "string2", "string3", "string4" };
                arr[i].seq9 = new MyEnum[] { MyEnum.enum3, MyEnum.enum2, MyEnum.enum1 };
                arr[i].seq10 = new MyClass[4]; // null elements.
                arr[i].d = new java.util.HashMap<String, MyClass>();
                arr[i].d.put("hi", arr[i]);
            }
            out = Ice.Util.createOutputStream(communicator);
            MyClassSHelper.write(out, arr);
            out.writePendingObjects();
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            MyClass[] arr2 = MyClassSHelper.read(in);
            in.readPendingObjects();
            test(arr2.length == arr.length);
            for(int i = 0; i < arr2.length; ++i)
            {
                test(arr2[i] != null);
                test(arr2[i].c == arr2[i]);
                test(arr2[i].o == arr2[i]);
                test(arr2[i].s.e == MyEnum.enum2);
                test(java.util.Arrays.equals(arr2[i].seq1, arr[i].seq1));
                test(java.util.Arrays.equals(arr2[i].seq2, arr[i].seq2));
                test(java.util.Arrays.equals(arr2[i].seq3, arr[i].seq3));
                test(java.util.Arrays.equals(arr2[i].seq4, arr[i].seq4));
                test(java.util.Arrays.equals(arr2[i].seq5, arr[i].seq5));
                test(java.util.Arrays.equals(arr2[i].seq6, arr[i].seq6));
                test(java.util.Arrays.equals(arr2[i].seq7, arr[i].seq7));
                test(java.util.Arrays.equals(arr2[i].seq8, arr[i].seq8));
                test(java.util.Arrays.equals(arr2[i].seq9, arr[i].seq9));
                test(arr2[i].d.get("hi") == arr2[i]);
            }
            out.destroy();
            in.destroy();
        }
        
        {
            MyInterface i = new MyInterfaceI();
            out = Ice.Util.createOutputStream(communicator);
            MyInterfaceHelper.write(out, i);
            out.writePendingObjects();
            byte[] data = out.finished();
            in = Ice.Util.createInputStream(communicator, data);
            MyInterfaceHolder j = new MyInterfaceHolder();
            MyInterfaceHelper.read(in, j);
            in.readPendingObjects();
            test(j.value != null);
        }

        {
            out = Ice.Util.createOutputStream(communicator);
            MyClass obj = new MyClass();
            obj.s = new SmallStruct();
            obj.s.e = MyEnum.enum2;
            TestObjectWriter writer = new TestObjectWriter(obj);
            out.writeObject(writer);
            out.writePendingObjects();
            byte[] data = out.finished();
            test(writer.called);
            factoryWrapper.setFactory(new TestObjectFactory());
            in = Ice.Util.createInputStream(communicator, data);
            TestReadObjectCallback cb = new TestReadObjectCallback();
            in.readObject(cb);
            in.readPendingObjects();
            test(cb.obj != null);
            test(cb.obj instanceof TestObjectReader);
            TestObjectReader reader = (TestObjectReader)cb.obj;
            test(reader.called);
            test(reader.obj != null);
            test(reader.obj.s.e == MyEnum.enum2);
            out.destroy();
            in.destroy();
        }

        printWriter.println("ok");

        return 0;
    }

    protected Ice.InitializationData getInitData(Ice.StringSeqHolder argsH)
    {
        Ice.InitializationData initData = new Ice.InitializationData();
        initData.properties = Ice.Util.createProperties(argsH);
        initData.properties.setProperty("Ice.Package.Test", "test.Ice.stream");
        return initData;
    }

    public static void main(String[] args)
    {
        Client app = new Client();
        int result = app.main("Client", args);
        System.gc();
        System.exit(result);
    }
}
