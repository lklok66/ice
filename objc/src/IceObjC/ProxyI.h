// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Proxy.h>

#include <Ice/Proxy.h>

@interface Ice_ObjectPrxI : Ice_ObjectPrx
{
    IceProxy::Ice::Object* proxy;
}
-(Ice_ObjectPrx*)initWithObjectPrx:(const Ice::ObjectPrx&)arg;
@end

