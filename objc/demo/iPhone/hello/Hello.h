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
//@protocol HelloProtocol
//-(void) sayHello:(Ice_Current*);
//@end

// Proxy protocol (Slice Hello* parameters are mapped to Ice_ObjectPrx<HelloPrxProtocol>)
@protocol HelloPrxProtocol
-(void) sayHello;
//-(void) sayHello:(NSDictionary*);
@end

@interface HelloPrx : Ice_ObjectPrx<HelloPrxProtocol>
+(HelloPrx*) checkedCast:(Ice_ObjectPrx*)proxy;
//+(HelloPrx*) checkedCast:(Ice_ObjectPrx*)proxy context:(NSDictionary*)context;
+(HelloPrx*) uncheckedCast:(Ice_ObjectPrx*)proxy;
//+(HelloPrx*) uncheckedCast:(Ice_ObjectPrx*)proxy context:(NSDictionary*)context;
@end

