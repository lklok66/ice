// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Identity.h>

#include <IceCpp/Identity.h>

@interface ICEIdentity (Internal)
-(ICEIdentity*)initWithIdentity:(const Ice::Identity&)arg;
-(Ice::Identity)identity__;
+(ICEIdentity*)identityWithIdentity:(const Ice::Identity&)arg;
@end
