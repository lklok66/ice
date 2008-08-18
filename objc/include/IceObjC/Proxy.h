// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

#import <IceObjC/Current.h> // For ICEOperationMode

//
// Forward declarations.
//
@class ICEObjectPrx;
@protocol ICEOutputStream;
@protocol ICEInputStream;

@protocol ICEObjectPrx <NSObject>
-(NSString*) ice_id;
-(NSArray*) ice_ids;
-(BOOL) ice_isA:(NSString*)typeId;
-(void) ice_ping;
@end

@interface ICEObjectPrx : NSObject<ICEObjectPrx>
{
    void* objectPrx__;
}
+(id) uncheckedCast:(id<ICEObjectPrx>)proxy;
+(id) checkedCast:(id<ICEObjectPrx>)proxy;
+(NSString*) ice_staticId;

+(Protocol*) protocol__;
-(id<ICEOutputStream>) createOutputStream__;
-(void) invoke__:(NSString*)operation mode:(ICEOperationMode)mode os:(id<ICEOutputStream>)os 
              is:(id<ICEInputStream>*)is context:(ICEContext*)context;

@end
