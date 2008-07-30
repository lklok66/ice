// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>

@interface Ice_Communicator : NSObject

-(void) shutdown;
-(void) destroy;

@end

@interface Ice_Communicator (Initialize)

+(Ice_Communicator*) create;

@end
