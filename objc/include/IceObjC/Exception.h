// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

#import <Foundation/NSException.h>

//
// Forward declarations
// 
@protocol ICEOutputStream;
@protocol ICEInputStream;

@interface ICEException : NSException
-(id) init;
-(NSString*)ice_name;
@end

@interface ICELocalException : ICEException
{
    const char* file;
    int line;
}
-(id)init:(const char*)file line:(int)line;
+(id)localException:(const char*)file line:(int)line;
@end

@interface ICEUserException : ICEException
-(BOOL)usesClasses__;
-(void)write__:(id<ICEOutputStream>)stream;
-(void)read__:(id<ICEInputStream>)stream readTypeId:(BOOL)rid;
@end
