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
#import <IceObjC/StreamI.h>

#include <assert.h>

@implementation Ice_Communicator (Internal)

-(Ice_Communicator*) initWithCommunicator:(Ice::Communicator*)arg
{
    if(![super init])
    {
        return nil;
    }
    communicator__ = arg;
    ((Ice::Communicator*)communicator__)->__incRef();
    return self;
}

-(Ice::Communicator*) communicator__
{
    return (Ice::Communicator*)communicator__;
}

-(void) dealloc
{
    ((Ice::Communicator*)communicator__)->__decRef();
    communicator__ = 0;
    [super dealloc];
}

@end

@implementation Ice_Communicator

-(Ice_ObjectPrx*) stringToProxy:(NSString*)str
{
    Ice::ObjectPrx proxy = ((Ice::Communicator*)communicator__)->stringToProxy([str UTF8String]);
    return [[[Ice_ObjectPrx alloc] initWithObjectPrx__:proxy.get()] autorelease];
}

-(Ice_Properties*) getProperties
{
    Ice::PropertiesPtr properties = ((Ice::Communicator*)communicator__)->getProperties();
    return [[[Ice_Properties alloc] initWithProperties:properties.get()] autorelease];
}

-(void) shutdown
{
    ((Ice::Communicator*)communicator__)->shutdown();
}

-(void) destroy
{
    ((Ice::Communicator*)communicator__)->destroy();
}

@end
