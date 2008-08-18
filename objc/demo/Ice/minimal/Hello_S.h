// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Proxy.h>
#import <Ice/Object.h>
#import <Hello.h>

// Servant protocol
@protocol DemoHello
-(void) sayHello:(ICECurrent*)current;
@end

// Servant class
@interface DemoHello : ICEObject
@end
