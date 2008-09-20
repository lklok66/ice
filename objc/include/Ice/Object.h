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
int ICEInternalLookupString(NSString *[], size_t, NSString *);

@protocol ICERequest <NSObject>
-(ICECurrent*) getCurrent;
@end

@protocol ICEObject <NSObject>
-(BOOL) ice_isA:(NSString*)typeId;
-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current;
-(void) ice_ping;
-(void) ice_ping:(ICECurrent*)current;
-(NSString*) ice_id;
-(NSString*) ice_id:(ICECurrent*)current;
-(NSArray*) ice_ids;
-(NSArray*) ice_ids:(ICECurrent*)current;
+(NSString*) ice_staticId;
-(void) ice_preMarshal;
-(void) ice_postUnmarshal;
-(BOOL) ice_dispatch:(id<ICERequest>)request;
@end

@interface ICEObject : NSObject<ICEObject, NSCopying>
{
    void* object__;
}
+(NSString**) staticIds__:(int*)count idIndex:(int*)idx;
-(void) checkModeAndSelector__:(ICEOperationMode)expected selector:(SEL)sel current:(ICECurrent*)current;
+(BOOL) ice_isA___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
+(BOOL) ice_ping___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
+(BOOL) ice_id___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
+(BOOL) ice_ids___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(void) write__:(id<ICEOutputStream>)os;
-(void) read__:(id<ICEInputStream>)is readTypeId:(BOOL)rid;
-(void) copy__:(ICEObject*)copy_;
@end

@protocol ICEBlobject<ICEObject>
-(BOOL) ice_invoke:(NSData*)inParams outParams:(NSData**)outParams current:(ICECurrent*)current;
@end

@interface ICEBlobject : ICEObject
@end
