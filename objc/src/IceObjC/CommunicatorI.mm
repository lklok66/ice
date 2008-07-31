// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/CommunicatorI.h>

#import <IceObjC/PropertiesI.h>

#include <assert.h>

@implementation Ice_Communicator

-(Ice_Properties*) getProperties
{
    assert(false);
}

-(void) shutdown
{ 
    assert(false);
} 

-(void) destroy
{ 
    assert(false);
} 

@end

@implementation Ice_CommunicatorI

-(Ice_Communicator*) initWithCommunicator:(const Ice::CommunicatorPtr&)arg
{
    communicator = arg.get();
    communicator->__incRef();
    return self;
}

-(void) dealloc
{
    communicator->__decRef();
    communicator = 0;
    [super dealloc];
}

-(Ice_Properties*) getProperties
{
    return [[Ice_PropertiesI alloc] initWithProperties:communicator->getProperties()];
}

-(void) shutdown
{
    communicator->shutdown();
}

-(void) destroy
{
    communicator->destroy();
}

@end
