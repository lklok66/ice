// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <hold/TestI.h>
#import <TestCommon.h>

#import <Foundation/Foundation.h>

@implementation HoldI

-(void) putOnHold:(ICEInt)milliSeconds current:(ICECurrent*)current
{
    @try
    {
        if(milliSeconds <= 0)
        {
            [current.adapter hold];
            [current.adapter activate];
        }
        else
        {
            [current.adapter hold];
            [current.adapter activate];
//         @try
//         {
//             [_timer schedule:this XXX:IceUtilTimemilliSeconds(milliSeconds]);
//         }
//         @catch(IceUtilIllegalArgumentException*)
//         {
//         }
        }
    }
    @catch(ICEObjectAdapterDeactivatedException*)
    {
    }
}

-(ICEInt) set:(ICEInt)value delay:(ICEInt)delay current:(ICECurrent*)current
{
    [NSThread sleepForTimeInterval:delay / 1000.0];

    @synchronized(self)
    {
        ICEInt tmp = last;
        last = value;
        return tmp;
    }
    return 0;
}


-(void) setOneway:(ICEInt)value expected:(ICEInt)expected current:(ICECurrent*)current
{
    @synchronized(self)
    {
        test(last == expected);
        last = value;
    }
}

-(void) shutdown:(ICECurrent*)current
{
    [current.adapter hold];
    [[current.adapter getCommunicator] shutdown];
}

@end
