// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
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
    id<ICELogger> logger_;
    id<DemoSessionPrx> session_;
    NSCondition* cond_;

    long timeout_;
}

-(id)initWithLogger:(id<ICELogger>) logger timeout:(long)timeout session:(id<DemoSessionPrx>)session;
+(id)sessionRefreshThreadWithLogger:(id<ICELogger>)logger timeout:(long)timeout session:(id<DemoSessionPrx>)session;
-(void)main;

-(void)dealloc;
@end

@implementation SessionRefreshThread

-(id)initWithLogger:(id<ICELogger>)logger timeout:(long)timeout session:(id<DemoSessionPrx>)session
{
    if(![super init])
    {
        return nil;
    }

    logger_ = [logger retain];
    session_ = [session retain];
    cond_ = [[NSCondition alloc] init];

    timeout_ = timeout;

    return self;
} 

+(id)sessionRefreshThreadWithLogger:(id<ICELogger>)logger timeout:(long)timeout session:(id<DemoSessionPrx>)session
{
    return [[[SessionRefreshThread alloc] initWithLogger:logger timeout:timeout session:session] autorelease];
}

-(void)main
{
    [cond_ lock];
    @try
    {
        while(!self.isCancelled)
        {
            NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
            [cond_ waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:timeout_]];
            if(!self.isCancelled)
            {
                @try
                {
                    [session_ refresh];
                }
                @catch(ICELocalException* ex)
                {
                    NSString* warning = [NSString stringWithFormat:@"SessionRefreshThread: %@", [ex description]];
                    [logger_ warning:warning];
                    [self cancel];
                }
            }
            [pool release];
        }
    }
    @finally
    {
        [cond_ unlock];
    }
}

-(void)cancel
{
    [super cancel];
    [cond_ lock];
    @try
    {
        [cond_ signal];
    }
    @finally
    {
        [cond_ unlock];
    }
}

-(void)dealloc
{
    [logger_ release];
    [session_ release];
    [cond_ release];
    [super dealloc];
}
@end

int
runParser(int argc, char* argv[], id<ICECommunicator> communicator)
{
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
