// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Initialize.h>
#import <IceObjC/PropertiesI.h>
#import <IceObjC/CommunicatorI.h>

#include <Ice/Initialize.h>

@implementation Ice_InitializationData

@synthesize properties;

@end

@implementation Ice_Properties (Initialize)

+(Ice_Properties*) create
{ 
    return [self create:nil argv:nil];
}

+(Ice_Properties*) create:(int*)argc argv:(char*[])argv
{ 
    Ice::PropertiesPtr properties;
    if(argc != nil && argv != nil)
    {
        properties = Ice::createProperties(*argc, argv);
    }
    else
    {
        properties = Ice::createProperties();
    }
    return [[[Ice_PropertiesI alloc] initWithProperties:properties] autorelease];
}

@end

@implementation Ice_Communicator (Initialize)

+(Ice_Communicator*) create
{ 
    return [self create:nil argv:nil initData:nil];
}

+(Ice_Communicator*) create:(Ice_InitializationData*)initData
{ 
    return [self create:nil argv:nil initData:initData];
}

+(Ice_Communicator*) create:(int*)argc argv:(char*[])argv
{ 
    return [self create:argc argv:argv initData:nil];
}

+(Ice_Communicator*) create:(int*)argc argv:(char*[])argv initData:(Ice_InitializationData*)initData
{ 
    Ice::InitializationData data; // TODO: convert initData to data
    Ice::CommunicatorPtr communicator;
    if(argc != nil && argv != nil)
    {
        communicator = Ice::initialize(*argc, argv, data);
    }
    else
    {
        communicator = Ice::initialize(data);
    }
    return [[[Ice_CommunicatorI alloc] initWithCommunicator:communicator] autorelease];
}

@end
