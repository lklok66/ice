// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.0
// Generated from file `Chat.ice'

#import <Ice/Config.h>
#import <Ice/Proxy.h>
#import <Ice/Object.h>
#import <Ice/Current.h>
#import <Ice/Exception.h>
#import <Ice/Stream.h>

@interface ChatInvalidMessageException : ICEUserException
{
    NSString *reason_;
}

@property(nonatomic, retain) NSString *reason_;

-(NSString *) ice_name;
-(id) init:(NSString *)reason;
+(id) invalidMessageException:(NSString *)reason;
+(id) invalidMessageException;
// This class also overrides copyWithZone:.
// This class also overrides dealloc.
@end
