// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/InitializeI.h>
#import <Ice/PropertiesI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/StreamI.h>
#import <Ice/LoggerI.h>
#import <Ice/DispatcherI.h>
#import <Ice/Util.h>
#import <Ice/LocalException.h>

#include <IceCpp/Initialize.h>
#include <IceUtilCpp/UUID.h>

#include <Availability.h>

#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSThread.h>

#ifdef ICE_NO_KQUEUE
#  define ICE_USE_CFSTREAM 1
#endif

namespace IceObjC
{

class ThreadNotification : public Ice::ThreadNotification, public IceUtil::Mutex
{
public:

    ThreadNotification()
    {
    }

    virtual void start()
    {
        Lock sync(*this);
        _pools.insert(std::make_pair(IceUtil::ThreadControl().id(), [[NSAutoreleasePool alloc] init]));
    }

    virtual void stop()
    {
        Lock sync(*this);
        std::map<IceUtil::ThreadControl::ID, NSAutoreleasePool*>::iterator p =
            _pools.find(IceUtil::ThreadControl().id());
        [p->second release];
        _pools.erase(p);
    }

private:

    std::map<IceUtil::ThreadControl::ID, NSAutoreleasePool*> _pools;
};

};

@implementation ICEInitializationData (ICEInternal)
-(Ice::InitializationData)initializationData
{
    Ice::InitializationData data;
    data.properties = [(ICEProperties*)properties properties];
    data.logger = [ICELogger loggerWithLogger:logger];
    if(dispatcher)
    {
        data.dispatcher = [ICEDispatcher dispatcherWithDispatcher:dispatcher];
    }
    return data;
}
@end

@implementation ICEInitializationData

@synthesize properties;
@synthesize logger;
@synthesize dispatcher;
@synthesize prefixTable__;

-(id) init:(id<ICEProperties>)props logger:(id<ICELogger>)log
dispatcher:(void(^)(id<ICEDispatcherCall>, id<ICEConnection>))d;
{
    if(![super init])
    {
        return nil;
    }
    properties = [props retain];
    logger = [log retain];
    dispatcher = [d copy];
    return self;
}

+(id) initializationData;
{
   ICEInitializationData *s = [[ICEInitializationData alloc] init];
   [s autorelease];
   return s;
}

+(id) initializationData:(id<ICEProperties>)p logger:(id<ICELogger>)l
              dispatcher:(void(^)(id<ICEDispatcherCall>, id<ICEConnection>))d;
{
   return [[((ICEInitializationData *)[ICEInitializationData alloc]) init:p logger:l dispatcher:d] autorelease];
}

-(id) copyWithZone:(NSZone *)zone
{
    ICEInitializationData *copy = [ICEInitializationData allocWithZone:zone];
    copy->properties = [properties retain];
    copy->logger = [logger retain];
    copy->dispatcher = [dispatcher copy];
    copy->prefixTable__ = [prefixTable__ retain];
    return copy;
}

-(NSUInteger) hash;
{
    NSUInteger h = 0;
    h = (h << 1 ^ [properties hash]);
    h = (h << 1 ^ [logger hash]);
    h = (h << 1 ^ [prefixTable__ hash]);
    return h;
}

-(BOOL) isEqual:(id)anObject;
{
    if(self == anObject)
    {
        return YES;
    }
    if(!anObject || ![anObject isKindOfClass:[self class]])
    {
        return NO;
    }
    ICEInitializationData * obj =(ICEInitializationData *)anObject;
    if(!properties)
    {
        if(obj->properties)
	{
	    return NO;
	}
    }
    else
    {
        if(![properties isEqual:obj->properties])
	{
	    return NO;
	}
    }
    if(!logger)
    {
        if(obj->logger)
	{
	    return NO;
	}
    }
    else
    {
        if(![logger isEqual:obj->logger])
	{
	    return NO;
	}
    }
    if(!dispatcher)
    {
        if(obj->dispatcher)
	{
	    return NO;
	}
    }
    else
    {
        if(dispatcher == obj->dispatcher)
	{
	    return NO;
	}
    }
    if(!prefixTable__)
    {
        if(obj->prefixTable__)
	{
	    return NO;
	}
    }
    else
    {
        if(![prefixTable__ isEqual:obj->prefixTable__])
	{
	    return NO;
	}
    }
    return YES;
}

