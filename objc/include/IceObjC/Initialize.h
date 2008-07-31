// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Communicator.h>
#import <IceObjC/Properties.h>

@interface Ice_InitializationData : NSObject
{
    Ice_Properties* properties;
    //Ice_Logger* logger;
    //Ice_Stats* stats;
    //Ice_StringConverter* stringConverter;
    //Ice_WstringConverter* wstringConverter;
    //Ice_ThreadNotification* threadHook;
}
@property(retain) Ice_Properties* properties;
@end

@interface Ice_Properties (Initialize)
+(Ice_Properties*) create;
+(Ice_Properties*) create:(int*)argc argv:(char*[])argv;
@end

@interface Ice_Communicator (Initialize)
+(Ice_Communicator*) create;
+(Ice_Communicator*) create:(Ice_InitializationData *)initData;
+(Ice_Communicator*) create:(int*)argc argv:(char*[])argv;
+(Ice_Communicator*) create:(int*)argc argv:(char*[])argv initData:(Ice_InitializationData *)initData;
@end
