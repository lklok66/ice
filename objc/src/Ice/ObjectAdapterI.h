// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ObjectAdapter.h>
#import <Ice/Wrapper.h>

#include <IceCpp/ObjectAdapter.h>

@class ICECommunicator;

@interface ICEObjectAdapter : ICEInternalWrapper<ICEObjectAdapter>
-(Ice::ObjectAdapter*) adapter;
@end

