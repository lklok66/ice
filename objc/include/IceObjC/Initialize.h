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
@protocol ICELogger;

@interface ICEInitializationData : NSObject
{
@private
    id<ICEProperties> properties;
    id<ICELogger> logger;
}
@property(retain, nonatomic) id<ICEProperties> properties;
@property(retain, nonatomic) id<ICELogger> logger;
@end

@interface ICEUtil : NSObject
+(id<ICEProperties>) createProperties;
+(id<ICEProperties>) createProperties:(int*)argc argv:(char*[])argv;
+(id<ICECommunicator>) createCommunicator;
+(id<ICECommunicator>) createCommunicator:(ICEInitializationData *)initData;
+(id<ICECommunicator>) createCommunicator:(int*)argc argv:(char*[])argv;
+(id<ICECommunicator>) createCommunicator:(int*)argc argv:(char*[])argv initData:(ICEInitializationData *)initData;
+(id<ICEInputStream>) createInputStream:(id<ICECommunicator>)communicator data:(NSData*)data;
+(id<ICEOutputStream>) createOutputStream:(id<ICECommunicator>)communicator;
@end
