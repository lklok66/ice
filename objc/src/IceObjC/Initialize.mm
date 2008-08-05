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
#import <IceObjC/StreamI.h>

#include <Ice/Initialize.h>

#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSThread.h>

namespace IceObjC
{

class ThreadNotification : public Ice::ThreadNotification
{
public:

    ThreadNotification()
    {
    }

    virtual void start()
    {
        _pools.insert(std::make_pair(IceUtil::ThreadControl().id(), [[NSAutoreleasePool alloc] init]));
    }

    virtual void stop()
    {
        std::map<IceUtil::ThreadControl::ID, NSAutoreleasePool*>::iterator p =
            _pools.find(IceUtil::ThreadControl().id());
        [p->second release];
        _pools.erase(p);
    }

private:

    std::map<IceUtil::ThreadControl::ID, NSAutoreleasePool*> _pools;
};

};

@implementation ICEInitializationData

@synthesize properties;

@end

@implementation ICEProperties (Initialize)

+(ICEProperties*) create
{ 
    return [self create:nil argv:nil];
}

+(ICEProperties*) create:(int*)argc argv:(char*[])argv
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
    return [ICEProperties propertiesWithProperties:properties];
}

@end

@implementation ICECommunicator (Initialize)

+(ICECommunicator*) create
{ 
    return [self create:nil argv:nil initData:nil];
}

+(ICECommunicator*) create:(ICEInitializationData*)initData
{ 
    return [self create:nil argv:nil initData:initData];
}

+(ICECommunicator*) create:(int*)argc argv:(char*[])argv
{ 
    return [self create:argc argv:argv initData:nil];
}

+(ICECommunicator*) create:(int*)argc argv:(char*[])argv initData:(ICEInitializationData*)initData
{ 
    if(![NSThread isMultiThreaded])
    {
        [[[NSThread alloc] init] start]; // TODO: doesn't this leak?
    }

    Ice::InitializationData data; // TODO: convert initData to data
    data.threadHook = new IceObjC::ThreadNotification();
    Ice::CommunicatorPtr communicator;
    if(argc != nil && argv != nil)
    {
        communicator = Ice::initialize(*argc, argv, data);
    }
    else
    {
        communicator = Ice::initialize(data);
    }
    return [ICECommunicator communicatorWithCommunicator:communicator];
}

@end

@implementation ICEInputStream (Initialize)

+(ICEInputStream*) createInputStream:(ICECommunicator*)communicator buf:(unsigned char*)buf length:(int)length
{
    Ice::InputStreamPtr is = Ice::createInputStream([communicator communicator__], std::make_pair(buf, buf + length));
    return [[[ICEInputStream alloc] initWithInputStream:is] autorelease];
}

@end

@implementation ICEOutputStream (Initialize)

+(ICEOutputStream*) createOutputStream:(ICECommunicator*)communicator
{
    Ice::OutputStreamPtr os = Ice::createOutputStream([communicator communicator__]);
    return [[[ICEOutputStream alloc] initWithOutputStream:os] autorelease];
}

@end
