

var ArrayUtil = require("Ice/ArrayUtil");
var Promise = require("Ice/Promise");
var Long = require("Ice/Long");
var Communicator = require("Ice/Communicator");
var InputStream = require("Ice/InputStream");
var OutputStream = require("Ice/OutputStream");
var LocalEx = require("Ice/LocalException").Ice;
var StreamHelpers = require("Ice/StreamHelpers");
var Ice = require("Ice/Ice");
var Protocol = require("Ice/Protocol");
var Debug = require("Ice/Debug");

var Test = require("./StreamTest").Test;

var is;
var os;
var data;
StreamTest = {}

var comm = Ice.initialize();

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
                Debug.assert(ex instanceof LocalEx.UnmarshalOutOfBoundsException);
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
            var MyEnumHelper = StreamHelpers.generateEnumHelper(Test.MyEnum);
            MyEnumHelper.write(os, Test.MyEnum.enum3);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(MyEnumHelper.read(is).equals(Test.MyEnum.enum3));
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
            s.l = new Long(0, 4);
            s.f = 5.0;
            s.d = 6.0;
            s.str = "7";
            s.e = Test.MyEnum.enum2;
            //TODO
            //s.p = Test.MyClassPrx.uncheckedCast(comm.stringToProxy("test:default"));
            var SmallStructHelper = StreamHelpers.generateStructHelper(Test.SmallStruct);
            SmallStructHelper.write(os, s);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            Debug.assert(SmallStructHelper.read(is).equals(s));
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
            var os = new OutputStream(comm, Protocol.Encoding_1_0);
            var o = new Test.OptionalClass();
            o.bo = true;
            o.by = 5;
            o.sh = 4;
            o.i = 3;
            os.writeObject(o);
            os.writePendingObjects();
            var data = os.finished();
            var is = new InputStream(comm, data, true, Protocol.Encoding_1_0);
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
            StreamHelpers.SequenceHelper.write(os, arr, [StreamHelpers.BoolHelper]);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            var arr2 = StreamHelpers.SequenceHelper.read(is, [StreamHelpers.BoolHelper]);
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
            StreamHelpers.SequenceHelper.write(os, arrS, [StreamHelpers.SequenceHelper, StreamHelpers.BoolHelper]);
            data = os.finished();
            is = new InputStream(comm, data, true);
            var arr2S = StreamHelpers.SequenceHelper.read(is, [StreamHelpers.SequenceHelper, StreamHelpers.BoolHelper]);
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
            StreamHelpers.SequenceHelper.write(os, arr, [StreamHelpers.ByteHelper]);
            var data = os.finished();
            var is = new InputStream(comm, data, true);
            var arr2 = StreamHelpers.SequenceHelper.read(is, [StreamHelpers.ByteHelper]);
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
            StreamHelpers.SequenceHelper.write(os, arrS, [StreamHelpers.SequenceHelper, StreamHelpers.ByteHelper]);
            data = os.finished();
            is = new InputStream(comm, data, true);
            var arr2S = StreamHelpers.SequenceHelper.read(is, [StreamHelpers.SequenceHelper, StreamHelpers.ByteHelper]);
            Debug.assert(arr2S.length === arrS.length);
            for(var i = 0; i < arr2S.length; ++i)
            {
                Debug.assert(ArrayUtil.equals(arr2S[i], arrS[i]));
            }
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
