// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0
// Generated from file `Chat.ice'

#import <Ice/LocalException.h>
#import <Ice/Stream.h>
#import <Chat.h>


@implementation ChatInvalidMessageException

@synthesize reason_;

-(NSString *) ice_name
{
    return @"Chat::InvalidMessageException";
}

-(id) init:(NSString *)reason_p
{
    if(![super initWithName:[self ice_name] reason:nil userInfo:nil])
    {
        return nil;
    }
    reason_ = [reason_p retain];
    return self;
}

+(id) invalidMessageException:(NSString *)reason_p
{
    ChatInvalidMessageException *s__ = [((ChatInvalidMessageException *)[ChatInvalidMessageException alloc]) init:reason_p];
    [s__ autorelease];
    return s__;
}

+(id) invalidMessageException
{
    ChatInvalidMessageException *s__ = [[ChatInvalidMessageException alloc] init];
    [s__ autorelease];
    return s__;
}

-(void) copy__:(ChatInvalidMessageException*)copy_
{
    [super copy__:copy_];
    copy_->reason_ = [reason_ copy];
}

-(void) dealloc;
{
    [reason_ release];
    [super dealloc];
}

-(void) write__:(id<ICEOutputStream>)os_
{
    [os_ writeString:@"::Chat::InvalidMessageException"];
    [os_ startSlice];
    [os_ writeString:self->reason_];
    [os_ endSlice];
}

-(void) read__:(id<ICEInputStream>)is_ readTypeId:(BOOL)rid_
{
    if(rid_)
    {
        [[is_ readString] release];
    }
    [is_ startSlice];
    self->reason_ = [is_ readString];
    [is_ endSlice];
}
@end
