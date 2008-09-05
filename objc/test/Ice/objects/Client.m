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
#import <TestI.h>

#import <Foundation/NSAutoreleasePool.h>

// @interface MyObjectFactory : ICEObjectFactory
// {
// public:

//     id<ICEObject> create(string* type)
//     {
//         if(type == @"TestB")
//         {
//             return [[BI alloc] init];
//         }
//         else if(type == @"TestC")
//         {
//             return [[CI alloc] init];
//         }
//         else if(type == @"TestD")
//         {
//             return [[DI alloc] init];
//         }
//         else if(type == @"TestE")
//         {
//             return [[EI alloc] init];
//         }
//         else if(type == @"TestF")
//         {
//             return [[FI alloc] init];
//         }
//         else if(type == @"TestI")
//         {
//             return [[II alloc] init];
//         }
//         else if(type == @"TestJ")
//         {
//             return [[JI alloc] init];
//         }
//         else if(type == @"TestH")
//         {
//             return [[HI alloc] init];
//         }

//         assert(NO); // Should never be reached
//         return 0;
//     }

// -(void) destroy
//     {
//         // Nothing to do
//     }
// };

int
run(int argc, char* argv[], id<ICECommunicator> communicator)
{
//     id<ICEObjectFactory> factory = [[MyObjectFactory alloc] init];
//     [communicator addObjectFactory:factory XXX:@"TestB"];
//     [communicator addObjectFactory:factory XXX:@"TestC"];
//     [communicator addObjectFactory:factory XXX:@"TestD"];
//     [communicator addObjectFactory:factory XXX:@"TestE"];
//     [communicator addObjectFactory:factory XXX:@"TestF"];
//     [communicator addObjectFactory:factory XXX:@"TestI"];
//     [communicator addObjectFactory:factory XXX:@"TestJ"];
//     [communicator addObjectFactory:factory XXX:@"TestH"];

    id<TestInitialPrx> allTests(id<ICECommunicator>, bool);
    id<TestInitialPrx> initial = allTests(communicator, NO);
    [initial shutdown];
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
        communicator = [ICEUtil createCommunicator:&argc argv:argv];
        status = run(argc, argv, communicator);
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
