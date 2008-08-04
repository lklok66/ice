// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Proxy.h>

// Servant protocol
//@protocol Hello
//-(void) sayHello:(Ice_Current*);
//@end

// Proxy protocol (Slice Hello* parameters are mapped to Ice_ObjectPrx<HelloPrx>)
@protocol HelloPrx<Ice_ObjectPrx>
-(void) sayHello;
@end

@interface HelloPrx : Ice_ObjectPrx<HelloPrx>
+(id<HelloPrx>) checkedCast:(id<Ice_ObjectPrx>)proxy;
+(id<HelloPrx>) uncheckedCast:(id<Ice_ObjectPrx>)proxy;
@end

