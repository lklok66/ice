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
#import <IceObjC/Object.h>
#import <IceObjC/Util.h>
#import <IceObjC/StreamI.h>

#include <Ice/Initialize.h>
#include <Ice/Proxy.h>
#include <Ice/LocalException.h>

#import <objc/runtime.h>

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

+(id) uncheckedCast:(id<ICEObjectPrx>)proxy
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

+(id) checkedCast:(id<ICEObjectPrx>)proxy
{
    if(proxy != nil)
    {
        if([(ICEObjectPrx*)proxy isKindOfClass:self])
        {
            return proxy;
        }
        else if([(ICEObjectPrx*)proxy conformsToProtocol:[self protocol__]] || [proxy ice_isA:[self ice_staticId]])
        {
            return [[[self alloc] initWithObjectPrx__:[(ICEObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}

#if 0
//
// Removed here because each Prx class now implements ice_staticId
//
+(NSString*) ice_staticId
{
    return [[self servant__] ice_staticId];
}
#endif

+(Protocol*) protocol__
{
    return objc_getProtocol(class_getName([self class]));
}

+(Class) servant__
{
    //
    // The servant class name is the proxy class name minus the 'Prx' suffix.
    //
    const char* proxyClassName = class_getName([self class]);
    int len = strlen(proxyClassName) - 3 + 1;
    char* objClassName = (char*)malloc(len);
    strncpy(objClassName, proxyClassName, len - 1);
    objClassName[len - 1] = '\0';
    Class c = objc_getClass(objClassName);
    NSAssert(c, @"can't find servant class");
    free(objClassName);
    return c;
}

-(id<ICEOutputStream>) createOutputStream__
{
    try
    {
        Ice::OutputStreamPtr os = Ice::createOutputStream(OBJECTPRX->ice_getCommunicator());
        return [[ICEOutputStream alloc] initWithOutputStream:os];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

-(void) invoke__:(NSString*)operation 
            mode:(ICEOperationMode)mode 
              os:(id<ICEOutputStream>)os 
              is:(id<ICEInputStream>*)is 
         context:(ICEContext*)context
{
    BOOL ok;
    try
    {
        std::vector<Ice::Byte> inParams;
        if(os)
        {
            [(ICEOutputStream*)os os__]->finished(inParams);
        }

        std::vector<Ice::Byte> outParams;
        if(context)
        {
            Ice::Context ctx;
            fromNSDictionary(context, ctx);
            ok = OBJECTPRX->ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inParams, outParams, ctx);
        }
        else
        {
            ok = OBJECTPRX->ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inParams, outParams);
        }

        if(is)
        {
            Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), outParams);
            *is = [[ICEInputStream alloc] initWithInputStream:s];
        }
        else if(!outParams.empty())
        {
            if(ok)
            {
                throw Ice::EncapsulationException(__FILE__, __LINE__);
            }
            else
            {
                Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), outParams);
                try
                {
                    s->throwException();
                }
                catch(const Ice::UserException& ex)
                {
                    throw Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_name());
                }
            }
        }
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }

    if(!ok)
    {
        NSAssert(is && *is, @"input stream not set");
        [*is throwException];
    }
}

-(NSString*) ice_id
{
    try
    {
        return [toNSString(OBJECTPRX->ice_id()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
    }
}

-(NSArray*) ice_ids
{
    try
    {
        return [toNSArray(OBJECTPRX->ice_ids()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
    }
}

-(BOOL) ice_isA:(NSString*)typeId
{
    try
    {
        return OBJECTPRX->ice_isA(fromNSString(typeId));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
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
