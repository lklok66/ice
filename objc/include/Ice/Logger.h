// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

@protocol ICELogger <NSObject>
-(void) print:(NSString*)message;
-(void) trace:(NSString*)category message:(NSString*)message;
-(void) warning:(NSString*)message;
-(void) error:(NSString*)message;
@end
