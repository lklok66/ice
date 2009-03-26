// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <TestCommon.h>
#import <Test.h>

#import <Foundation/Foundation.h>

@interface Callback : NSObject
{
    BOOL called;
    NSCondition* cond;
    ICEInt pid;
}
-(BOOL) check;
-(void) called;
-(ICEInt) pid;
@end

@implementation Callback
-(id) init
{
    if(![super init])
    {
        return nil;
    }
    cond = [[NSCondition alloc] init];
    return self;
}
-(void) dealloc
{
    [cond release];
    [super dealloc];
}
-(BOOL) check
{
    [cond lock];
    while(!called)
    {
        if(![cond waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:50]])
        {
            return NO;
        }
    }
    called = NO;
    [cond unlock];
    return YES;
}
-(void) called
{
    [cond lock];
    called = YES;
    [cond signal];
    [cond unlock];
}
-(void) pidResponse:(ICEInt)p
{
    pid = p;
    [self called];
}

-(void) pidException:(ICEException*)ex
{
    test(NO);
}
-(ICEInt) pid
{
    return pid;
}
-(void) shutdownResponse
{
    [self called];
}

-(void) shutdownException:(ICEException*)ex
{
    test(NO);
}
-(void) abortResponse
{
    test(NO);
}
-(void) abortException:(ICEException*)ex
{
    @try
    {
        @throw ex;
    }
    @catch(ICEConnectionLostException*)
    {
    }
    @catch(ICEConnectFailedException*)
    {
    }
    @catch(ICEException* ex)
    {
        NSLog(@"%@", ex);
        test(NO);
    }
    [self called];
}
-(void) idempotentAbortResponse
{
    test(NO);
}
-(void) idempotentAbortException:(ICEException*)ex
{
    @try
    {
        @throw ex;
    }
    @catch(ICEConnectionLostException*)
    {
    }
    @catch(ICEConnectFailedException*)
    {
    }
    @catch(ICEException* ex)
    {
        NSLog(@"%@", ex);
        test(NO);
    }
    [self called];
}
@end

void
allTests(id<ICECommunicator> communicator, NSArray* ports)
{
    tprintf("testing stringToProxy... ");
    NSString* ref = @"test";
    for(NSString* p in ports)
    {
        ref = [ref stringByAppendingString:@":default -t 60000 -p "];
        ref = [ref stringByAppendingString:p];
    }
    id<ICEObjectPrx> base = [communicator stringToProxy:ref];
    test(base);
    tprintf("ok\n");

    tprintf("testing checked cast... ");
    id<TestTestIntfPrx> obj = [TestTestIntfPrx checkedCast:base];
    test(obj);
    test([obj isEqual:base]);
    tprintf("ok\n");

    int oldPid = 0;
    BOOL ami = NO;
    unsigned int i, j;
    for(i = 1, j = 0; i <= [ports count]; ++i, ++j)
    {
        if(j > 3)
        {
            j = 0;
            ami = !ami;
        }

        if(!ami)
        {
            tprintf("testing server %d... ", i);
            int pid = [obj pid];
            test(pid != oldPid);
            tprintf("ok\n");
            oldPid = pid;
        }
        else
        {
            tprintf("testing server %d with AMI... ", i);
            Callback* cb = [[Callback alloc] init];
            [obj pid_async:cb response:@selector(pidResponse:) exception:@selector(pidException:)];
            test([cb check]);
            int pid = [cb pid];
            test(pid != oldPid);
            oldPid = pid;
            [cb release];
            tprintf("ok\n");
        }

        if(j == 0)
        {
            if(!ami)
            {
                tprintf("shutting down server %d... ", i);
                [obj shutdown];
                tprintf("ok\n");
            }
            else
            {
                tprintf("shutting down server %d with AMI... ", i);
                Callback* cb = [[Callback alloc] init];
                [obj shutdown_async:cb response:@selector(shutdownResponse) exception:@selector(shutdownException:)];
                test([cb check]);
                [cb release];
                tprintf("ok\n");
            }
        }
        else if(j == 1 || i + 1 > [ports count])
        {
            if(!ami)
            {
                tprintf("aborting server %d... ", i);
                @try
                {
                    [obj abort];
                    test(NO);
                }
                @catch(ICEConnectionLostException*)
                {
                    tprintf("ok\n");
                }
                @catch(ICEConnectFailedException*)
                {
                    tprintf("ok\n");
                }
            }
            else
            {
                tprintf("aborting server %d with AMI... ", i);
                Callback* cb = [[Callback alloc] init];
                [obj abort_async:cb response:@selector(abortResponse) exception:@selector(abortException:)];
                test([cb check]);
                [cb release];
                tprintf("ok\n");
            }
        }
        else if(j == 2 || j == 3)
        {
            if(!ami)
            {
                tprintf("aborting server %d and #%d with idempotent call... ", i, i + 1);
                @try
                {
                    [obj idempotentAbort];
                    test(NO);
                }
                @catch(ICEConnectionLostException*)
                {
                    tprintf("ok\n");
                }
                @catch(ICEConnectFailedException*)
                {
                    tprintf("ok\n");
                }
            }
            else
            {
                tprintf("aborting server %d and #%d with idempotent AMI call... ", i, i + 1);
                Callback* cb = [[Callback alloc] init];
                [obj idempotentAbort_async:cb response:@selector(idempotentAbortResponse) exception:@selector(idempotentAbortException:)];
                test([cb check]);
                [cb release];
                tprintf("ok\n");
            }

            ++i;
        }
        else
        {
            test(NO);
        }
    }

    tprintf("testing whether all servers are gone... ");
    @try
    {
        [obj ice_ping];
        test(NO);
    }
    @catch(ICELocalException*)
    {
        tprintf("ok\n");
    }
}

