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
    id<Initial> initial = [[InitialI alloc] init](adapter);
    [adapter add:initial XXX:[communicator stringToIdentity:@"initial"]];
-(InitialPrx) allTests:(id<ICECommunicator>)XXX XXX:(bool)XXX;
    allTests(communicator, YES);
    // We must call shutdown even in the collocated case for cyclic dependency cleanup
    [initial shutdown:ICECurrent(]);
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
