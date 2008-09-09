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

void
batchOneways(id<TestMyClassPrx> p)
{
    ICEByte buf1[10 * 1024];
    ICEByte buf2[99 * 1024];
    ICEByte buf3[100 * 1024];
    TestMutableByteS *bs1 = [TestMutableByteS dataWithBytes:buf1 length:sizeof(buf1)];
    TestMutableByteS *bs2 = [TestMutableByteS dataWithBytes:buf2 length:sizeof(buf2)];
    TestMutableByteS *bs3 = [TestMutableByteS dataWithBytes:buf3 length:sizeof(buf3)];

    @try
    {
        [p opByteSOneway:bs1];
    }
    @catch(ICEMemoryLimitException*)
    {
        test(NO);
    }

    @try
    {
        [p opByteSOneway:bs2];
    }
    @catch(ICEMemoryLimitException*)
    {
        test(NO);
    }
    
    @try
    {
        [p opByteSOneway:bs3];
        test(NO);
    }
    @catch(ICEMemoryLimitException*)
    {
    }
    
    id<TestMyClassPrx> batch = [TestMyClassPrx uncheckedCast:[p ice_batchOneway]];
    
    int i;

    for(i = 0 ; i < 30 ; ++i)
    {
        @try
        {
            [batch opByteSOneway:bs1];
        }
        @catch(ICEMemoryLimitException*)
        {
            test(NO);
        }
    }
    
    [[batch ice_getConnection] flushBatchRequests];
}
