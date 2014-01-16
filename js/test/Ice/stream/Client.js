// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){
        
        require("Ice/Ice");
        require("Ice/Debug");
        require("Ice/ArrayUtil");
        require("Ice/InputStream");
        require("Ice/OutputStream");
        
        var Ice = global.Ice;
        
        var Debug = Ice.Debug;
        var ArrayUtil = Ice.ArrayUtil;
        var InputStream = Ice.InputStream;
        var OutputStream = Ice.OutputStream;

        require("Test");
        
        var Test = global.Test;
        
        var is;
        var os;
        var data;

        function test(v)
        {
            Debug.assert(v);
        }

        var MyInterfaceI = function()
        {
        };

        MyInterfaceI.prototype = new Test.MyInterface();
        MyInterfaceI.constructor = MyInterfaceI;

        var MyInterfaceFactory = function()
        {
        };

        MyInterfaceFactory.prototype = new Ice.ObjectFactory();
        MyInterfaceFactory.constructor = MyInterfaceFactory;

        MyInterfaceFactory.prototype.create = function(type)
        {
            test(type == Test.MyInterface.ice_staticId());
            return new MyInterfaceI();
        }

        MyInterfaceFactory.prototype.destroy = function()
        {
        };

        var run = function(out)
        {
            var p = new Ice.Promise();
            setTimeout(function()
            {
                var comm = Ice.initialize();
                try
                {
                    comm.addObjectFactory(new MyInterfaceFactory(), Test.MyInterface.ice_staticId());
                    
                    out.write("testing primitive types... ");
                    (function()
                    {
                        var data = Ice.Buffer.createNative();
                        var is = Ice.createInputStream(comm, data);
                        is.destroy();
                    }());

                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        os.startEncapsulation();
                        os.writeBool(true);
                        os.endEncapsulation();
                        var data = os.finished();
                        os.destroy();

                        var is = Ice.createInputStream(comm, data);
                        is.startEncapsulation();
                        Debug.assert(is.readBool());
                        is.endEncapsulation();
                        is.destroy();

                        is = Ice.createInputStream(comm, data);
                        is.startEncapsulation();
                        Debug.assert(is.readBool());
                        is.endEncapsulation();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var data = new Ice.Buffer.createNative();
                        var is = Ice.createInputStream(comm, data);
                        try
                        {
                            is.readBool();
                            Debug.assert(false);
                        }
                        catch(ex)
                        {
                            Debug.assert(ex instanceof Ice.UnmarshalOutOfBoundsException);
                        }
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        os.writeBool(true);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readBool());
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        os.writeByte(1);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readByte() === 1);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        os.writeShort(2);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readShort() === 2);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        os.writeInt(3);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readInt() === 3);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        os.writeFloat(5.0);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readFloat() === 5.0);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        os = Ice.createOutputStream(comm);
                        os.writeDouble(6.0);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readDouble() === 6.0);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    /*(function()
                    {
                        os = Ice.createOutputStream(comm);
                        os.writeString("hello world");
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readString() == "hello world");
                        os.destroy();
                        is.destroy();
                    }());*/
                    out.writeLine("ok");
                    
                    out.write("testing constructed types... ");
                    (function()
                    {
                        var os = Ice.createOutputStream(comm)
                        os.writeEnum(Test.MyEnum.enum3);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readEnum(Test.MyEnum).equals(Test.MyEnum.enum3));
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm)
                        var s = new Test.SmallStruct();
                        s.bo = true;
                        s.by = 1;
                        s.sh = 2;
                        s.i = 3;
                        s.l = new Ice.Long(0, 4);
                        s.f = 5.0;
                        s.d = 6.0;
                        s.str = "7";
                        s.e = Test.MyEnum.enum2;
                        s.p = Test.MyClassPrx.uncheckedCast(comm.stringToProxy("test:default"));
                        os.writeStruct(s);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        Debug.assert(is.readStruct(Test.SmallStruct).equals(s));
                        os.destroy();
                        is.destroy();
                    }());
                    (function()
                    {
                        var os = Ice.createOutputStream(comm)
                        var o = new Test.OptionalClass();
                        o.bo = true;
                        o.by = 5;
                        o.sh = 4;
                        o.i = 3;
                        os.writeObject(o);
                        os.writePendingObjects();
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        
                        var o2 = null;
                        is.readObject(function(obj) { o2 = obj;}, Test.OptionalClass );
                        is.readPendingObjects();
                        
                        Debug.assert(o2.bo == o.bo);
                        Debug.assert(o2.by == o.by);
                        if(comm.getProperties().getProperty("Ice.Default.EncodingVersion") == "1.0")
                        {
                            Debug.assert(o2.sh === undefined);
                            Debug.assert(o2.i === undefined);
                        }
                        else
                        {
                            Debug.assert(o2.sh === o.sh);
                            Debug.assert(o2.sh === 4);
                            Debug.assert(o2.i === o.i);
                            Debug.assert(o2.i === 3);
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    (function()
                    {
                        var os = Ice.createOutputStream(comm, Ice.Encoding_1_0);
                        var o = new Test.OptionalClass();
                        o.bo = true;
                        o.by = 5;
                        o.sh = 4;
                        o.i = 3;
                        os.writeObject(o);
                        os.writePendingObjects();
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data, Ice.Encoding_1_0);
                        var o2 = null;
                        is.readObject(function(obj) { o2 = obj;}, Test.OptionalClass );
                        is.readPendingObjects();
                        
                        Debug.assert(o2.bo === o.bo);
                        Debug.assert(o2.by === o.by);
                        Debug.assert(o2.sh === undefined);
                        Debug.assert(o2.i === undefined);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            true,
                            false,
                            true,
                            false
                        ];
                        
                        var os = Ice.createOutputStream(comm);
                        Ice.BoolSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.BoolSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr, function(v1, v2){ return v1 === v2; }));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        
                        os = Ice.createOutputStream(comm);
                        Test.BoolSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.BoolSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr = Ice.Buffer.createNative(
                        [
                            0x01,
                            0x11,
                            0x12,
                            0x22
                        ]);
                        os = Ice.createOutputStream(comm);
                        Ice.ByteSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.ByteSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.ByteSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.ByteSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            0x01,
                            0x11,
                            0x12,
                            0x22
                        ];
                        var os = Ice.createOutputStream(comm);
                        Ice.ShortSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.ShortSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.ShortSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.ShortSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            0x01,
                            0x11,
                            0x12,
                            0x22
                        ];
                        var os = Ice.createOutputStream(comm);
                        Ice.IntSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.IntSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.IntSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.IntSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            new Ice.Long(0x00, 0x01),
                            new Ice.Long(0x00, 0x11),
                            new Ice.Long(0x00, 0x12),
                            new Ice.Long(0x00, 0x22),
                        ];
                        var os = Ice.createOutputStream(comm);
                        Ice.LongSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.LongSeqHelper.read(is);
                        var eq = ArrayUtil.equals(arr2, arr, function(v1, v2) { return v1.equals(v2); });
                        Debug.assert(eq);
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.LongSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.LongSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i], function(v1, v2) { return v1.equals(v2); }));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            1,
                            2,
                            3,
                            4
                        ];
                        var os = Ice.createOutputStream(comm);
                        Ice.FloatSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.FloatSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.FloatSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.FloatSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            1,
                            2,
                            3,
                            4
                        ];
                        var os = Ice.createOutputStream(comm);
                        Ice.DoubleSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.DoubleSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.DoubleSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.DoubleSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            "string1",
                            "string2",
                            "string3",
                            "string4"
                        ];
                        var os = Ice.createOutputStream(comm);
                        Ice.StringSeqHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Ice.StringSeqHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.StringSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.StringSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr =
                        [
                            Test.MyEnum.enum3,
                            Test.MyEnum.enum2,
                            Test.MyEnum.enum1,
                            Test.MyEnum.enum2
                        ];
                        var os = Ice.createOutputStream(comm);
                        Test.MyEnumSHelper.write(os, arr);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Test.MyEnumSHelper.read(is);
                        Debug.assert(ArrayUtil.equals(arr2, arr, function(v1, v2){ return v1.equals(v2); }));
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.MyEnumSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.MyEnumSSHelper.read(is);
                        Debug.assert(arr2S.length === arrS.length);
                        for(var i = 0; i < arr2S.length; ++i)
                        {
                            Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i], function(v1, v2){ return v1.equals(v2); }));
                        }
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var arr = [];
                        arr.length = 4;
                        for(var i = 0; i < arr.length; ++i)
                        {
                            arr[i] = new Test.MyClass();
                            arr[i].c = arr[i];
                            arr[i].o = arr[i];
                            arr[i].s = new Test.SmallStruct();
                            arr[i].s.e = Test.MyEnum.enum2;
                            arr[i].seq1 = [true, false, true, false];
                            arr[i].seq2 = Ice.Buffer.createNative([1, 2, 3, 4]);
                            arr[i].seq3 = [1, 2, 3, 4];
                            arr[i].seq4 = [1, 2, 3, 4];
                            arr[i].seq5 = [new Ice.Long(0, 1), new Ice.Long(0, 2), new Ice.Long(0, 3), new Ice.Long(0, 4)];
                            arr[i].seq6 = [1, 2, 3, 4];
                            arr[i].seq7 = [1, 2, 3, 4];
                            arr[i].seq8 = ["string1", "string2", "string3", "string4"];
                            arr[i].seq9 = [Test.MyEnum.enum3, Test.MyEnum.enum2, Test.MyEnum.enum1];
                            arr[i].seq10 = [null, null, null, null]; // null elements.
                            arr[i].d = new Ice.HashMap();
                            arr[i].d.set("hi", arr[i]);
                        }
                        var os = Ice.createOutputStream(comm);
                        Test.MyClassSHelper.write(os, arr);
                        os.writePendingObjects();
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var arr2 = Test.MyClassSHelper.read(is);
                        is.readPendingObjects();
                        Debug.assert(arr2.length == arr.length);
                        for(i = 0; i < arr2.length; ++i)
                        {
                            test(arr2[i] !== null);
                            test(arr2[i].c === arr2[i]);
                            test(arr2[i].o === arr2[i]);
                            test(arr2[i].s.e === Test.MyEnum.enum2);
                            test(ArrayUtil.equals(arr2[i].seq1, arr[i].seq1));
                            test(ArrayUtil.equals(arr2[i].seq2, arr[i].seq2));
                            test(ArrayUtil.equals(arr2[i].seq3, arr[i].seq3));
                            test(ArrayUtil.equals(arr2[i].seq4, arr[i].seq4));
                            test(ArrayUtil.equals(arr2[i].seq5, arr[i].seq5, function(v1, v2) { return v1.equals(v2); }));
                            test(ArrayUtil.equals(arr2[i].seq6, arr[i].seq6));
                            test(ArrayUtil.equals(arr2[i].seq7, arr[i].seq7));
                            test(ArrayUtil.equals(arr2[i].seq8, arr[i].seq8));
                            test(ArrayUtil.equals(arr2[i].seq9, arr[i].seq9));
                            test(arr2[i].d.get("hi") === arr2[i]);
                        }
                        os.destroy();
                        is.destroy();

                        var arrS =
                        [
                            arr,
                            [],
                            arr
                        ];
                        os = Ice.createOutputStream(comm);
                        Test.MyClassSSHelper.write(os, arrS);
                        data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var arr2S = Test.MyClassSSHelper.read(is);
                        test(arr2S.length == arrS.length);
                        test(arr2S[0].length == arrS[0].length);
                        test(arr2S[1].length == arrS[1].length);
                        test(arr2S[2].length == arrS[2].length);
                        os.destroy();
                        is.destroy();
                    }());
                    
                    (function()
                    {
                        var i = new MyInterfaceI();
                        var os = Ice.createOutputStream(comm);
                        os.writeObject(i);
                        os.writePendingObjects();
                        var data = os.finished();
                        is = Ice.createInputStream(comm, data);
                        var j = null;
                        is.readObject(function(obj){ j = obj; }, Test.MyInterface);
                        is.readPendingObjects();
                        Debug.assert(j !== null);
                    }());
                    
                    (function()
                    {
                        var os = Ice.createOutputStream(comm);
                        var ex = new Test.MyException();

                        var c = new Test.MyClass();
                        c.c = c;
                        c.o = c;
                        c.s = new Test.SmallStruct();
                        c.s.e = Test.MyEnum.enum2;
                        c.seq1 = [true, false, true, false];
                        c.seq2 = Ice.Buffer.createNative([1, 2, 3, 4]);
                        c.seq3 = [1, 2, 3, 4];
                        c.seq4 = [1, 2, 3, 4];
                        c.seq5 = [new Ice.Long(0, 1), new Ice.Long(0, 2), new Ice.Long(0, 3), new Ice.Long(0, 4)];
                        c.seq6 = [1, 2, 3, 4];
                        c.seq7 = [1, 2, 3, 4];
                        c.seq8 = ["string1", "string2", "string3", "string4"];
                        c.seq9 = [Test.MyEnum.enum3, Test.MyEnum.enum2, Test.MyEnum.enum1];
                        c.seq10 = [null, null, null, null]; // null elements.
                        c.d = new Ice.HashMap(); // HashMap<String, MyClass>
                        c.d.set("hi", c);

                        ex.c = c;
                    
                        os.writeException(ex);
                        var data = os.finished();
            
                        var is = Ice.createInputStream(comm, data);
                        try
                        {
                            is.throwException();
                            test(false);
                        }
                        catch(ex1)
                        {
                            test(ex instanceof Test.MyException);
                            test(ex1.c.s.e == c.s.e);
                            test(ArrayUtil.equals(ex1.c.seq1, c.seq1));
                            test(ArrayUtil.equals(ex1.c.seq2, c.seq2));
                            test(ArrayUtil.equals(ex1.c.seq3, c.seq3));
                            test(ArrayUtil.equals(ex1.c.seq4, c.seq4));
                            test(ArrayUtil.equals(ex1.c.seq5, c.seq5, function(v1, v2){ return v1.equals(v2); }));
                            test(ArrayUtil.equals(ex1.c.seq6, c.seq6));
                            test(ArrayUtil.equals(ex1.c.seq7, c.seq7));
                            test(ArrayUtil.equals(ex1.c.seq8, c.seq8));
                            test(ArrayUtil.equals(ex1.c.seq9, c.seq9));
                        }
                    }());
                    
                    (function()
                    {
                        var dict = new Ice.HashMap(); //HashMap<Byte, Boolean>();
                        dict.set(4, true);
                        dict.set(1, false);
                        var os = Ice.createOutputStream(comm);
                        Test.ByteBoolDHelper.write(os, dict);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var dict2 = Test.ByteBoolDHelper.read(is);
                        test(dict2.equals(dict));
                    }());

                    (function()
                    {
                        var dict = new Ice.HashMap(); //HashMap<Short, Integer>();
                        dict.set(1, 9);
                        dict.set(4, 8);
                        var os = Ice.createOutputStream(comm);
                        Test.ShortIntDHelper.write(os, dict);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var dict2 = Test.ShortIntDHelper.read(is);
                        test(dict2.equals(dict));
                    }());


                    (function()
                    {
                        var floatComparator = function(v1, v2)
                        {
                            return Math.round(v1 * 100) === Math.round(v2 * 100);
                        };
                        var dict = new Ice.HashMap(); // HashMap<Long, Float>();
                        dict.keyComparator = Ice.HashMap.compareEquals;
                        dict.valueComparator = floatComparator;
                        dict.set(new Ice.Long(0, 123809828), 0.51);
                        dict.set(new Ice.Long(0, 123809829), 0.56);
                        var os = Ice.createOutputStream(comm);
                        Test.LongFloatDHelper.write(os, dict);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var dict2 = Test.LongFloatDHelper.read(is);
                        dict2.keyComparator = Ice.HashMap.compareEquals;
                        dict2.valueComparator = floatComparator;
                        test(dict2.equals(dict));
                    }());

                    (function()
                    {
                        var dict = new Ice.HashMap(); // HashMap<String, String>();
                        dict.set("key1", "value1");
                        dict.set("key2", "value2");
                        var os = Ice.createOutputStream(comm);
                        Test.StringStringDHelper.write(os, dict);
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var dict2 = Test.StringStringDHelper.read(is);
                        test(dict2.equals(dict));
                    }());

                    (function()
                    {
                        var dict = new Ice.HashMap(); // HashMap<String, MyClass>();
                        var c = new Test.MyClass();
                        c.s = new Test.SmallStruct();
                        c.s.e = Test.MyEnum.enum2;
                        dict.set("key1", c);
                        c = new Test.MyClass();
                        c.s = new Test.SmallStruct();
                        c.s.e = Test.MyEnum.enum3;
                        dict.set("key2", c);
                        var os = Ice.createOutputStream(comm);
                        Test.StringMyClassDHelper.write(os, dict);
                        os.writePendingObjects();
                        var data = os.finished();
                        var is = Ice.createInputStream(comm, data);
                        var dict2 = Test.StringMyClassDHelper.read(is);
                        is.readPendingObjects();
                        test(dict2.size === dict.size);
                        test(dict2.get("key1").s.e == Test.MyEnum.enum2);
                        test(dict2.get("key2").s.e == Test.MyEnum.enum3);
                    }());
                    p.succeed();
                    out.writeLine("ok");
                }
                catch(ex)
                {
                    p.fail(ex);
                }
                comm.destroy();
            });
            return p;
        };
        
        module.exports.run = run;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/stream"));
