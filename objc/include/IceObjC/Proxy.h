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

@interface Ice_ObjectPrx : NSObject
{
    void* proxy__;
}

+(Ice_ObjectPrx*) uncheckedCast__:(Ice_ObjectPrx*)proxy;
+(Ice_ObjectPrx*) checkedCast__:(Ice_ObjectPrx*)proxy protocol:(Protocol*)protocol sliceId:(NSString*)sliceId;

-(Ice_OutputStream*) createOutputStream__;
-(BOOL) invoke__:(NSString*)operation mode:(Ice_OperationMode)mode os:(Ice_OutputStream*)os is:(Ice_InputStream**)is;

-(BOOL) ice_isA:(NSString*)typeId;
-(void) ice_ping;

@end
