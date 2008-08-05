// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSObjCRuntime.h>

#import <IceObjC/IceObjC.h>

#import <Hello.h>

int
main(int argc, char* argv[])
{
    ICEInitializationData* initData = [[ICEInitializationData alloc] init];
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    ICECommunicator* communicator = [ICECommunicator create:&argc argv:argv initData:initData];
    id<HelloPrx> hello = [HelloPrx uncheckedCast:[communicator stringToProxy:@"hello:tcp -p 10000"]];
    @try
    {
        [hello sayHello];
    }
    @catch(ICELocalException* ex)
    {
        NSLog(@"%@", ex);
    }
    [initData release];
    [communicator destroy];
    [pool release];
    return 0;
}
