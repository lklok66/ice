// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Exception.h>

#include <Ice/Exception.h>

@interface ICELocalException (Internal)
-(id) initWithLocalException:(const Ice::LocalException&)ex;
-(void) rethrowCxx__;
+(id) localExceptionWithLocalException:(const Ice::LocalException&)ex;
@end
