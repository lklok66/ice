// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

public final class Holder<T>
{
    public
    Holder()
    {
    }

    public
    Holder(T value)
    {
        this.value = value;
    }

    public T value;
}
