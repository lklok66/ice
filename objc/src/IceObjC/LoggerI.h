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

@interface ICELogger (Internal)
-(ICELogger*)initWithLogger:(const Ice::LoggerPtr&)arg;
+(ICELogger*)loggerWithLogger:(const Ice::LoggerPtr&)arg;
@end
