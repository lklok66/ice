// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/ObjectAdapter.h>

#include <Ice/ObjectAdapter.h>

@interface ICEObjectAdapter (Internal)
-(ICEObjectAdapter*)initWithObjectAdapter:(Ice::ObjectAdapter*)arg;
-(Ice::ObjectAdapter*)objectAdapter__;
@end

