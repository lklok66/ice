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
//-(void) sayHello:(ICECurrent*);
//@end

// Proxy protocol (Slice Hello* parameters are mapped to ICEObjectPrx<HelloPrxProtocol>)
@protocol HelloPrxProtocol
-(void) sayHello;
//-(void) sayHello:(NSDictionary*);
@end

@interface HelloPrx : ICEObjectPrx<HelloPrxProtocol>
+(HelloPrx*) checkedCast:(ICEObjectPrx*)proxy;
//+(HelloPrx*) checkedCast:(ICEObjectPrx*)proxy context:(NSDictionary*)context;
+(HelloPrx*) uncheckedCast:(ICEObjectPrx*)proxy;
//+(HelloPrx*) uncheckedCast:(ICEObjectPrx*)proxy context:(NSDictionary*)context;
@end

