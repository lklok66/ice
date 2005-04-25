// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

public class InputStreamI implements InputStream
{
    public
    InputStreamI(Communicator communicator, byte[] data)
    {
        _communicator = communicator;

	_is = new IceInternal.BasicInputStream(Util.getInstance(communicator), this);
        _is.resize(data.length, true);
        java.nio.ByteBuffer buf = _is.prepareRead();
        buf.position(0);
        buf.put(data);
        buf.position(0);
    }

    public Communicator
    communicator()
    {
        return _communicator;
    }

    public void
    sliceObjects(boolean slice)
    {
        _is.sliceObjects(slice);
    }

    public boolean
    readBool()
    {
        return _is.readBool();
    }

    public boolean[]
    readBoolSeq()
    {
        return _is.readBoolSeq();
    }

    public byte
    readByte()
    {
        return _is.readByte();
    }

    public byte[]
    readByteSeq()
    {
        return _is.readByteSeq();
    }

    public short
    readShort()
    {
        return _is.readShort();
    }

    public short[]
    readShortSeq()
    {
        return _is.readShortSeq();
    }

    public int
    readInt()
    {
        return _is.readInt();
    }

    public int[]
    readIntSeq()
    {
        return _is.readIntSeq();
    }

    public long
    readLong()
    {
        return _is.readLong();
    }

    public long[]
    readLongSeq()
    {
        return _is.readLongSeq();
    }

    public float
    readFloat()
    {
        return _is.readFloat();
    }

    public float[]
    readFloatSeq()
    {
        return _is.readFloatSeq();
    }

    public double
    readDouble()
    {
        return _is.readDouble();
    }

    public double[]
    readDoubleSeq()
    {
        return _is.readDoubleSeq();
    }

    public String
    readString()
    {
        return _is.readString();
    }

    public String[]
    readStringSeq()
    {
        return _is.readStringSeq();
    }

    public int
    readSize()
    {
        return _is.readSize();
    }

    public ObjectPrx
    readProxy()
    {
        return _is.readProxy();
    }

    private static class Patcher implements IceInternal.Patcher
    {
        Patcher(ReadObjectCallback cb)
        {
            _cb = cb;
        }

        public void
        patch(Ice.Object v)
        {
            _cb.invoke(v);
        }

        public String
        type()
        {
            return "unknown";
        }

        ReadObjectCallback _cb;
    }

    public void
    readObject(ReadObjectCallback cb)
    {
        _is.readObject(new Patcher(cb));
    }

    public String
    readTypeId()
    {
        return _is.readTypeId();
    }

    public void
    throwException()
        throws UserException
    {
        _is.throwException();
    }

    public void
    startSlice()
    {
        _is.startReadSlice();
    }

    public void
    endSlice()
    {
        _is.endReadSlice();
    }

    public void
    skipSlice()
    {
        _is.skipSlice();
    }

    public void
    startEncapsulation()
    {
        _is.startReadEncaps();
    }

    public void
    endEncapsulation()
    {
        _is.endReadEncaps();
    }

    public void
    readPendingObjects()
    {
        _is.readPendingObjects();
    }

    public void
    destroy()
    {
        if(_is != null)
        {
            _is.destroy();
            _is = null;
        }
    }

    private Communicator _communicator;
    private IceInternal.BasicInputStream _is;
}
