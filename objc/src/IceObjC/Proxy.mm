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

@implementation Ice_ObjectPrx(Internal)

-(Ice_ObjectPrx*) initWithObjectPrx__:(IceProxy::Ice::Object*)arg
{
    if(![super init])
    {
        return nil;
    }
    proxy__ = arg;
    ((IceProxy::Ice::Object*)proxy__)->__incRef();
    return self;
}

-(IceProxy::Ice::Object*) proxy__
{
    return (IceProxy::Ice::Object*)proxy__;
}

-(void) dealloc
{
    ((IceProxy::Ice::Object*)proxy__)->__decRef();
    proxy__ = 0;
    [super dealloc];
}

@end

@implementation Ice_ObjectPrx

+(Ice_ObjectPrx*) uncheckedCast__:(Ice_ObjectPrx*)proxy
{
    if(proxy != nil)
    {
        if([proxy isKindOfClass:self])
        {
            return proxy;
        }
        else
        {
            return [[[self alloc] initWithObjectPrx__:[proxy proxy__]] autorelease];
        }
    }
    return nil;
}

+(Ice_ObjectPrx*) checkedCast__:(Ice_ObjectPrx*)proxy protocol:(Protocol*)protocol sliceId:(NSString*)sliceId
{
    if(proxy != nil)
    {
        if([proxy isKindOfClass:self])
        {
            return proxy;
        }
        else if([proxy conformsToProtocol:protocol] || [proxy ice_isA:sliceId])
        {
            return [[[self alloc] initWithObjectPrx__:[proxy proxy__]] autorelease];
        }
    }
    return nil;
}

-(Ice_OutputStream*) createOutputStream__
{
    Ice::OutputStreamPtr os = Ice::createOutputStream(((IceProxy::Ice::Object*)proxy__)->ice_getCommunicator());
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
        ok = ((IceProxy::Ice::Object*)proxy__)->ice_invoke([operation UTF8String], (Ice::OperationMode)mode, inParams, outParams);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return FALSE; // Keep the compiler happy
    }
    
    Ice::InputStreamPtr s = Ice::createInputStream(((IceProxy::Ice::Object*)proxy__)->ice_getCommunicator(), outParams);
    *is = [[Ice_InputStream alloc] initWithInputStream:s.get()];
    return ok;
}

-(BOOL) ice_isA:(NSString*)typeId
{
    try
    {
        return ((IceProxy::Ice::Object*)proxy__)->ice_isA([typeId UTF8String]);
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
        ((IceProxy::Ice::Object*)proxy__)->ice_ping();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
