// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSString.h>

#import <IceObjC/ProxyI.h>

#include <IceObjC/Util.h>

#include <Ice/Proxy.h>

@implementation Ice_ObjectPrx

-(BOOL) ice_isA:(NSString*)typeId
{
    assert(false);
}

-(void) ice_ping
{
    assert(false);
}

@end

@implementation Ice_ObjectPrxI

-(Ice_ObjectPrx*) initWithObjectPrx:(const Ice::ObjectPrx&)arg
{
    proxy = arg.get();
    proxy->__incRef();
    return self;
}

-(void) dealloc
{
    proxy->__decRef();
    proxy = 0;
    [super dealloc];
}

-(BOOL) ice_isA:(NSString*)typeId
{
    try
    {
        return proxy->ice_isA([typeId UTF8String]);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return FALSE; // Keep the compiler happy
    }
}

-(void) ice_ping
{
    try
    {
        proxy->ice_ping();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
