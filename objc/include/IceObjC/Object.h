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
@end

@interface ICEObject : NSObject<ICEObject>
{
    void* object__;
}
-(ICEOutputStream*) createOutputStream__:(ICECurrent*)current;
-(BOOL) dispatch__:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream**)os;
@end
