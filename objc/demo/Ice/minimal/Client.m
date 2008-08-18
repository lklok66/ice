// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

#import <Foundation/NSAutoreleasePool.h>

#import <Hello.h>

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status = 0;
    id<ICECommunicator> communicator;
    @try
    {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];
        if(argc > 1)
        {
            NSLog(@"%s: too many arguments", argv[0]);
            return 1;
        }
        id<DemoHelloPrx> hello = [DemoHelloPrx checkedCast:[communicator stringToProxy:@"hello:tcp -p 10000"]];
        if(hello == nil)
        {
            NSLog(@"%s: invalid proxy", argv[0]);
            return 1;
        }
        else
        {
            [hello sayHello];
        }
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
    return 0;
}
