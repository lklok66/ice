// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

#import <Ice/Communicator.h>
#import <Ice/Properties.h>
#import <Ice/Stream.h>

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

-(id) init:(id<ICEProperties>)properties logger:(id<ICELogger>)logger;
+(id) initializationData;
+(id) initializationData:(id<ICEProperties>)properties logger:(id<ICELogger>)logger;
-(id) copyWithZone:(NSZone *) zone;
-(NSUInteger) hash;
-(BOOL) isEqual:(id)anObject;
-(void) dealloc;
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
+(NSString*) generateUUID;
@end
