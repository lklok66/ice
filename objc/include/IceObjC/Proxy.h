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

#include <IceObjC/Current.h>
#include <IceObjC/Stream.h>

@class Ice_ObjectPrx;

@protocol Ice_ObjectPrx
-(BOOL) ice_isA:(NSString*)typeId;
-(void) ice_ping;
@end

@interface Ice_ObjectPrx : NSObject<Ice_ObjectPrx>
{
    void* objectPrx__;
}

+(id<Ice_ObjectPrx>) uncheckedCast__:(id<Ice_ObjectPrx>)proxy;
+(id<Ice_ObjectPrx>) checkedCast__:(id<Ice_ObjectPrx>)proxy protocol:(Protocol*)protocol sliceId:(NSString*)sliceId;

-(Ice_OutputStream*) createOutputStream__;
-(BOOL) invoke__:(NSString*)operation mode:(Ice_OperationMode)mode os:(Ice_OutputStream*)os is:(Ice_InputStream**)is;

@end
