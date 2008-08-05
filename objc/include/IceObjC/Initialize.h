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
    ICEProperties* properties;
    ICELogger* logger;
    //ICEStats* stats;
    //ICEStringConverter* stringConverter;
    //ICEWstringConverter* wstringConverter;
    //ICEThreadNotification* threadHook;
}
@property(retain) ICEProperties* properties;
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
+(ICEInputStream*) createInputStream:(ICECommunicator*)communicator buf:(unsigned char*)buf length:(int)length;
@end

@interface ICEOutputStream (Initialize)
+(ICEOutputStream*) createOutputStream:(ICECommunicator*)communicator;
@end
