

var Promise = require("../../src/Ice/Promise");
var Communicator = require("../../src/Ice/Communicator");
var InputStream = require("../../src/Ice/InputStream");
var OutputStream = require("../../src/Ice/OutputStream");
var LocalEx = require("../../src/Ice/LocalException").Ice;
var StreamHelpers = require("../../src/Ice/StreamHelpers");
var Ice = require("../../src/Ice/Ice")
var Debug = require("../../src/Ice/Debug");

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
        console.log("ok");
    }
    catch(ex)
    {
        console.log(ex);
    }
    comm.destroy();
};

StreamTest.run();
