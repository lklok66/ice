

var Ice = require("Ice/Ice");

var Debug = require("Ice/Debug").Ice.Debug;
var ArrayUtil = require("Ice/ArrayUtil").Ice.ArrayUtil;

var InputStream = require("Ice/InputStream").Ice.InputStream;
var OutputStream = require("Ice/OutputStream").Ice.OutputStream;

var Test = require("./StreamTest").Test;

var is;
var os;
var data;
StreamTest = {}

var comm = Ice.initialize();

function test(v)
{
    Debug.assert(v);
}

StreamTest.run = function(){
    try
    {
        process.stdout.write("testing primitive types... ");
        (function()
        {
            var data = new Buffer(0);
            var is = new InputStream(comm, data, true);
            is.destroy();
        }());

        (function()
        {
            var os = new OutputStream(comm);
            os.startEncapsulation();
            os.writeBool(true);
            os.endEncapsulation();
            var data = os.finished();
            os.destroy();

            var is = new InputStream(comm, data, true);
            is.startEncapsulation();
            Debug.assert(is.readBool());
            is.endEncapsulation();
            is.destroy();

            is = new InputStream(comm, data, true);
            is.startEncapsulation();
            Debug.assert(is.readBool());
            is.endEncapsulation();
            is.destroy();
        }());
        
        (function()
        {
            var data = new Buffer(0);
            var is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            os.writeBool(true);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readBool());
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            var os = new OutputStream(comm);
            os.writeByte(1);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readByte() === 1);
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            var os = new OutputStream(comm);
            os.writeShort(2);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readShort() === 2);
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            var os = new OutputStream(comm);
            os.writeInt(3);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readInt() === 3);
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            var os = new OutputStream(comm);
            os.writeFloat(5.0);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readFloat() === 5.0);
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            os = new OutputStream(comm);
            os.writeDouble(6.0);
            data = os.finished();
            is = new InputStream(comm, data, true);
            Debug.assert(is.readDouble() === 6.0);
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            os = new OutputStream(comm);
            os.writeString("hello world");
            data = os.finished();
            is = new InputStream(comm, data, true);
            Debug.assert(is.readString() == "hello world");
            os.destroy();
            is.destroy();
        }());
        console.log("ok");
        
        process.stdout.write("testing constructed types... ");
        (function()
        {
            var os = new OutputStream(comm);
            os.writeEnum(Test.MyEnum.enum3);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readEnum(Test.MyEnum).equals(Test.MyEnum.enum3));
            os.destroy();
            is.destroy();
        }());
        
        (function()
        {
            var os = new OutputStream(comm);
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
            //TODO
            //s.p = Test.MyClassPrx.uncheckedCast(comm.stringToProxy("test:default"));
            os.writeStruct(s);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(is.readStruct(Test.SmallStruct).equals(s));
            os.destroy();
            is.destroy();
        }());
            
        (function()
        {
            var os = new OutputStream(comm);
            var o = new Test.OptionalClass();
            o.bo = true;
            o.by = 5;
            o.sh = 4;
            o.i = 3;
            os.writeObject(o);
            os.writePendingObjects();
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            
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
            var os = new OutputStream(comm, Ice.Protocol.Encoding_1_0);
            var o = new Test.OptionalClass();
            o.bo = true;
            o.by = 5;
            o.sh = 4;
            o.i = 3;
            os.writeObject(o);
            os.writePendingObjects();
            var data = os.finished();
            var is = new InputStream(comm, data, true, Ice.Protocol.Encoding_1_0);
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
            
            var os = new OutputStream(comm);
            Ice.BoolSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            
            os = new OutputStream(comm);
            Test.BoolSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var arr =
            [
                0x01,
                0x11,
                0x12,
                0x22
            ];
            os = new OutputStream(comm);
            Ice.ByteSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.ByteSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Ice.ShortSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.ShortSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Ice.IntSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.IntSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Ice.LongSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.LongSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Ice.FloatSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.FloatSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Ice.DoubleSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.DoubleSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Ice.StringSeqHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.StringSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
            var os = new OutputStream(comm);
            Test.MyEnumSHelper.write(os, arr);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.MyEnumSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
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
                arr[i].seq2 = [1, 2, 3, 4];
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
            var os = new OutputStream(comm);
            Test.MyClassSHelper.write(os, arr);
            os.writePendingObjects();
            var data = os.finished();
            var is = new InputStream(comm, data, true);
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
            os = new OutputStream(comm);
            Test.MyClassSSHelper.write(os, arrS);
            data = os.finished();
            is = new InputStream(comm, data, true);
            var arr2S = Test.MyClassSSHelper.read(is);
            test(arr2S.length == arrS.length);
            test(arr2S[0].length == arrS[0].length);
            test(arr2S[1].length == arrS[1].length);
            test(arr2S[2].length == arrS[2].length);
            os.destroy();
            is.destroy();
        }());
        
        /*os = new OutputStream(comm);
        os.writeObject(new Test.Derived(1, 2, 3, 4));
        os.writePendingObjects();
        data = os.finished();
        is = new InputStream(comm, data, true);
        
        o2 = null;
        is.readObject(function(obj) { o2 = obj;}, Test.Derived );
        is.readPendingObjects();
        
        Debug.assert(o2.a === 1);
        Debug.assert(o2.b === 2);
        Debug.assert(o2.c === 3);
        Debug.assert(o2.d === 4);*/
        
        console.log("ok");
    }
    catch(ex)
    {
        console.log(ex);
        console.log(ex.stack);
    }
    comm.destroy();
};

StreamTest.run();
