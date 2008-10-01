// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Properties.h>
#import <Ice/Wrapper.h>

#include <IceCpp/Properties.h>

@interface ICEProperties : ICEInternalWrapper<ICEProperties>
{
    Ice::Properties* properties_;
}
-(Ice::Properties*)properties;
@end

