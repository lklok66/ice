// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSString.h>

#import <IceObjC/CommunicatorI.h>

#import <IceObjC/PropertiesI.h>
#import <IceObjC/ProxyI.h>

#include <assert.h>

@implementation Ice_Communicator

-(Ice_ObjectPrx*) stringToProxy:(NSString*)str
{
    assert(false);
}

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
    if(![super init])
    {
        return nil;
    }
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

-(Ice_ObjectPrx*) stringToProxy:(NSString*)str
{
    return [[[Ice_ObjectPrxI alloc] initWithObjectPrx:communicator->stringToProxy([str UTF8String])] autorelease];
}

-(Ice_Properties*) getProperties
{
    return [[[Ice_PropertiesI alloc] initWithProperties:communicator->getProperties()] autorelease];
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