-(void) dealloc;
{
    [properties release];
    [logger release];
    [dispatcher release];
    [prefixTable__ release];
    [super dealloc];
}
@end

@implementation ICEUtil

+(id<ICEProperties>) createProperties
{ 
    return [self createProperties:nil argv:nil];
}

+(id<ICEProperties>) createProperties:(int*)argc argv:(char*[])argv
{ 
    NSException* nsex = nil;
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
        return [ICEProperties wrapperWithCxxObject:properties.get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
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

    id<ICEProperties> properties = [initData properties];
    if(properties != nil && ![properties isKindOfClass:[ICEProperties class]])
    {
        @throw [ICEInitializationException initializationException:__FILE__ line:__LINE__ 
                                           reason_:@"properties were not created with createProperties"];
    }

    NSException* nsex = nil;
    try
    {
        Ice::InitializationData data;
        if(initData != nil)
        {
            data = [initData initializationData];
        }
        data.threadHook = new IceObjC::ThreadNotification();
        if(!data.properties)
        {
            data.properties = Ice::createProperties();
        }
        data.properties->setProperty("Ice.Default.CollocationOptimized", "0");

        if(argc != nil && argv != nil)
        {
            data.properties = createProperties(*argc, argv, data.properties);
        }

        Ice::CommunicatorPtr communicator;
        if(argc != nil && argv != nil)
        {
            communicator = Ice::initialize(*argc, argv, data);
        }
        else
        {
            communicator = Ice::initialize(data);
        }

        ICECommunicator* c = [ICECommunicator wrapperWithCxxObject:communicator.get()];
        [c setup:initData.prefixTable__];
        return c;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

+(id<ICEInputStream>) createInputStream:(id<ICECommunicator>)communicator data:(NSData*)data
{
    NSException* nsex = nil;
    try
    {
        Ice::CommunicatorPtr com = [(ICECommunicator*)communicator communicator];
        Ice::Byte* start = (Ice::Byte*)[data bytes];
        Ice::Byte* end = (Ice::Byte*)[data bytes] + [data length];
        Ice::InputStreamPtr is = Ice::createInputStream(com, std::make_pair(start, end));
        if(is)
        {
            return [ICEInputStream wrapperWithCxxObject:is.get()];
        }
        else
        {
            return nil;
        }
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

+(id<ICEOutputStream>) createOutputStream:(id<ICECommunicator>)communicator
{
    NSException* nsex = nil;
    try
    {
        Ice::CommunicatorPtr com = [(ICECommunicator*)communicator communicator];
        Ice::OutputStreamPtr os = Ice::createOutputStream(com);
        if(os)
        {
            return [ICEOutputStream wrapperWithCxxObject:os.get()];
        }
        else
        {
            return nil;
        }
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

+(NSString*) generateUUID
{
    return [NSString stringWithUTF8String:IceUtil::generateUUID().c_str()];
}

+(NSArray*)argsToStringSeq:(int)argc argv:(char*[])argv
{
    NSMutableArray* ns = [NSMutableArray array];
    int i;
    for(i = 0; i < argc; ++i)
    {
        [ns addObject:[NSString stringWithCString:argv[i] encoding:NSUTF8StringEncoding]];
    }
    return [[ns copy] autorelease];
}

+(void)stringSeqToArgs:(NSArray*)args argc:(int*)argc argv:(char*[])argv;
{
    //
    // Shift all elements in argv which are present in args to the
    // beginning of argv. We record the original value of argc so
    // that we can know later if we've shifted the array.
    //
    const int argcOrig = *argc;
    int i = 0;
    while(i < *argc)
    {
        BOOL found = NO;
        for(NSString* s in args)
        {
            if([s compare:[NSString stringWithCString:argv[i] encoding:NSUTF8StringEncoding]] == 0)
            {
                found = YES;
                break;
            }
        }
        if(!found)
        {
            int j;
            for(j = i; j < *argc - 1; j++)
            {
                argv[j] = argv[j + 1];
            }
            --(*argc);
        }
        else
        {
            ++i;
        }
    }

    //
    // Make sure that argv[*argc] == 0, the ISO C++ standard requires this.
    // We can only do this if we've shifted the array, otherwise argv[*argc]
    // may point to an invalid address.
    //
    if(argv && argcOrig != *argc)
    {
        argv[*argc] = 0;
    }
}

@end

