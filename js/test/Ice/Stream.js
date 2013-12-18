

var Promise = require("../../src/Ice/Promise");
var Long = require("../../src/Ice/Long");
var Communicator = require("../../src/Ice/Communicator");
var InputStream = require("../../src/Ice/InputStream");
var OutputStream = require("../../src/Ice/OutputStream");
var LocalEx = require("../../src/Ice/LocalException").Ice;
var StreamHelpers = require("../../src/Ice/StreamHelpers");
var Ice = require("../../src/Ice/Ice")
var Debug = require("../../src/Ice/Debug");

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
        data = new Buffer(0);
        is = new InputStream(comm, data, true);
        is.destroy();

        os = new OutputStream(comm);
        os.startEncapsulation();
        os.writeBool(true);
        os.endEncapsulation();
        data = os.finished();
        os.destroy();

        is = new InputStream(comm, data, true);
        is.startEncapsulation();
        Debug.assert(is.readBool());
        is.endEncapsulation();
        is.destroy();

        is = new InputStream(comm, data, true);
        is.startEncapsulation();
        Debug.assert(is.readBool());
        is.endEncapsulation();
        is.destroy();

        data = new Buffer(0);
        is = new InputStream(comm, data, true);
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

        os = new OutputStream(comm);
        os.writeBool(true);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readBool());
        os.destroy();
        is.destroy();

        os = new OutputStream(comm);
        os.writeByte(1);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readByte() === 1);
        os.destroy();
        is.destroy();

        os = new OutputStream(comm);
        os.writeShort(2);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readShort() === 2);
        os.destroy();
        is.destroy();

        os = new OutputStream(comm);
        os.writeInt(3);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readInt() === 3);
        os.destroy();
        is.destroy();
        
        os = new OutputStream(comm);
        os.writeFloat(5.0);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readFloat() === 5.0);
        os.destroy();
        is.destroy();
        
        os = new OutputStream(comm);
        os.writeDouble(6.0);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readDouble() === 6.0);
        os.destroy();
        is.destroy();
        
        os = new OutputStream(comm);
        os.writeString("hello world");
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(is.readString() == "hello world");
        os.destroy();
        is.destroy();
        console.log("ok");
        
        process.stdout.write("testing constructed types... ");
        os = new OutputStream(comm);
        var MyEnumHelper = StreamHelpers.generateEnumHelper(Test.MyEnum);
        MyEnumHelper.write(os, Test.MyEnum.enum3);
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(MyEnumHelper.read(is).equals(Test.MyEnum.enum3));
        os.destroy();
        is.destroy();
        
        os = new OutputStream(comm);
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
        data = os.finished();
        is = new InputStream(comm, data, true);
        Debug.assert(SmallStructHelper.read(is).equals(s));
        os.destroy();
        is.destroy();
        
        os = new OutputStream(comm);
        var o = new Test.OptionalClass();
        o.bo = true;
        o.by = 5;
        o.sh = 4;
        o.i = 3;
        os.writeObject(o);
        os.writePendingObjects();
        data = os.finished();
        is = new InputStream(comm, data, true);
        
        var o2 = null;
        is.readObject(function(obj) { o2 = obj;}, Test.OptionalClass );
        is.readPendingObjects();
        
        Debug.assert(o2.bo == o.bo);
        Debug.assert(o2.by == o.by);
        /*if(comm.getProperties().getProperty("Ice.Default.EncodingVersion").equals("1.0"))
        {
            test(!o2.hasSh());
            test(!o2.hasI());
        }
        else
        {
            test(o2.getSh() == o.getSh());
            test(o2.getI() == o.getI());
        }*/
        os.destroy();
        is.destroy();
        console.log("ok");
    }
    catch(ex)
    {
        console.log(ex);
    }
    comm.destroy();
};

StreamTest.run();
