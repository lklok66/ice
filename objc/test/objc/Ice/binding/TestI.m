// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <binding/TestI.h>

@implementation RemoteCommunicatorI
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    nextPort_ = 10001;
    return self;
}


-(id<TestBindingRemoteObjectAdapterPrx>) createObjectAdapter:(NSMutableString*)name endpoints:(NSMutableString*)endpts
                                              current:(ICECurrent*)current
{
    id<ICECommunicator> com = [current.adapter getCommunicator];
    [[com getProperties] setProperty:[name stringByAppendingString:@".ThreadPool.Size"] value:@"1"];
    id<ICEObjectAdapter> adapter = [com createObjectAdapterWithEndpoints:name endpoints:endpts];
    RemoteObjectAdapterI* remote = [[[RemoteObjectAdapterI alloc] initWithAdapter:adapter] autorelease];
    return [TestBindingRemoteObjectAdapterPrx uncheckedCast:[current.adapter addWithUUID:remote]];
}

-(void) deactivateObjectAdapter:(id<TestBindingRemoteObjectAdapterPrx>)adapter current:(ICECurrent*)current
{
    [adapter deactivate]; // Collocated call
}

-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}
@end

@implementation RemoteObjectAdapterI
-(id) initWithAdapter:(id<ICEObjectAdapter>)adapter
{
    if(![super init])
    {
        return nil;
    }
    adapter_ = [adapter retain];
    testIntf_ = [TestBindingTestIntfPrx uncheckedCast:[adapter_ add:[[[TestBindingI alloc] init] autorelease]
                                                    identity:[[adapter_ getCommunicator] stringToIdentity:@"test"]]];
    [testIntf_ retain];
    [adapter_ activate];
    return self;
}
-(void) dealloc
{
    [testIntf_ release];
    [adapter_ release];
    [super dealloc];
}
-(id<TestBindingTestIntfPrx>) getTestIntf:(ICECurrent*)current
{
    return testIntf_;
}

-(void) deactivate:(ICECurrent*)current
{
    @try
    {
        [adapter_ destroy];
    }
    @catch(ICEObjectAdapterDeactivatedException*)
    {
    }
}
@end

@implementation TestBindingI
-(NSString*) getAdapterName:(ICECurrent*)current
{
    return [current.adapter getName];
}
@end
