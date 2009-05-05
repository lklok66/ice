// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LoggingDelegate.h>

@implementation LogEntry

@synthesize type;
@synthesize timestamp;
@synthesize message;
@synthesize category;

-(id)init:(LogEntryType)t message:(NSString*)msg category:(NSString*)cat
{
    if((self = [super init]))
    {
        type = t;
        timestamp = [[NSDate date] retain];
        message = [msg retain];
        category = [cat retain];
    }
    return self;
}

+(id)logEntryPrint:(NSString*)message
{
    return [[[LogEntry alloc] init:LogEntryTypePrint message:message category:0] autorelease];
}

+(id)logEntryTrace:(NSString*)message category:(NSString*)category
{
    return [[[LogEntry alloc] init:LogEntryTypeTrace message:message category:category] autorelease];
}

+(id)logEntryWarning:(NSString*)message
{
    return [[[LogEntry alloc] init:LogEntryTypeWarning message:message category:0] autorelease];
}

+(id)logEntryError:(NSString*)message
{
    return [[[LogEntry alloc] init:LogEntryTypeError message:message category:0] autorelease];
}

-(void)dealloc
{
    [message release];
    [category release];
    [timestamp release];
    [super dealloc];
}

@end