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
//-(void) sayHello:(ICECurrent*);
//@end

// Proxy protocol (Slice Hello* parameters are mapped to ICEObjectPrx<HelloPrx>)
@protocol HelloPrx<ICEObjectPrx>
-(void) sayHello;
@end

@interface HelloPrx : ICEObjectPrx<HelloPrx>
+(id<HelloPrx>) checkedCast:(id<ICEObjectPrx>)proxy;
+(id<HelloPrx>) uncheckedCast:(id<ICEObjectPrx>)proxy;
@end

