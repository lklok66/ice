// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

//
// Forward declarations.
//
@class ICEObject;
@class ICECurrent;
@class ICEInputStream;
@class ICEOutputStream;

@protocol ICEObject
-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current;
-(void) ice_ping:(ICECurrent*)current;
-(NSString*) ice_id:(ICECurrent*)current;
-(NSArray*) ice_ids:(ICECurrent*)current;
+(const char*) ice_staticId;
@end

@interface ICEObject : NSObject<ICEObject>
{
    void* object__;
}
-(ICEOutputStream*) createOutputStream__:(ICECurrent*)current;
-(BOOL) dispatch__:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream**)os;
-(void) write__:(ICEOutputStream*)os;
-(void) read__:(ICEInputStream*)is readTypeId:(BOOL)rid;
@end
