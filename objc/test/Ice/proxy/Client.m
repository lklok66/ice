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

#import <Foundation/NSAutoreleasePool.h>

int
run(int argc, char* argv[], 
    id<ICECommunicator> communicator,
    ICEInitializationData* initData)
{
    TestMyClassPrx* allTests(id<ICECommunicator>);
    TestMyClassPrx* myClass = allTests(communicator);

    [myClass shutdown];

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status;
    id<ICECommunicator> communicator;

    @try
    {
        ICEInitializationData* initData = [[ICEInitializationData alloc] init];
        [initData setProperties:[ICEUtil createProperties:&argc argv:argv]];
        communicator = [ICEUtil createCommunicator:&argc argv:argv initData:initData];
        status = run(argc, argv, communicator, initData);
    }
    @catch(ICEException* ex)
    {
        NSLog(@"%@", ex);
        status = EXIT_FAILURE;
    }

    if(communicator)
    {
        @try
        {
            [communicator destroy];
        }
        @catch(ICEException* ex)
        {
            NSLog(@"%@", ex);
            status = EXIT_FAILURE;
        }
    }

    [pool release];
    return status;
}
