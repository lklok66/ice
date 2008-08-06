// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

#import <IceObjC/Communicator.h>
#import <IceObjC/Properties.h>
#import <IceObjC/Stream.h>

//
// Forward declarations.
//
@class ICELogger;

@interface ICEInitializationData : NSObject
{
@private
    ICEProperties* properties;
    ICELogger* logger;
}
@property(retain, nonatomic) ICEProperties* properties;
@property(retain, nonatomic) ICELogger* logger;
@end

@interface ICEProperties (Initialize)
+(ICEProperties*) create;
+(ICEProperties*) create:(int*)argc argv:(char*[])argv;
@end

@interface ICECommunicator (Initialize)
+(ICECommunicator*) create;
+(ICECommunicator*) create:(ICEInitializationData *)initData;
+(ICECommunicator*) create:(int*)argc argv:(char*[])argv;
+(ICECommunicator*) create:(int*)argc argv:(char*[])argv initData:(ICEInitializationData *)initData;
@end

@interface ICEInputStream (Initialize)
+(ICEInputStream*) createInputStream:(ICECommunicator*)communicator data:(NSData*)data;
@end

@interface ICEOutputStream (Initialize)
+(ICEOutputStream*) createOutputStream:(ICECommunicator*)communicator;
@end
