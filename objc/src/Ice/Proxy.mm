// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ProxyI.h>
#import <Ice/Object.h>
#import <Ice/Util.h>
#import <Ice/StreamI.h>
#import <Ice/CommunicatorI.h>
#import <Ice/IdentityI.h>
#import <Ice/ConnectionI.h>
#import <Ice/LocalException.h>

#import <Ice/Router.h>
#import <Ice/Locator.h>

#include <IceCpp/Initialize.h>
#include <IceCpp/Proxy.h>
#include <IceCpp/LocalException.h>
#include <IceCpp/Router.h>
#include <IceCpp/Locator.h>

#import <objc/runtime.h>
#import <objc/message.h>

#import <Foundation/NSThread.h>
#import <Foundation/NSInvocation.h>
#import <Foundation/NSAutoreleasePool.h>

#define OBJECTPRX ((IceProxy::Ice::Object*)objectPrx__)

namespace
{

class AMICallbackBase
{
public:

AMICallbackBase(id target, SEL ex) : _target(target), _exception(ex)
{
    [_target retain];
}

virtual ~AMICallbackBase()
{
    [_target release];
}

void ice_exception(const Ice::Exception& ex)
{
    if(!_exception)
    {
        return;
    }
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        @try
        {
            rethrowObjCException(ex);
        }
        @catch(ICEException* e)
        {
            objc_msgSend(_target, _exception, e);
        }
    }
    @catch(NSException* e)
    {
        exception = [e retain];
    }
    [pool release];

    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

protected:

id _target;
SEL _exception;

};

class AMICallbackBaseWithSent : virtual public AMICallbackBase, public Ice::AMISentCallback
{
public:

AMICallbackBaseWithSent(id target, SEL ex, SEL sent) : AMICallbackBase(target, ex), _sent(sent)
{
}

virtual void ice_sent()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        objc_msgSend(_target, _sent);
    }
    @catch(NSException* e)
    {
        exception = [e retain];
    }
    [pool release];

    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

private:

SEL _sent;

};

class AMICallback : virtual public AMICallbackBase, public Ice::AMI_Array_Object_ice_invoke
{
public:

AMICallback(const Ice::CommunicatorPtr& communicator, id target, SEL resp, SEL ex, Class finishedClass, SEL finished) : 
    AMICallbackBase(target, ex),
    _communicator(communicator), 
    _response(resp),
    _finishedClass(finishedClass),
    _finished(finished)
{
}

virtual void
ice_response(bool ok , const std::pair<const Byte*, const Byte*>& outParams)
{
    if(ok && !_response || !ok && !_exception)
    {
        return;
    }
    
    ICEInputStream* is;
    {
        Ice::InputStreamPtr s = Ice::createInputStream(_communicator, outParams);
        is = [ICEInputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
    }
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        objc_msgSend(_finishedClass, _finished, _target, _response, _exception, ok, is);
    }
    @catch(NSException* ex)
    {
        exception = [ex retain];
    }
    [pool release];
    [is release];

    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

virtual void ice_exception(const Ice::Exception& ex)
{
    AMICallbackBase::ice_exception(ex);
}

private:

const Ice::CommunicatorPtr _communicator;
SEL _response;
Class _finishedClass;
SEL _finished;
};
typedef IceUtil::Handle<AMICallback> AMICallbackPtr;

class AMICallbackWithSent : public AMICallbackBaseWithSent, public AMICallback
{
public:
    
AMICallbackWithSent(const Ice::CommunicatorPtr& communicator, id target, SEL resp, SEL ex, SEL sent, 
                    Class finishedClass ,SEL finished) : 
    AMICallbackBase(target, ex),
    AMICallbackBaseWithSent(target, ex, sent),
    AMICallback(communicator, target, resp, ex, finishedClass, finished)
{
}
};

class AMIIceInvokeCallback : virtual public AMICallbackBase, public Ice::AMI_Array_Object_ice_invoke
{
public:
    
AMIIceInvokeCallback(id target, SEL response, SEL ex) : AMICallbackBase(target, ex), _response(response)
{
}

virtual void
ice_response(bool ok , const std::pair<const Byte*, const Byte*>& oP)
{
    if(!_response)
    {
        return;
    }

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        objc_msgSend(_target, _response, ok, [NSMutableData dataWithBytes:oP.first length:(oP.second - oP.first)]);
    }
    @catch(NSException* e)
    {
        exception = [e retain];
    }
    [pool release];

    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

virtual void ice_exception(const Ice::Exception& ex)
{
    AMICallbackBase::ice_exception(ex);
}

private:

SEL _response;

};

class AMIIceInvokeCallbackWithSent : public AMICallbackBaseWithSent, public AMIIceInvokeCallback
{
public:
    
AMIIceInvokeCallbackWithSent(id target, SEL response, SEL ex, SEL sent) :
    AMICallbackBase(target, ex),
    AMICallbackBaseWithSent(target, ex, sent),
    AMIIceInvokeCallback(target, response, ex)
{
}

};

class AMIIceFlushBatchRequestsCallback : virtual public AMICallbackBase, public Ice::AMI_Object_ice_flushBatchRequests
{
public:

AMIIceFlushBatchRequestsCallback(id target, SEL ex) : AMICallbackBase(target, ex)
{
}

virtual void ice_exception(const Ice::Exception& ex)
{
    AMICallbackBase::ice_exception(ex);
}

};

class AMIIceFlushBatchRequestsCallbackWithSent : public AMICallbackBaseWithSent, public AMIIceFlushBatchRequestsCallback
{
public:

AMIIceFlushBatchRequestsCallbackWithSent(id target, SEL ex, SEL sent) :
    AMICallbackBase(target, ex),
    AMICallbackBaseWithSent(target, ex, sent),
    AMIIceFlushBatchRequestsCallback(target, ex)
{
}

};

};


