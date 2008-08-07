// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/InitializeI.h>
#import <IceObjC/PropertiesI.h>
#import <IceObjC/CommunicatorI.h>
#import <IceObjC/StreamI.h>
#import <IceObjC/LoggerI.h>
#import <IceObjC/Util.h>

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

@implementation ICEInitializationData (Internal)
-(Ice::InitializationData)initializationData__
{
    Ice::InitializationData data;
    data.properties = [properties properties__];
    data.logger = [logger logger__];
    return data;
}
@end

@implementation ICEInitializationData

@synthesize properties;
@synthesize logger;

@end

@implementation ICEProperties (Initialize)

+(ICEProperties*) create
{ 
    return [self create:nil argv:nil];
}

+(ICEProperties*) create:(int*)argc argv:(char*[])argv
{ 
    try
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
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
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
    if(![NSThread isMultiThreaded]) // Ensure sure Cocoa is multithreaded.
    {
        NSThread* thread = [[NSThread alloc] init];
        [thread start];
        [thread release];
    }
    
    try
    {
        Ice::InitializationData data;
        if(initData != nil)
        {
            data = [initData initializationData__];
        }
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
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end

@implementation ICEInputStream (Initialize)

+(ICEInputStream*) createInputStream:(ICECommunicator*)communicator data:(NSData*)data
{
    try
    {
        Ice::Byte* start = (Ice::Byte*)[data bytes];
        Ice::Byte* end = (Ice::Byte*)[data bytes] + [data length];
        Ice::InputStreamPtr is = Ice::createInputStream([communicator communicator__], std::make_pair(start, end));
        return [[[ICEInputStream alloc] initWithInputStream:is] autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end

@implementation ICEOutputStream (Initialize)

+(ICEOutputStream*) createOutputStream:(ICECommunicator*)communicator
{
    try
    {
        Ice::OutputStreamPtr os = Ice::createOutputStream([communicator communicator__]);
        return [[[ICEOutputStream alloc] initWithOutputStream:os] autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end
