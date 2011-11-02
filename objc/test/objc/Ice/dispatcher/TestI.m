// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <dispatcher/TestI.h>

@implementation TestDispatcherTestIntfI
-(void) op:(ICECurrent*)current
{
}
-(void) opWithPayload:(ICEMutableByteSeq*)data current:(ICECurrent*)current
{
}
-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end

@implementation TestDispatcherTestIntfControllerI
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
