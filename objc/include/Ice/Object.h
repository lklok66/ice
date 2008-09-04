// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

#include <Ice/Current.h>

//
// Forward declarations.
//
@class ICEObject;
@protocol ICEInputStream;
@protocol ICEOutputStream;

#if defined(__cplusplus)
extern "C"
#endif
int ICELookupString(const char**, size_t, const char*);

@protocol ICEObject <NSObject>
-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current;
-(void) ice_ping:(ICECurrent*)current;
-(NSString*) ice_id:(ICECurrent*)current;
-(NSArray*) ice_ids:(ICECurrent*)current;
+(NSString*) ice_staticId;
-(ICEInt) ice_hash;
-(id<ICEObject>) ice_clone;
@end

@interface ICEObject : NSObject<ICEObject>
{
    void* object__;
}
+(const char**) staticIds__:(int*)count idIndex:(int*)idx;
-(void) checkModeAndSelector__:(ICEOperationMode)expected selector:(SEL)sel current:(ICECurrent*)current;
+(BOOL) ice_isA___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
+(BOOL) ice_ping___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
+(BOOL) ice_id___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
+(BOOL) ice_ids___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(void) write__:(id<ICEOutputStream>)os;
-(void) read__:(id<ICEInputStream>)is readTypeId:(BOOL)rid;
@end
