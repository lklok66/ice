// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Logger.h>
#import <Ice/Wrapper.h>

#include <IceCpp/Logger.h>

@interface ICELogger : ICEInternalWrapper<ICELogger>
{
    Ice::Logger* logger_;
}
-(Ice::Logger*)logger;
+(ICELogger*)loggerWithLogger:(id<ICELogger>)arg;
@end
