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
    @private
        NSString *reason_;
}

@property(nonatomic, retain) NSString *reason_;

-(NSString *) ice_name;
-(id) init:(NSString *)reason;
+(id) invalidMessageException:(NSString *)reason;
+(id) invalidMessageException;
-(void) copy__:(ChatInvalidMessageException *)copy_;
-(void) dealloc;
-(void) write__:(id<ICEOutputStream>)stream;
-(void) read__:(id<ICEInputStream>)stream readTypeId:(BOOL)rid_;
@end
