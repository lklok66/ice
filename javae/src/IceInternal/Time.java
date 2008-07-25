// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

final public class Time
{
    static public long
    currentMonotonicTimeMillis()
    {
        return System.nanoTime() / 1000000;
    }
}
