// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

@interface ICELogger : NSObject
{
    void* logger__;
}
-(void) print:(NSString*)message;
-(void) trace:(NSString*)category message:(NSString*)message;
-(void) warning:(NSString*)message;
-(void) error:(NSString*)message;
@end
