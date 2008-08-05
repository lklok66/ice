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

@class ICEObject;

@protocol ICEObject
-(BOOL) ice_isA:(NSString*)typeId;
-(void) ice_ping;
@end

@interface ICEObject : NSObject<ICEObject>
{
    void* object__;
}
@end
