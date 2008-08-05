// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Properties.h>

#include <Ice/Properties.h>

@interface ICEProperties (Internal)
-(ICEProperties*)initWithProperties:(const Ice::PropertiesPtr&)arg;
+(ICEProperties*)propertiesWithProperties:(const Ice::PropertiesPtr&)arg;
@end

