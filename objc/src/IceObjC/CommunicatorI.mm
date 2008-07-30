// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/CommunicatorI.h>

#include <Ice/Initialize.h>
#include <Ice/Communicator.h>

@implementation CommunicatorI

+ (Communicator*)initialize
{
    Ice::CommunicatorPtr communicator = Ice::initialize();
    return [[CommunicatorI alloc] initWithCommunicator:communicator];
}

- (Communicator*)initWithCommunicator:(const Ice::CommunicatorPtr&)communicator
{
    _communicator = communicator.get();
    _communicator->__incRef();
    return self;
}

- (void)dealloc
{
    _communicator->__decRef();
    _communicator = 0;
    [super dealloc];
}

- (void)shutdown
{
    _communicator->shutdown();
}

@end
