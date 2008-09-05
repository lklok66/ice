// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestI.h>

#import <Foundation/NSAutoreleasePool.h>

// @interface MyObjectFactory : ICEObjectFactory
// {
// public:

//     id<ICEObject> create(string* type)
//     {
//         if(type == @"TestI")
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
//     [communicator addObjectFactory:factory XXX:@"TestI"];
//     [communicator addObjectFactory:factory XXX:@"TestJ"];
//     [communicator addObjectFactory:factory XXX:@"TestH"];

    [[communicator getProperties] setProperty:@"TestAdapter.Endpoints" value:@"default -p 12010 -t 10000"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAdapter"];
    ICEObject* initial = [[[InitialI alloc] init] autorelease];
    [adapter add:initial identity:[communicator stringToIdentity:@"initial"]];
//    ICEObject* uoet = [[[UnexpectedObjectExceptionTestI alloc] init] autorelease];
//    [adapter add:uoet identity:[communicator stringToIdentity:@"uoet"]];
    [adapter activate];
    [communicator waitForShutdown];
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