@implementation ICECallbackOnMainThread
-(id)init:(id)cb
{
    cb_ = [cb retain];
    return self;
}
-(void) dealloc
{
    [cb_ release];
    [super dealloc];
}
+(id)callbackOnMainThread:(id)cb
{
    return [[[self alloc] init:cb] autorelease];
}
-(void)forwardInvocation:(NSInvocation *)inv
{
    [inv retainArguments];
    [inv performSelectorOnMainThread:@selector(invokeWithTarget:) withObject:cb_ waitUntilDone:NO];
}
-(NSMethodSignature *)methodSignatureForSelector:(SEL)selector
{
    return [cb_ methodSignatureForSelector:selector];
}
@end

@implementation ICEObjectPrx (ICEInternal)

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
    if(!arg)
    {
        return nil;
    }
    else
    {
        return [[[self alloc] initWithObjectPrx__:arg] autorelease];
    }
}
@end

@implementation ICEObjectPrx
+(id) ice_readWithStream:(id<ICEInputStream>)stream
{
    return [stream readProxy:self];
}
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream
{
    [stream writeProxy:obj];
}
+(id) uncheckedCast:(id<ICEObjectPrx>)proxy
{
    if(proxy != nil)
    {
        if([(ICEObjectPrx*)proxy isKindOfClass:self])
        {
            return [[proxy retain] autorelease];
        }
        else
        {
            return [[[self alloc] initWithObjectPrx__:[(ICEObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}
+(id) uncheckedCast:(id<ICEObjectPrx>)proxy facet:(NSString*)facet
{
    return [self uncheckedCast:[proxy ice_facet:facet]];
}
+(id) checkedCast:(id<ICEObjectPrx>)proxy
{
    if(proxy != nil)
    {
        if([(ICEObjectPrx*)proxy isKindOfClass:self])
        {
            return [[proxy retain] autorelease];
        }
        else if([(ICEObjectPrx*)proxy conformsToProtocol:[self protocol__]] ||
                [proxy ice_isA:[self ice_staticId]])
        {
            return [[[self alloc] initWithObjectPrx__:[(ICEObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}
+(id) checkedCast:(id<ICEObjectPrx>)proxy facet:(NSString*)facet
{
    try
    {
        return [self checkedCast:[proxy ice_facet:facet]];
    }
    catch(ICEFacetNotExistException* ex)
    {
        return nil;
    }
}
+(id) checkedCast:(id<ICEObjectPrx>)proxy context:(ICEContext*)context
{
    if(proxy != nil)
    {
        if([(ICEObjectPrx*)proxy isKindOfClass:self])
        {
            return [[proxy retain] autorelease];
        }
        else if([(ICEObjectPrx*)proxy conformsToProtocol:[self protocol__]] ||
                [proxy ice_isA:[self ice_staticId] context:context])
        {
            return [[[self alloc] initWithObjectPrx__:[(ICEObjectPrx*)proxy objectPrx__]] autorelease];
        }
    }
    return nil;
}
+(id) checkedCast:(id<ICEObjectPrx>)proxy facet:(NSString*)facet context:(ICEContext*)context
{
    try
    {
        return [self checkedCast:[proxy ice_facet:facet] context:context];
    }
    catch(ICEFacetNotExistException* ex)
    {
        return nil;
    }
}
+(NSString*) ice_staticId
{
    return @"::Ice::Object";
}

+(Protocol*) protocol__
{
    return objc_getProtocol(class_getName([self class]));
}

-(id<ICEOutputStream>) createOutputStream__
{
    try
    {
        Ice::OutputStreamPtr os = Ice::createOutputStream(OBJECTPRX->ice_getCommunicator());
        return [ICEOutputStream wrapperWithCxxObjectNoAutoRelease:os.get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(void) checkTwowayOnly__:(NSString*)name
{
    if(!OBJECTPRX->ice_isTwoway())
    {
        @throw [ICETwowayOnlyException twowayOnlyException:__FILE__ line:__LINE__ operation:name];
    }
}

-(void) invoke__:(NSString*)operation 
            mode:(ICEOperationMode)mode 
              os:(id<ICEOutputStream>)os 
              is:(id<ICEInputStream>*)is 
         context:(ICEContext*)context
{
    BOOL ok = YES; // Keep the compiler happy.
    try
    {
        std::vector<Ice::Byte> inParams;
        if(os)
        {
            [(ICEOutputStream*)os os]->finished(inParams);
        }

        std::vector<Ice::Byte> outParams;
        if(context != nil)
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
            *is = [ICEInputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
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

-(BOOL) invoke_async__:(id)target
              response:(SEL)response
             exception:(SEL)exception
                  sent:(SEL)sent
              finishedClass:(Class)finishedClass
              finished:(SEL)finished
             operation:(NSString*)operation
                  mode:(ICEOperationMode)mode 
                    os:(id<ICEOutputStream>)os
               context:(ICEContext*)context
{
    try
    {
        std::vector<Ice::Byte> inParams;
        if(os)
        {
            [(ICEOutputStream*)os os]->finished(inParams);
        }
        std::pair<const ::Ice::Byte*, const ::Ice::Byte*> inP(&inParams[0], &inParams[0] + inParams.size());
        
        AMICallbackPtr amiCB;
        if(sent != nil)
        {
            amiCB = new AMICallbackWithSent(OBJECTPRX->ice_getCommunicator(), target, response, exception, sent,
                                            finishedClass, finished);
        }
        else
        {
            amiCB = new AMICallback(OBJECTPRX->ice_getCommunicator(), target, response, exception, finishedClass, 
                                    finished);
        }

        if(context != nil)
        {
            Ice::Context ctx;
            fromNSDictionary(context, ctx);
            return OBJECTPRX->ice_invoke_async(amiCB, fromNSString(operation), (Ice::OperationMode)mode, inP, ctx);
        }
        else
        {
            return OBJECTPRX->ice_invoke_async(amiCB, fromNSString(operation), (Ice::OperationMode)mode, inP);
        }
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

-(id) copyWithZone:(NSZone *)zone
{
    return [self retain];
}

-(NSUInteger) hash
{
    return (NSUInteger)OBJECTPRX->ice_getHash();
}
-(NSString*) description
{
    return [toNSString(OBJECTPRX->ice_toString()) autorelease];
}
-(BOOL) isEqual:(id)o_
{
    if(self == o_)
    {
        return YES;
    }
    if(!o_ || ![o_ isKindOfClass:[ICEObjectPrx class]])
    {
        return NO;
    }
    return *OBJECTPRX == *[o_ objectPrx__];
}

-(NSComparisonResult) compareIdentity:(id<ICEObjectPrx>)aProxy
{
    IceProxy::Ice::Object* lhs = OBJECTPRX;
    IceProxy::Ice::Object* rhs = [(ICEObjectPrx*)aProxy objectPrx__];
    if(Ice::proxyIdentityEqual(lhs, rhs))
    {
        return NSOrderedSame;
    } 
    else if(Ice::proxyIdentityLess(lhs, rhs))
    {
        return NSOrderedAscending;
    }
    else
    {
        return NSOrderedDescending;
    }
}

-(NSComparisonResult) compareIdentityAndFacet:(id<ICEObjectPrx>)aProxy
{
    IceProxy::Ice::Object* lhs = OBJECTPRX;
    IceProxy::Ice::Object* rhs = [(ICEObjectPrx*)aProxy objectPrx__];
    if(Ice::proxyIdentityAndFacetEqual(lhs, rhs))
    {
        return NSOrderedSame;
    } 
    else if(Ice::proxyIdentityAndFacetLess(lhs, rhs))
    {
        return NSOrderedAscending;
    }
    else
    {
        return NSOrderedDescending;
    }
}

-(id<ICECommunicator>) ice_getCommunicator
{
    return [ICECommunicator wrapperWithCxxObject:OBJECTPRX->ice_getCommunicator().get()];
}

-(NSString*) ice_toString
{
    return [toNSString(OBJECTPRX->ice_toString()) autorelease];
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
-(BOOL) ice_isA:(NSString*)typeId context:(ICEContext*)context
{
    try
    {
        Ice::Context ctx;
        return OBJECTPRX->ice_isA(fromNSString(typeId), fromNSDictionary(context, ctx));
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
-(void) ice_ping:(ICEContext*)context
{
    try
    {
        Ice::Context ctx;
        OBJECTPRX->ice_ping(fromNSDictionary(context, ctx));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
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
-(NSArray*) ice_ids:(ICEContext*)context
{
    try
    {
        Ice::Context ctx;
        return [toNSArray(OBJECTPRX->ice_ids(fromNSDictionary(context, ctx))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
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
-(NSString*) ice_id:(ICEContext*)context
{
    try
    {
        Ice::Context ctx;
        return [toNSString(OBJECTPRX->ice_id(fromNSDictionary(context, ctx))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
    }
}

-(BOOL) ice_invoke:(NSString*)operation 
              mode:(ICEOperationMode)mode
          inParams:(NSData*)inParams 
         outParams:(NSMutableData**)outParams
{
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> inP((ICEByte*)[inParams bytes], 
                                                          (ICEByte*)[inParams bytes] + [inParams length]);
        std::vector<Ice::Byte> outP;
        BOOL ok = OBJECTPRX->ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inP, outP);
        *outParams = [NSMutableData dataWithBytes:&outP[0] length:outP.size()];
        return ok;
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
    }
}

-(BOOL) ice_invoke:(NSString*)operation 
              mode:(ICEOperationMode)mode 
          inParams:(NSData*)inParams 
         outParams:(NSMutableData**)outParams
           context:(ICEContext*)context
{
    try
    {
        Ice::Context ctx;
        std::pair<const Ice::Byte*, const Ice::Byte*> inP((ICEByte*)[inParams bytes], 
                                                          (ICEByte*)[inParams bytes] + [inParams length]);
        std::vector<Ice::Byte> outP;
        BOOL ok = OBJECTPRX->ice_invoke(fromNSString(operation), 
                                        (Ice::OperationMode)mode, 
                                        inP, 
                                        outP, 
                                        fromNSDictionary(context, ctx));
        *outParams = [NSMutableData dataWithBytes:&outP[0] length:outP.size()];
        return ok;
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy
    }
}

-(BOOL) ice_invoke_async:(id)target
                response:(SEL)response
               exception:(SEL)exception 
               operation:(NSString*)operation 
                    mode:(ICEOperationMode)mode 
                inParams:(NSData*)inParams
{
    return [self ice_invoke_async:target response:response exception:exception sent:nil operation:operation mode:mode 
                 inParams:inParams];
}

-(BOOL) ice_invoke_async:(id)target
                response:(SEL)response
               exception:(SEL)exception 
               operation:(NSString*)operation 
                    mode:(ICEOperationMode)mode 
                inParams:(NSData*)inParams
                 context:(ICEContext*)context
{
    return [self ice_invoke_async:target response:response exception:exception sent:nil operation:operation mode:mode 
                 inParams:inParams context:context];
}

-(BOOL) ice_invoke_async:(id)target
                response:(SEL)response
               exception:(SEL)exception 
                    sent:(SEL)sent
               operation:(NSString*)operation 
                    mode:(ICEOperationMode)mode 
                inParams:(NSData*)inParams
{
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> inP((ICEByte*)[inParams bytes], 
                                                          (ICEByte*)[inParams bytes] + [inParams length]);
        Ice::AMI_Array_Object_ice_invokePtr amiCB;
        if(sent != nil)
        {
            amiCB = new AMIIceInvokeCallbackWithSent(target, response, exception, sent);
        }
        else
        {
            amiCB = new AMIIceInvokeCallback(target, response, exception);
        }
        return OBJECTPRX->ice_invoke_async(amiCB, fromNSString(operation), (Ice::OperationMode)mode, inP);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

-(BOOL) ice_invoke_async:(id)target
                response:(SEL)response
               exception:(SEL)exception
                    sent:(SEL)sent
               operation:(NSString*)operation 
                    mode:(ICEOperationMode)mode 
                inParams:(NSData*)inParams 
                 context:(ICEContext*)context
{
    try
    {
        std::pair<const Ice::Byte*, const Ice::Byte*> inP((ICEByte*)[inParams bytes], 
                                                          (ICEByte*)[inParams bytes] + [inParams length]);
        Ice::AMI_Array_Object_ice_invokePtr amiCB;
        if(sent != nil)
        {
            amiCB = new AMIIceInvokeCallbackWithSent(target, response, exception, sent);
        }
        else
        {
            amiCB = new AMIIceInvokeCallback(target, response, exception);
        }
        Ice::Context ctx;
        return OBJECTPRX->ice_invoke_async(amiCB, fromNSString(operation), (Ice::OperationMode)mode, inP, 
                                           fromNSDictionary(context, ctx));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

-(ICEIdentity*) ice_getIdentity
{
    return [ICEIdentity identityWithIdentity:OBJECTPRX->ice_getIdentity()];
}
-(id) ice_identity:(ICEIdentity*)identity
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_identity([identity identity])];
}
-(ICEMutableContext*) ice_getContext
{
    return [toNSDictionary(OBJECTPRX->ice_getContext()) autorelease];
}
-(id) ice_context:(ICEContext*)context
{
    Ice::Context ctx;
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_context(fromNSDictionary(context, ctx))];
}
-(NSString*) ice_getFacet
{
    return [toNSString(OBJECTPRX->ice_getFacet()) autorelease];
}
-(id) ice_facet:(NSString*)facet
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_facet(fromNSString(facet))];
}
-(NSString*) ice_getAdapterId
{
    return [toNSString(OBJECTPRX->ice_getAdapterId()) autorelease];
}
-(id) ice_adapterId:(NSString*)adapterId
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_adapterId(fromNSString(adapterId))];
}
//-(NSArray*) ice_getEndpoints
//{
//}
//-(id) ice_endpoints:(NSArray*)endpoints
//{
//}
-(ICEInt) ice_getLocatorCacheTimeout
{
    return OBJECTPRX->ice_getLocatorCacheTimeout();
}
-(id) ice_locatorCacheTimeout:(ICEInt)timeout
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_locatorCacheTimeout(timeout)];
}
-(BOOL) ice_isConnectionCached
{
    return OBJECTPRX->ice_isConnectionCached();
}
-(id) ice_connectionCached:(BOOL)cached
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_connectionCached(cached)];
}
-(ICEEndpointSelectionType) ice_getEndpointSelection
{
    return (ICEEndpointSelectionType)OBJECTPRX->ice_getEndpointSelection();
}
-(id) ice_endpointSelection:(ICEEndpointSelectionType)type
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_endpointSelection((Ice::EndpointSelectionType)type)];
}
-(BOOL) ice_isSecure
{
    return OBJECTPRX->ice_isSecure();
}
-(id) ice_secure:(BOOL)secure
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_secure(secure)];
}
-(BOOL) ice_isPreferSecure
{
    return OBJECTPRX->ice_isPreferSecure();
}
-(id) ice_preferSecure:(BOOL)preferSecure
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_preferSecure(preferSecure)];
}
-(id<ICERouterPrx>) ice_getRouter
{
    return (id<ICERouterPrx>)[ICERouterPrx objectPrxWithObjectPrx__:OBJECTPRX->ice_getRouter()];
}
-(id) ice_router:(id<ICERouterPrx>)router
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_router(
            Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)router objectPrx__])))];
}
-(id<ICELocatorPrx>) ice_getLocator
{
    return (id<ICELocatorPrx>)[ICELocatorPrx objectPrxWithObjectPrx__:OBJECTPRX->ice_getLocator()];
}
-(id) ice_locator:(id<ICELocatorPrx>)locator
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_locator(
            Ice::LocatorPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)locator objectPrx__])))];
}
// -(BOOL) ice_isCollocationOptimized
// {
//     return OBJECTPRX->ice_isCollocationOptimized();
// }
// -(id) ice_collocationOptimized:(BOOL)collocOptimized
// {
//     return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_collocationOptimized(collocOptimized)];
// }
-(id) ice_twoway
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_twoway()];
}
-(BOOL) ice_isTwoway
{
    return OBJECTPRX->ice_isTwoway();
}
-(id) ice_oneway
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_oneway()];
}
-(BOOL) ice_isOneway
{
    return OBJECTPRX->ice_isOneway();
}
-(id) ice_batchOneway
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_batchOneway()];
}
-(BOOL) ice_isBatchOneway
{
    return OBJECTPRX->ice_isBatchOneway();
}
-(id) ice_datagram
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_datagram()];
}
-(BOOL) ice_isDatagram
{
    return OBJECTPRX->ice_isDatagram();
}
-(id) ice_batchDatagram
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_batchDatagram()];
}
-(BOOL) ice_isBatchDatagram
{
    return OBJECTPRX->ice_isBatchDatagram();
}
-(id) ice_compress:(BOOL)compress
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_compress(compress)];
}
-(id) ice_timeout:(int)timeout
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_timeout(timeout)];
}
-(id) ice_connectionId:(NSString*)connectionId
{
    return [[self class] objectPrxWithObjectPrx__:OBJECTPRX->ice_connectionId(fromNSString(connectionId))];
}
-(id<ICEConnection>) ice_getConnection
{
    try
    {
        return [ICEConnection wrapperWithCxxObject:OBJECTPRX->ice_getConnection().get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    return nil; // Keep the compiler happy.
}
-(id<ICEConnection>) ice_getCachedConnection
{
    try
    {
        return [ICEConnection wrapperWithCxxObject:OBJECTPRX->ice_getCachedConnection().get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    return nil; // Keep the compiler happy.
}
-(void) ice_flushBatchRequests
{
    try
    {
        OBJECTPRX->ice_flushBatchRequests();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(BOOL) ice_flushBatchRequests_async:(id)target exception:(SEL)exception
{
    return [self ice_flushBatchRequests_async:target exception:exception sent:nil];
}
-(BOOL) ice_flushBatchRequests_async:(id)target exception:(SEL)exception sent:(SEL)sent
{
    try
    {
        Ice::AMI_Object_ice_flushBatchRequestsPtr amiCB;
        if(sent != nil)
        {
            amiCB = new AMIIceFlushBatchRequestsCallbackWithSent(target, exception, sent);
        }
        else
        {
            amiCB = new AMIIceFlushBatchRequestsCallback(target, exception);
        }
        return OBJECTPRX->ice_flushBatchRequests_async(amiCB);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}
@end
