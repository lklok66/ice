// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <OperationsTest.h>

void
oneways(id<ICECommunicator> communicator, id<TestOperationsMyClassPrx> proxy)
{
    id<TestOperationsMyClassPrx> p = [TestOperationsMyClassPrx uncheckedCast:[proxy ice_oneway]];
    
    {
        [p opVoid];
    }

    {
        ICEByte b;
        ICEByte r;

        @try
        {
            r = [p opByte:(ICEByte)0xff p2:(ICEByte)0x0f p3:&b];
            test(NO);
        }
        @catch(ICETwowayOnlyException*)
        {
        }
    }

}
