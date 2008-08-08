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
@class ICEOutputStream;
@class ICEInputStream;

@interface ICELocalException : NSException
{
    const char* file;
    int line;
}
-(NSString*)ice_name;
-(id)init:(const char*)file line:(int)line;
+(id)localException:(const char*)file line:(int)line;
@end

@interface ICEUserException : NSException
-(NSString*)ice_name;
-(BOOL)usesClasses__;
-(void)write__:(ICEOutputStream*)stream;
-(void)read__:(ICEInputStream*)stream;
@end
