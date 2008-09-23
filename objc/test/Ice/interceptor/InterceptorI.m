// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <InterceptorI.h>
#import <Test.h>
#import <TestCommon.h>

@implementation InterceptorI

-(id) init:(ICEObject*) servant_
{
    if (![super init])
    {
        return nil;
    }
    servant = [servant_ retain];
    return self;
}

-(void) dealloc
{
    [servant release];
    [super dealloc];
}

 
-(BOOL) dispatch:(id<ICERequest>) request
{
    ICECurrent* current = [request getCurrent];
    lastOperation = [[current operation] retain];

    if([lastOperation isEqualToString:@"addWithRetry"])
    {
        int i = 0;
        for(i = 0; i < 10; ++i)
        {
            @try
            {
                [servant ice_dispatch:request];
                test(NO);
            }
            @catch(TestRetryException*)
            {
                tprintf("caught retry exception\n");
                //
                // Expected, retry
                //
            }
            @catch(ICELocalException*) 
            {
                tprintf("caught local exception\n");
            }
        }
        
        [(NSMutableDictionary*)current.ctx setObject:@"no" forKey:@"retry"];
    }
      
    lastStatus = [servant ice_dispatch:request];
    return lastStatus;
}
     
-(BOOL) getLastStatus
{
    return lastStatus;
}

-(NSString*) getLastOperation
{
    return lastOperation;
}
 
-(void) clear
{
    lastStatus = NO;
    lastOperation = nil;
}

@end
