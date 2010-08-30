// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Proxy.h>

#include <IceCpp/Proxy.h>

@interface ICEObjectPrx (ICEInternal)
-(ICEObjectPrx*)initWithObjectPrx__:(const Ice::ObjectPrx&)arg;
-(IceProxy::Ice::Object*) objectPrx__;
+(ICEObjectPrx*)objectPrxWithObjectPrx__:(const Ice::ObjectPrx&)arg;
@end
