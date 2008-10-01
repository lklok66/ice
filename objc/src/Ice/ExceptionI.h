// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Exception.h>

#include <IceCpp/Exception.h>

@interface ICELocalException (ICEInternal)
-(id) initWithLocalException:(const Ice::LocalException&)ex;
-(void) rethrowCxx;
+(id) localExceptionWithLocalException:(const Ice::LocalException&)ex;
@end
