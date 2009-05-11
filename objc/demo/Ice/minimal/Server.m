// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <HelloI.h>

#import <Foundation/NSAutoreleasePool.h>

#if !TARGET_OS_IPHONE
  #import <objc/objc-auto.h>
#endif

int
main(int argc, char* argv[])
{
#if !TARGET_OS_IPHONE
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = 0;
    id<ICECommunicator> communicator = nil;
    @try
    {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];
        if(argc > 1)
        {
            NSLog(@"%s: too many arguments", argv[0]);
            return 1;
        }

        id<ICEObjectAdapter> adapter = [communicator createObjectAdapterWithEndpoints:@"Hello"
                                                     endpoints:@"tcp -p 10000"];
        DemoHello* hello = [[[HelloI alloc] init] autorelease];
        [adapter add:hello identity:[communicator stringToIdentity:@"hello"]];
        [adapter activate];
        [communicator waitForShutdown];
    }
    @catch(ICELocalException* ex)
    {
        NSLog(@"%@", ex);
        status = 1;
    }

    if(communicator != nil)
    {
        @try
        {
            [communicator destroy];
        }
        @catch(ICELocalException* ex)
        {
            NSLog(@"%@", ex);
            status = 1;
        }
    }

    [pool release];
    return status;
}
