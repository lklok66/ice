// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/IceObjC.h>

#import <Foundation/NSAutoreleasePool.h>

#import <Hello.h>

int
main(int argc, char* argv[])
{
    NSString* s1 = nil;
    NSString* s2 = @"TEST";
    if(![s1 isEqual:s2])
    {
        NSLog(@"not equal");
    }

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
        id<HelloPrx> hello = [HelloPrx checkedCast:[communicator stringToProxy:@"hello:tcp -p 10000"]];
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
