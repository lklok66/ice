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

@implementation Ice_ObjectPrx(Internal)

-(Ice_ObjectPrx*) initWithObjectPrx__:(const Ice::ObjectPrx&)arg
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

+(Ice_ObjectPrx*) objectPrxWithObjectPrx__:(const Ice::ObjectPrx&)arg
{
    return [[[self alloc] initWithObjectPrx__:arg] autorelease];
}

@end

@implementation Ice_ObjectPrx

+(id<Ice_ObjectPrx>) uncheckedCast__:(id<Ice_ObjectPrx>)proxy
{
    if(proxy != nil)
    {
        if([(Ice_ObjectPrx*)proxy isKindOfClass:self])
        {
            return proxy;
        }
        else
        {
            return [[[self alloc] initWithObjectPrx__:[(Ice_ObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}

+(id<Ice_ObjectPrx>) checkedCast__:(id<Ice_ObjectPrx>)proxy protocol:(Protocol*)protocol sliceId:(NSString*)sliceId
{
    if(proxy != nil)
    {
        if([(Ice_ObjectPrx*)proxy isKindOfClass:self])
        {
            return proxy;
        }
        else if([(Ice_ObjectPrx*)proxy conformsToProtocol:protocol] || [proxy ice_isA:sliceId])
        {
            return [[[self alloc] initWithObjectPrx__:[(Ice_ObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}

-(Ice_OutputStream*) createOutputStream__
{
    Ice::OutputStreamPtr os = Ice::createOutputStream(OBJECTPRX->ice_getCommunicator());
    return [[Ice_OutputStream alloc] initWithOutputStream:os.get()];
}

-(BOOL) invoke__:(NSString*)operation mode:(Ice_OperationMode)mode os:(Ice_OutputStream*)os is:(Ice_InputStream**)is
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
    *is = [[Ice_InputStream alloc] initWithInputStream:s.get()];
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
