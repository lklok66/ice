// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Exception.h>

#include <IceCpp/Exception.h>

@interface ICELocalException (ICEInternal)
-(id) initWithLocalException:(const Ice::LocalException&)ex;
-(void) rethrowCxx;
+(id) localExceptionWithLocalException:(const Ice::LocalException&)ex;
@end
