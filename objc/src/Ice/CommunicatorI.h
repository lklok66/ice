// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Communicator.h>

#include <IceCpp/Communicator.h>

@interface ICECommunicator : NSObject<ICECommunicator>
{
    Ice::Communicator* communicator__;
}
-(ICECommunicator*)initWithCommunicator:(const Ice::CommunicatorPtr&)arg;
-(Ice::Communicator*)communicator__;
+(ICECommunicator*)communicatorWithCommunicator:(const Ice::CommunicatorPtr&)arg;
@end

