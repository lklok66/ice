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

int
main(int argc, const char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    Ice_Communicator* communicator = [Ice_Communicator create];
    [communicator destroy];
    [pool release];
    return 0;
}
