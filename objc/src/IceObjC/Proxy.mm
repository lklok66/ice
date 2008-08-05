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
#include <Ice/Initialize.h>
#include <IceObjC/StreamI.h>

#define OBJECTPRX ((IceProxy::Ice::Object*)objectPrx__)

@implementation ICEObjectPrx(Internal)

-(ICEObjectPrx*) initWithObjectPrx__:(const Ice::ObjectPrx&)arg
{
    if(![super init])
    {
        return nil;
    }
    objectPrx__ = arg.get();
    OBJECTPRX->__incRef();
    return self;
}

-(IceProxy::Ice::Object*) objectPrx__
{
    return (IceProxy::Ice::Object*)objectPrx__;
}

-(void) dealloc
{
    OBJECTPRX->__decRef();
    objectPrx__ = 0;
    [super dealloc];
}

+(ICEObjectPrx*) objectPrxWithObjectPrx__:(const Ice::ObjectPrx&)arg
{
    return [[[self alloc] initWithObjectPrx__:arg] autorelease];
}

@end

@implementation ICEObjectPrx

+(id<ICEObjectPrx>) uncheckedCast__:(id<ICEObjectPrx>)proxy
{
    if(proxy != nil)
    {
        if([(ICEObjectPrx*)proxy isKindOfClass:self])
        {
            return proxy;
        }
        else
        {
            return [[[self alloc] initWithObjectPrx__:[(ICEObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}

+(id<ICEObjectPrx>) checkedCast__:(id<ICEObjectPrx>)proxy protocol:(Protocol*)protocol sliceId:(NSString*)sliceId
{
    if(proxy != nil)
    {
        if([(ICEObjectPrx*)proxy isKindOfClass:self])
        {
            return proxy;
        }
        else if([(ICEObjectPrx*)proxy conformsToProtocol:protocol] || [proxy ice_isA:sliceId])
        {
            return [[[self alloc] initWithObjectPrx__:[(ICEObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}

-(ICEOutputStream*) createOutputStream__
{
    Ice::OutputStreamPtr os = Ice::createOutputStream(OBJECTPRX->ice_getCommunicator());
    return [[ICEOutputStream alloc] initWithOutputStream:os];
}

-(BOOL) invoke__:(NSString*)operation mode:(ICEOperationMode)mode os:(ICEOutputStream*)os is:(ICEInputStream**)is
{
    std::vector<Ice::Byte> inParams;
    [os os__]->finished(inParams);
    [os release];

    std::vector<Ice::Byte> outParams;
    BOOL ok;
    try
    {
        ok = OBJECTPRX->ice_invoke([operation UTF8String], (Ice::OperationMode)mode, inParams, outParams);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return FALSE; // Keep the compiler happy
    }
    
    Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), outParams);
    *is = [[ICEInputStream alloc] initWithInputStream:s.get()];
    return ok;
}

-(BOOL) ice_isA:(NSString*)typeId
{
    try
    {
        return OBJECTPRX->ice_isA([typeId UTF8String]);
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
        OBJECTPRX->ice_ping();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
