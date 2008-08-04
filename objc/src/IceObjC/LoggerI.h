// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Logger.h>

#include <Ice/Logger.h>

@interface Ice_Logger (Internal)
-(Ice_Logger*)initWithLogger:(const Ice::LoggerPtr&)arg;
+(Ice_Logger*)loggerWithLogger:(const Ice::LoggerPtr&)arg;
@end
