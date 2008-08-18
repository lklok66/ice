// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <Test.h>
#import <TestI.h> // TODO: remove
#import <limits.h>
    
void
twoways(id<ICECommunicator> communicator, TestMyClassPrx* p)
{
    {
        [p opVoid];
    }

    {
        ICEByte b;
        ICEByte r;

        r = [p opByte:(ICEByte)0xff p2:(ICEByte)0x0f p3:&b];
        test(b == (ICEByte)0xf0);
        test(r == (ICEByte)0xff);
    }
}
