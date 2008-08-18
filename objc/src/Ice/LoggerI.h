// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Logger.h>

#include <IceCpp/Logger.h>

@interface ICELogger : NSObject<ICELogger>
{
    Ice::Logger* logger__;
}
-(ICELogger*)initWithLogger:(const Ice::LoggerPtr&)arg;
-(Ice::Logger*)logger__;
+(ICELogger*)loggerWithLogger:(const Ice::LoggerPtr&)arg;
@end
