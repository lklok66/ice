// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>
#import <Foundation/NSString.h>

@class Ice_Object;

@protocol Ice_Object
-(BOOL) ice_isA:(NSString*)typeId;
-(void) ice_ping;
@end

@interface Ice_Object : NSObject<Ice_Object>
{
    void* object__;
}
@end
