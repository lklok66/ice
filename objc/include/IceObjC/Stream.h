// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

@interface ICEInputStream : NSObject
{
    void* is__;
}
-(BOOL)readBool;
-(NSString*)readString;
@end

@interface ICEOutputStream : NSObject
{
    void* os__;
}
-(void)writeBool:(BOOL)v;
-(void)writeString:(NSString*)v;
@end
