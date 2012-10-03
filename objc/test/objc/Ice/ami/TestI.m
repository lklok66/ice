// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

#import <ami/TestI.h>
#import <TestCommon.h>

@implementation TestAMITestIntfI
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    _cond = [[NSCondition alloc] init];
    return self;
}
-(void) dealloc
{
    [_cond release];
    [super dealloc];
}
-(void) op:(ICECurrent*)current
{
}
-(void) opWithPayload:(ICEMutableByteSeq*)data current:(ICECurrent*)current
{
}
-(int) opWithResult:(ICECurrent*)current
{
    return 15;
}
-(void) opWithUE:(ICECurrent*)current
{
    @throw [TestAMITestIntfException testIntfException];
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
-(void) opBatch:(ICECurrent *)current
{
    [_cond lock];
    ++_batchCount;
    [_cond signal];
    [_cond unlock];
}
-(ICEInt) opBatchCount:(ICECurrent *)current
{
    [_cond lock];
    @try
    {
        return _batchCount;
    }
    @finally
    {
        [_cond unlock];
    }
    return 0;
}
-(BOOL) waitForBatch:(ICEInt)count current:(ICECurrent *)current
{
    [_cond lock];
    @try
    {
        while(_batchCount < count)
        {
            [_cond waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:500]];
        }
        BOOL result = count == _batchCount;
        _batchCount = 0;
        return result;
    }
    @finally
    {
        [_cond unlock];
    }
    return NO;
}
@end

@implementation TestAMITestIntfControllerI
-(id) initWithAdapter:(id<ICEObjectAdapter>)adapter
{
    if(![super init])
    {
        return nil;
    }
    _adapter = adapter;
    return self;
}
-(void) holdAdapter:(ICECurrent*)current
{
    [_adapter hold];
}
-(void) resumeAdapter:(ICECurrent*)current
{
    [_adapter activate];
}
@end
