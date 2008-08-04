// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Communicator.h>

#include <Ice/Communicator.h>

@interface Ice_Communicator (Internal)
-(Ice_Communicator*)initWithCommunicator:(const Ice::CommunicatorPtr&)arg;
-(Ice::Communicator*)communicator__;
+(Ice_Communicator*)communicatorWithCommunicator:(const Ice::CommunicatorPtr&)arg;
@end

