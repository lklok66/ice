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

@interface ICEObjectAdapter : NSObject<ICEObjectAdapter>
{
    Ice::ObjectAdapter* objectAdapter__;
}
-(ICEObjectAdapter*)initWithObjectAdapter:(const Ice::ObjectAdapterPtr&)arg;
-(Ice::ObjectAdapter*)objectAdapter__;
+(ICEObjectAdapter*)objectAdapterWithObjectAdapter:(const Ice::ObjectAdapterPtr&)arg;
@end

