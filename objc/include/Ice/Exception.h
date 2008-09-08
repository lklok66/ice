// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

#import <Foundation/NSException.h>

//
// Forward declarations
// 
@protocol ICEOutputStream;
@protocol ICEInputStream;

@class NSCoder;

@interface ICEException : NSException
-(id) init;
-(NSString*)ice_name;
-(id) initWithCoder:(NSCoder*)decoder; // Implementation throws NSInvalidArchiveOperationException
-(void) encodeWithCoder:(NSCoder*)coder; // Implementation throws NSInvalidArchiveOperationException
-(void) copy__:(ICEException*)copy_;
-(void) dealloc;
@end

@interface ICELocalException : ICEException
{
    const char* file;
    int line;
}
-(id)init:(const char*)file line:(int)line;
+(id)localException:(const char*)file line:(int)line;
-(void) copy__:(ICELocalException*)copy_;
-(void) dealloc;
@end

@interface ICEUserException : ICEException
-(BOOL)usesClasses__;
-(void)writeWithStream:(id<ICEOutputStream>)stream;
-(void)readWithStream:(id<ICEInputStream>)stream readTypeId:(BOOL)rid;
-(void) copy__:(ICEUserException*)copy_;
-(void) dealloc;
@end
