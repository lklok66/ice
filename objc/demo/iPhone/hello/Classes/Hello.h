// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Proxy.h>
#import <IceObjC/Object.h>

// Servant protocol
@protocol Hello
-(void) sayHello:(ICECurrent*)current;
@end

// Servant class
@interface Hello : ICEObject
@end

// Proxy protocol
@protocol HelloPrx<ICEObjectPrx>
-(void) sayHello;
@end

// Proxy class
@interface HelloPrx : ICEObjectPrx<HelloPrx>
@end

