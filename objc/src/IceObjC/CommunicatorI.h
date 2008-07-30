// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Communicator.h>

#include <Ice/CommunicatorF.h>

@interface Ice_CommunicatorI : Ice_Communicator
{
    Ice::Communicator* _communicator;
}

-(Ice_Communicator*)initWithCommunicator:(const Ice::CommunicatorPtr&)communicator;
@end

