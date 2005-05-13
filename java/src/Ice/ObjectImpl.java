// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

public class ObjectImpl implements Object, java.lang.Cloneable
{
    public
    ObjectImpl()
    {
    }

    public java.lang.Object
    clone()
        throws java.lang.CloneNotSupportedException
    {
        return super.clone();
    }

    public int
    ice_hash()
    {
        return hashCode();
    }

    public final static String[] __ids =
    {
        "::Ice::Object"
    };

    public boolean
    ice_isA(String s)
    {
        return s.equals(__ids[0]);
    }

    public boolean
    ice_isA(String s, Current current)
    {
        return s.equals(__ids[0]);
    }

    public static IceInternal.DispatchStatus
    ___ice_isA(Ice.Object __obj, IceInternal.Incoming __inS, Current __current)
    {
        IceInternal.BasicStream __is = __inS.is();
        IceInternal.BasicStream __os = __inS.os();
        String __id = __is.readString();
        boolean __ret = __obj.ice_isA(__id, __current);
        __os.writeBool(__ret);
        return IceInternal.DispatchStatus.DispatchOK;
    }

    public void
    ice_ping()
    {
        // Nothing to do.
    }

    public void
    ice_ping(Current current)
    {
        // Nothing to do.
    }

    public static IceInternal.DispatchStatus
    ___ice_ping(Ice.Object __obj, IceInternal.Incoming __inS, Current __current)
    {
        __obj.ice_ping(__current);
        return IceInternal.DispatchStatus.DispatchOK;
    }

    public String[]
    ice_ids()
    {
        return __ids;
    }

    public String[]
    ice_ids(Current current)
    {
        return __ids;
    }

    public static IceInternal.DispatchStatus
    ___ice_ids(Ice.Object __obj, IceInternal.Incoming __inS, Current __current)
    {
        IceInternal.BasicStream __os = __inS.os();
        String[] __ret = __obj.ice_ids(__current);
        __os.writeStringSeq(__ret);
        return IceInternal.DispatchStatus.DispatchOK;
    }

    public String
    ice_id()
    {
        return __ids[0];
    }

    public String
    ice_id(Current current)
    {
        return __ids[0];
    }

    public static IceInternal.DispatchStatus
    ___ice_id(Ice.Object __obj, IceInternal.Incoming __inS, Current __current)
    {
        IceInternal.BasicStream __os = __inS.os();
        String __ret = __obj.ice_id(__current);
        __os.writeString(__ret);
        return IceInternal.DispatchStatus.DispatchOK;
    }

    public static String
    ice_staticId()
    {
        return __ids[0];
    }

    public void
    ice_preMarshal()
    {
    }

    public void
    ice_postUnmarshal()
    {
    }

    private final static String[] __all =
    {
        "ice_id",
        "ice_ids",
        "ice_isA",
        "ice_ping"
    };

    public IceInternal.DispatchStatus
    __dispatch(IceInternal.Incoming in, Current current)
    {
        int pos = java.util.Arrays.binarySearch(__all, current.operation);
        if(pos < 0)
        {
            return IceInternal.DispatchStatus.DispatchOperationNotExist;
        }

        switch(pos)
        {
            case 0:
            {
                return ___ice_id(this, in, current);
            }
            case 1:
            {
                return ___ice_ids(this, in, current);
            }
            case 2:
            {
                return ___ice_isA(this, in, current);
            }
            case 3:
            {
                return ___ice_ping(this, in, current);
            }
        }

        assert(false);
        return IceInternal.DispatchStatus.DispatchOperationNotExist;
    }

    public void
    __write(IceInternal.BasicStream __os)
    {
        __os.writeTypeId(ice_staticId());
        __os.startWriteSlice();
        __os.writeSize(0); // For compatibility with the old AFM.
        __os.endWriteSlice();
    }

    public void
    __read(IceInternal.BasicStream __is, boolean __rid)
    {
        if(__rid)
        {
            String myId = __is.readTypeId();
        }

        __is.startReadSlice();

        // For compatibility with the old AFM.
        int sz = __is.readSize();
        if(sz != 0)
        {
            throw new MarshalException();
        }

        __is.endReadSlice();
    }

    public void
    __write(Ice.OutputStream __outS)
    {
        __outS.writeTypeId(ice_staticId());
        __outS.startSlice();
        __outS.writeSize(0); // For compatibility with the old AFM.
        __outS.endSlice();
    }

    public void
    __read(Ice.InputStream __inS, boolean __rid)
    {
        if(__rid)
        {
            String myId = __inS.readTypeId();
        }

        __inS.startSlice();

        // For compatibility with the old AFM.
        int sz = __inS.readSize();
        if(sz != 0)
        {
            throw new MarshalException();
        }

        __inS.endSlice();
    }
}
