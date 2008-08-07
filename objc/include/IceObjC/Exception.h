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

@interface ICELocalException : NSException
{
    NSString* file;
    int line;
}
-(NSString*)ice_name;
-(id)init:(NSString*)file line:(int)line;
+(id)localException:(NSString*)file line:(int)line;
@end

@interface ICEUserException : NSException
-(NSString*)ice_name;
@end
