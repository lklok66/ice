// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

#import <Foundation/NSAutoreleasePool.h>

#import <Hello.h>

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status = EXIT_SUCCESS;
    id<ICECommunicator> communicator = nil;
    @try
    {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];
        if(argc > 1)
        {
            NSLog(@"%s: too many arguments", argv[0]);
            return EXIT_FAILURE;
        }
        id<DemoHelloPrx> hello = [DemoHelloPrx checkedCast:[communicator stringToProxy:@"hello:tcp -p 10000"]];
        if(hello == nil)
        {
            NSLog(@"%s: invalid proxy", argv[0]);
            status = EXIT_FAILURE;
        }
        else
        {
            [hello sayHello];
        }
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
