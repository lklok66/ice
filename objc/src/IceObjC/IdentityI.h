// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Identity.h>

#include <Ice/Identity.h>

@interface Ice_Identity (Internal)
-(Ice_Identity*)initWithIdentity:(const Ice::Identity&)arg;
-(Ice::Identity)identity__;
+(Ice_Identity*)identityWithIdentity:(const Ice::Identity&)arg;
@end
