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
run(int argc, char* argv[], id<ICECommunicator> communicator)
{
    if(argc > 1)
    {
        NSLog(@"%s: too many arguments", argv[0]);
        return EXIT_FAILURE;
    }

    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"Hello"];
    DemoHello* hello = [[[HelloI alloc] init] autorelease];
    [adapter add:hello identity:[communicator stringToIdentity:@"hello"]];
    [adapter activate];
    [communicator waitForShutdown];
    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
#ifndef ICE_OBJC_NOGC
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = EXIT_SUCCESS;
    id<ICECommunicator> communicator = nil;
    @try
    {
        ICEInitializationData* initData = [ICEInitializationData initializationData];
        initData.properties = [ICEUtil createProperties];
        [initData.properties load:@"config.server"];

        communicator = [ICEUtil createCommunicator:&argc argv:argv initData:initData];
        status = run(argc, argv, communicator);
    }
    @catch(ICELocalException* ex)
    {
        NSLog(@"%@", ex);
        status = EXIT_FAILURE;
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
            status = EXIT_FAILURE;
        }
    }

    [pool release];
    return status;
}
