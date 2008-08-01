// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSObjCRuntime.h>

#import <IceObjC/IceObjC.h>

int
main(int argc, char* argv[])
{
    Ice_InitializationData* initData = [[Ice_InitializationData alloc] init];
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    Ice_Communicator* communicator = [Ice_Communicator create:&argc argv:argv initData:initData];
    Ice_ObjectPrx* proxy = [communicator stringToProxy:@"hello:tcp -p 10000"];
    @try
    {
        [proxy ice_ping];
    }
    @catch(Ice_LocalException* ex)
    {
        NSLog(@"%@", ex);
    }
    [initData release];
    [communicator destroy];
    [pool release];
    return 0;
}
