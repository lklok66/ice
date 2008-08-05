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
@class ICEOutputStream;
@class ICEInputStream;

@protocol ICEObjectPrx
-(BOOL) ice_isA:(NSString*)typeId;
-(void) ice_ping;
@end

@interface ICEObjectPrx : NSObject<ICEObjectPrx>
{
    void* objectPrx__;
}

+(id<ICEObjectPrx>) uncheckedCast__:(id<ICEObjectPrx>)proxy;
+(id<ICEObjectPrx>) checkedCast__:(id<ICEObjectPrx>)proxy protocol:(Protocol*)protocol sliceId:(NSString*)sliceId;

-(ICEOutputStream*) createOutputStream__;
-(BOOL) invoke__:(NSString*)operation mode:(ICEOperationMode)mode os:(ICEOutputStream*)os is:(ICEInputStream**)is;

@end
