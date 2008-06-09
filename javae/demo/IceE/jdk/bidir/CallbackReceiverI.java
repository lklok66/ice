// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

import Demo.*;

class CallbackReceiverI extends _CallbackReceiverDisp
{
    CallbackReceiverI()
    {
    }

    public void
    callback(int num, Ice.Current current)
    {
        System.out.println("received callback #" + num);
    }
}
