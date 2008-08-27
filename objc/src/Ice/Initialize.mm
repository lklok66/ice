// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/InitializeI.h>
#import <Ice/PropertiesI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/StreamI.h>
#import <Ice/LoggerI.h>
#import <Ice/Util.h>

#include <IceCpp/Initialize.h>

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
    data.properties = [(ICEProperties*)properties properties__];
    data.logger = [(ICELogger*)logger logger__];
    return data;
}
@end

@implementation ICEInitializationData

@synthesize properties;
@synthesize logger;

@end

@implementation ICEUtil

+(id<ICEProperties>) createProperties
{ 
    return [self createProperties:nil argv:nil];
}

+(id<ICEProperties>) createProperties:(int*)argc argv:(char*[])argv
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

+(id<ICECommunicator>) createCommunicator
{ 
    return [self createCommunicator:nil argv:nil initData:nil];
}

+(id<ICECommunicator>) createCommunicator:(ICEInitializationData*)initData
{ 
    return [self createCommunicator:nil argv:nil initData:initData];
}

+(id<ICECommunicator>) createCommunicator:(int*)argc argv:(char*[])argv
{ 
    return [self createCommunicator:argc argv:argv initData:nil];
}

+(id<ICECommunicator>) createCommunicator:(int*)argc argv:(char*[])argv initData:(ICEInitializationData*)initData
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
        [ICEInputStream installObjectFactory:communicator];
        return [ICECommunicator communicatorWithCommunicator:communicator];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

+(id<ICEInputStream>) createInputStream:(id<ICECommunicator>)communicator data:(NSData*)data
{
    try
    {
        Ice::CommunicatorPtr com = [(ICECommunicator*)communicator communicator__];
        Ice::Byte* start = (Ice::Byte*)[data bytes];
        Ice::Byte* end = (Ice::Byte*)[data bytes] + [data length];
        Ice::InputStreamPtr is = Ice::createInputStream(com, std::make_pair(start, end));
        if(is)
        {
            return [[[ICEInputStream alloc] initWithInputStream:is] autorelease];
        }
        else
        {
            return nil;
        }
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

+(id<ICEOutputStream>) createOutputStream:(id<ICECommunicator>)communicator
{
    try
    {
        Ice::CommunicatorPtr com = [(ICECommunicator*)communicator communicator__];
        Ice::OutputStreamPtr os = Ice::createOutputStream(com);
        if(os)
        {
            return [[[ICEOutputStream alloc] initWithOutputStream:os] autorelease];
        }
        else
        {
            return nil;
        }
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end
