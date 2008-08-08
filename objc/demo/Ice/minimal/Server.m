// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/IceObjC.h>
#import <IceObjC/LocalException.h>
#import <Foundation/NSAutoreleasePool.h>

#import <HelloI.h>

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = 0;
    ICECommunicator* communicator = nil;
    @try
    {
        communicator = [ICECommunicator create:&argc argv:argv];
        if(argc > 1)
        {
            NSLog(@"%s: too many arguments", argv[0]);
            return 1;
        }
        ICEObjectAdapter* adapter = [communicator createObjectAdapterWithEndpoints:@"Hello" endpoints:@"tcp -p 10000"];
        Hello* hello = [[HelloI alloc] init];
        [adapter add:hello identity:[communicator stringToIdentity:@"hello"]];
        [hello release];
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
