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


int
run(int argc, char* argv[], id<ICECommunicator> communicator)
{
    [[communicator getProperties] setProperty:@"TestAdapter.Endpoints" value:@"default -p 12010 -t 10000"];
    id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"TestAdapter"];
    id<ICEObject> d = [[DI alloc] init];
    [adapter add:d XXX:[communicator stringToIdentity:@"d"]];
    [adapter addFacet:d XXX:[communicator stringToIdentity:@"d"] XXX:@"facetABCD"];
    id<ICEObject> f = [[FI alloc] init];
    [adapter addFacet:f XXX:[communicator stringToIdentity:@"d"] XXX:@"facetEF"];
    id<ICEObject> h = [[HI alloc] init](communicator);
    [adapter addFacet:h XXX:[communicator stringToIdentity:@"d"] XXX:@"facetGH"];

-(GPrx) allTests:(id<ICECommunicator>)XXX;
    allTests(communicator);

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
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

    return status;
}
