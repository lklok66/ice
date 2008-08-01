// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>

#import <IceObjC/Proxy.h>

#import <IceObjC/Properties.h>

@interface Ice_Communicator : NSObject
{
    void* communicator__;
}
-(Ice_ObjectPrx*) stringToProxy:(NSString*)str;
-(Ice_Properties*) getProperties;
-(void) shutdown;
-(void) destroy;

@end

