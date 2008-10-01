// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Library.h>
#import <Session.h>
#import <Parser.h>
#import <stdio.h>

#import <Foundation/NSThread.h>
#import <Foundation/NSLock.h>
#import <Foundation/NSAutoreleasePool.h>

@interface SessionRefreshThread : NSThread
{
@private
    id<ICELogger> logger;
    id<DemoSessionPrx> session;
    NSCondition* cond;

    long timeout;
}

-(id)initWithLogger:(id<ICELogger>) logger timeout:(long)timeout session:(id<DemoSessionPrx>)session;
+(id)sessionRefreshThreadWithLogger:(id<ICELogger>)logger timeout:(long)timeout session:(id<DemoSessionPrx>)session;
@end

@interface SessionRefreshThread()
@property (nonatomic, retain) id<ICELogger> logger;
@property (nonatomic, retain) id<DemoSessionPrx> session;
@property (nonatomic, retain) NSCondition* cond;
@end

@implementation SessionRefreshThread

@synthesize logger;
@synthesize session;
@synthesize cond;

-(id)initWithLogger:(id<ICELogger>)l timeout:(long)t session:(id<DemoSessionPrx>)s
{
    if((self = [super init]))
    {
        self.logger = l;
        self.session = s;
        self.cond = [[[NSCondition alloc] init] autorelease];
        timeout = t;
    }
    return self;
} 

+(id)sessionRefreshThreadWithLogger:(id<ICELogger>)logger timeout:(long)timeout session:(id<DemoSessionPrx>)session
{
    return [[[SessionRefreshThread alloc] initWithLogger:logger timeout:timeout session:session] autorelease];
}

-(void)main
{
    [cond lock];
    @try
    {
        while(!self.isCancelled)
        {
            NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
            [cond waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:timeout]];
            if(!self.isCancelled)
            {
                @try
                {
                    [session refresh];
                }
                @catch(ICELocalException* ex)
                {
                    NSString* warning = [NSString stringWithFormat:@"SessionRefreshThread: %@", [ex description]];
                    [logger warning:warning];
                    [self cancel];
                }
            }
            [pool release];
        }
    }
    @finally
    {
        [cond unlock];
    }
}

-(void)cancel
{
    [super cancel];
    [cond lock];
    @try
    {
        [cond signal];
    }
    @finally
    {
        [cond unlock];
    }
}

-(void)dealloc
{
    [logger release];
    [session release];
    [cond release];
    [super dealloc];
}
@end

int
runParser(int argc, char* argv[], id<ICECommunicator> communicator)
{
    // For this demo we don't need to retain the below objects since
    // the autorelease pool in scope is not released until main
    // terminates.
    id<DemoSessionFactoryPrx> factory = [DemoSessionFactoryPrx checkedCast:[
            communicator propertyToProxy:@"SessionFactory.Proxy"] ];
    if(factory == nil)
    {
        fprintf(stderr, "%s: invalid object reference", argv[0]);
        return 1;
    }

    id<DemoSessionPrx> session = [factory create];

    SessionRefreshThread* refresh = [SessionRefreshThread sessionRefreshThreadWithLogger:[communicator getLogger]
                                                          timeout:[factory getSessionTimeout]/2 session:session];
    [refresh start];

    id<DemoLibraryPrx> library = [session getLibrary];

    Parser* parser = [Parser parserWithLibrary:library];

    int rc = [parser parse];

    [refresh cancel];

    // No join.
    while(!refresh.isFinished)
    {
        [NSThread sleepForTimeInterval:0.1];
    }

    [session destroy];

    return rc;
}
