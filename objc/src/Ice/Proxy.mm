// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
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

#include <Block.h>

#define OBJECTPRX ((IceProxy::Ice::Object*)objectPrx__)
#define ASYNCRESULT ((Ice::AsyncResult*)asyncResult__)

namespace
{

class BeginInvokeAsyncCallback : public IceUtil::Shared
{
public:

BeginInvokeAsyncCallback(void (^completed)(id<ICEInputStream>, BOOL), 
                         void (^exception)(ICEException*), 
                         void (^sent)(BOOL),
                         BOOL returnsData) :
    _completed(Block_copy(completed)), 
    _exception(Block_copy(exception)),
    _sent(Block_copy(sent)), 
    _returnsData(returnsData)
{
}

virtual ~BeginInvokeAsyncCallback()
{
    Block_release(_completed);
    Block_release(_exception);
    Block_release(_sent);
}

void completed(const Ice::AsyncResultPtr& result)
{
    BOOL ok = YES; // Keep the compiler happy.
    id<ICEInputStream> is = nil;
    NSException* nsex = nil;
    Ice::ObjectPrx proxy = result->getProxy();
    try
    {
        std::vector<Ice::Byte> outParams;
        ok = proxy->end_ice_invoke(outParams, result);
        Ice::InputStreamPtr s = Ice::createInputStream(proxy->ice_getCommunicator(), outParams);
        if(_returnsData)
        {
            is = [ICEInputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
        }
        else if(!outParams.empty())
        {
            if(ok)
            {
                throw Ice::EncapsulationException(__FILE__, __LINE__);
            }
            else
            {
                Ice::InputStreamPtr s = Ice::createInputStream(proxy->ice_getCommunicator(), outParams);
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
        if(is != nil)
        {
            [is release];
            is = nil;
        }
        nsex = toObjCException(ex);
    }

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        if(nsex != nil)
        {
            @try
            {
                @throw nsex;
            }
            @catch(ICEException* ex)
            {
                if(_exception)
                {
                    _exception(ex);
                }
                return;
            }
        }

        _completed(is, ok);
    }
    @catch(NSException* e)
    {
        exception = [e retain];
    }
    [is release];
    [pool release];

    if(exception != nil)
    {
        rethrowCxxException(exception, true); // True = release the exception.
    }
}

void sent(const Ice::AsyncResultPtr& result)
{
    if(!_sent)
    {
        return;
    }

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSException* exception = nil;
    @try
    {
        _sent(result->sentSynchronously());
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

void (^_completed)(id<ICEInputStream>, BOOL);
void (^_exception)(ICEException*);
void (^_sent)(BOOL);
BOOL _returnsData;

};

class ObjCAMICallbackBase
{
public:

// We must explicitely CFRetain/CFRelease so that the garbage
// collector does not trash the _target.
ObjCAMICallbackBase(id target, SEL ex) : _target(target), _exception(ex)
{
    if(_target)
    {
	CFRetain(_target);
    }
}

virtual ~ObjCAMICallbackBase()
{
    if(_target)
    {
	CFRelease(_target);
    }
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
            NSException* nsex = toObjCException(ex);
            @throw nsex;
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

class ObjCAMICallbackBaseWithSent : virtual public ObjCAMICallbackBase, public Ice::AMISentCallback
{
public:

ObjCAMICallbackBaseWithSent(id target, SEL ex, SEL sent) : ObjCAMICallbackBase(target, ex), _sent(sent)
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

class AMICallback : virtual public ObjCAMICallbackBase, public Ice::AMI_Array_Object_ice_invoke
{
public:

AMICallback(const Ice::CommunicatorPtr& communicator, id target, SEL resp, SEL ex, Class finishedClass, SEL finished) : 
    ObjCAMICallbackBase(target, ex),
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
    ObjCAMICallbackBase::ice_exception(ex);
}

private:

const Ice::CommunicatorPtr _communicator;
SEL _response;
Class _finishedClass;
SEL _finished;
};
typedef IceUtil::Handle<AMICallback> AMICallbackPtr;

class AMICallbackWithSent : public ObjCAMICallbackBaseWithSent, public AMICallback
{
public:
    
AMICallbackWithSent(const Ice::CommunicatorPtr& communicator, id target, SEL resp, SEL ex, SEL sent, 
                    Class finishedClass ,SEL finished) : 
    ObjCAMICallbackBase(target, ex),
    ObjCAMICallbackBaseWithSent(target, ex, sent),
    AMICallback(communicator, target, resp, ex, finishedClass, finished)
{
}
};

class AMIIceInvokeCallback : virtual public ObjCAMICallbackBase, public Ice::AMI_Array_Object_ice_invoke
{
public:
    
AMIIceInvokeCallback(id target, SEL response, SEL ex) : ObjCAMICallbackBase(target, ex), _response(response)
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
    ObjCAMICallbackBase::ice_exception(ex);
}

private:

SEL _response;

};

class AMIIceInvokeCallbackWithSent : public ObjCAMICallbackBaseWithSent, public AMIIceInvokeCallback
{
public:
    
AMIIceInvokeCallbackWithSent(id target, SEL response, SEL ex, SEL sent) :
    ObjCAMICallbackBase(target, ex),
    ObjCAMICallbackBaseWithSent(target, ex, sent),
    AMIIceInvokeCallback(target, response, ex)
{
}

};

class AMIIceFlushBatchRequestsCallback : virtual public ObjCAMICallbackBase, public Ice::AMI_Object_ice_flushBatchRequests
{
public:

AMIIceFlushBatchRequestsCallback(id target, SEL ex) : ObjCAMICallbackBase(target, ex)
{
}

virtual void ice_exception(const Ice::Exception& ex)
{
    ObjCAMICallbackBase::ice_exception(ex);
}

};

class AMIIceFlushBatchRequestsCallbackWithSent : public ObjCAMICallbackBaseWithSent, public AMIIceFlushBatchRequestsCallback
{
public:

AMIIceFlushBatchRequestsCallbackWithSent(id target, SEL ex, SEL sent) :
    ObjCAMICallbackBase(target, ex),
    ObjCAMICallbackBaseWithSent(target, ex, sent),
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

@implementation ICEAsyncResult (ICEInternal)
-(ICEAsyncResult*) initWithAsyncResult__:(const Ice::AsyncResultPtr&)arg operation:(NSString*)op
{
    if(![super init])
    {
        return nil;
    }

    asyncResult__ = arg.get();
    ASYNCRESULT->__incRef();
    operation_ = [op retain];
    return self;
}

-(Ice::AsyncResult*) asyncResult__
{
    return ASYNCRESULT;
}

-(void) dealloc
{
    ASYNCRESULT->__decRef();
    asyncResult__ = 0;
    [operation_ release];
    [super dealloc];
}

-(void) finalize
{
    ASYNCRESULT->__decRef();
    asyncResult__ = 0;
    [super finalize];
}

+(ICEAsyncResult*) asyncResultWithAsyncResult__:(const Ice::AsyncResultPtr&)arg
{
    return [self asyncResultWithAsyncResult__:arg operation:nil];
}
+(ICEAsyncResult*) asyncResultWithAsyncResult__:(const Ice::AsyncResultPtr&)arg operation:(NSString*)op
{
    if(!arg)
    {
        return nil;
    }
    else
    {
        return [[[self alloc] initWithAsyncResult__:arg operation:op] autorelease];
    }
}
-(NSString*) operation
{
    return operation_;
}
@end

@implementation ICEAsyncResult

-(id<ICECommunicator>) getCommunicator
{
    return [ICECommunicator wrapperWithCxxObject:ASYNCRESULT->getCommunicator().get()];
}

-(id<ICEConnection>) getConnection
{
    return [ICEConnection wrapperWithCxxObject:ASYNCRESULT->getConnection().get()];
}

-(id<ICEObjectPrx>) getProxy
{
    // XXX
    //return [ICEObjectPrx wrapperWithCxxObject:ASYNCRESULT->getProxy().get()];
    return nil;
}

-(BOOL) isCompleted
{
    return ASYNCRESULT->isCompleted();
}

-(void) waitForCompleted
{
    ASYNCRESULT->waitForCompleted();
}

-(BOOL) isSent
{
    return ASYNCRESULT->isSent();
}

-(void) waitForSent
{
    ASYNCRESULT->waitForSent();
}

-(BOOL) sentSynchronously
{
    return ASYNCRESULT->sentSynchronously();
}

-(NSString*) getOperation
{
    if(operation_ != nil)
    {
        return [[operation_ retain] autorelease];
    }
    else
    {
        return [toNSString(ASYNCRESULT->getOperation()) autorelease];
    }
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

-(void) finalize
{
    OBJECTPRX->__decRef();
    objectPrx__ = 0;
    [super finalize];
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
+(ICEInt) minWireSize
{
    return 2;
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
    NSException* nsex = nil;
    try
    {
        Ice::OutputStreamPtr os = Ice::createOutputStream(OBJECTPRX->ice_getCommunicator());
        return [ICEOutputStream wrapperWithCxxObjectNoAutoRelease:os.get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}
-(void) checkTwowayOnly__:(NSString*)operation
{
    if(!OBJECTPRX->ice_isTwoway())
    {
        @throw [ICETwowayOnlyException twowayOnlyException:__FILE__ line:__LINE__ operation:operation];
    }
}
-(void) checkAsyncTwowayOnly__:(NSString*)operation
{
    //
    // No mutex lock necessary, there is nothing mutable in this
    // operation.
    //
    
    if(![self ice_isTwoway])
    {
        @throw [NSException exceptionWithName:NSInvalidArgumentException 
                            reason:[NSString stringWithFormat:@"`%@' can only be called with a twoway proxy", operation]
                            userInfo:nil];
    }
}

-(void) invoke__:(NSString*)operation 
            mode:(ICEOperationMode)mode 
         marshal:(ICEMarshalCB)marshal 
       unmarshal:(ICEUnmarshalCB)unmarshal
         context:(ICEContext*)context
{
    if(unmarshal && !OBJECTPRX->ice_isTwoway())
    {
        @throw [ICETwowayOnlyException twowayOnlyException:__FILE__ line:__LINE__ operation:operation];
    }

    id<ICEOutputStream> os = nil;
    if(marshal)
    {
        os = [self createOutputStream__];
        @try
        {
            marshal(os);
        }
        @catch(NSException* ex)
        {
            [os release];
            @throw ex;
        }
    }

    BOOL ok = YES; // Keep the compiler happy.
    ICEInputStream* is = nil;
    NSException* nsex = nil;
    try
    {
        std::vector<Ice::Byte> inParams;
        if(os)
        {
            [(ICEOutputStream*)os os]->finished(inParams);
            [os release];
            os = nil;
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

        if(unmarshal)
        {
            Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), outParams);
            is = [ICEInputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
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
        if(os != nil)
        {
            [os release];
            os = nil;
        }
        if(is != nil)
        {
            [is release];
            is = nil;
        }
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }

    NSAssert(!os, @"output stream not cleared");
    if(is)
    {
        @try
        {
            unmarshal(is, ok);
        }
        @finally
        {
            [is release];
        }
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
    NSException* nsex = nil;
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
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return NO; // Keep the compiler happy.
}

-(ICEAsyncResult*) begin_invoke__:(NSString*)operation 
                             mode:(ICEOperationMode)mode 
                          marshal:(void(^)(id<ICEOutputStream>))marshal
                      returnsData:(BOOL)returnsData
                          context:(ICEContext*)context
{
    if(returnsData)
    {
        [self checkAsyncTwowayOnly__:operation];
    }

    id<ICEOutputStream> os = nil;
    if(marshal)
    {
        os = [self createOutputStream__];
        @try
        {
            marshal(os);
        }
        @catch(NSException* ex)
        {
            [os release];
            @throw ex;
        }
    }

    NSException* nsex = nil;
    try
    {
        std::vector<Ice::Byte> inParams;
        if(os)
        {
            [(ICEOutputStream*)os os]->finished(inParams);
            [os release];
            os = nil;
        }

        Ice::AsyncResultPtr r;
        if(context != nil)
        {
            Ice::Context ctx;
            fromNSDictionary(context, ctx);
            r = OBJECTPRX->begin_ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inParams, ctx);
        }
        else
        {
            r = OBJECTPRX->begin_ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inParams);
        }
        return [ICEAsyncResult asyncResultWithAsyncResult__:r operation:operation];
    }
    catch(const std::exception& ex)
    {
        if(os != nil)
        {
            [os release];
            os = nil;
        }
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
    return nil; // Keep the compiler happy.
}

-(ICEAsyncResult*) begin_invoke__:(NSString*)operation 
                             mode:(ICEOperationMode)mode 
                          marshal:(void(^)(id<ICEOutputStream>))marshal
                      returnsData:(BOOL)returnsData
                        completed:(void(^)(id<ICEInputStream>, BOOL))completed
                        exception:(void(^)(ICEException*))exception 
                             sent:(void(^)(BOOL))sent 
                          context:(ICEContext*)context
{
    if(returnsData)
    {
        [self checkAsyncTwowayOnly__:operation];
    }

    id<ICEOutputStream> os = nil;
    if(marshal)
    {
        os = [self createOutputStream__];
        @try
        {
            marshal(os);
        }
        @catch(NSException* ex)
        {
            [os release];
            @throw ex;
        }
    }

    NSException* nsex = nil;
    try
    {
        std::vector<Ice::Byte> inParams;
        if(os)
        {
            [(ICEOutputStream*)os os]->finished(inParams);
            [os release];
            os = nil;
        }

        Ice::CallbackPtr cb = Ice::newCallback(new BeginInvokeAsyncCallback(completed, exception, sent, returnsData), 
                                               &BeginInvokeAsyncCallback::completed,
                                               &BeginInvokeAsyncCallback::sent);
        Ice::AsyncResultPtr r;
        if(context != nil)
        {
            Ice::Context ctx;
            fromNSDictionary(context, ctx);
            r = OBJECTPRX->begin_ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inParams, ctx, cb);
        }
        else
        {
            r = OBJECTPRX->begin_ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inParams, cb);
        }
        return [ICEAsyncResult asyncResultWithAsyncResult__:r operation:operation];
    }
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        if(os != nil)
        {
            [os release];
            os = nil;
        }
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:[toNSString(ex.reason()) autorelease]
                            userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        if(os != nil)
        {
            [os release];
            os = nil;
        }
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
    return nil; // Keep the compiler happy.
}
-(ICEAsyncResult*) begin_invoke__:(NSString*)op
                             mode:(ICEOperationMode)mode 
                          marshal:(void(^)(id<ICEOutputStream>))marshal
                         response:(void(^)())response
                        exception:(void(^)(ICEException*))exception 
                             sent:(void(^)(BOOL))sent 
                          context:(ICEContext*)ctx
{
    void(^completed)(id<ICEInputStream>, BOOL) = ^(id<ICEInputStream>, BOOL) {
        if(response)
        {
            response();   
        }
    };
    return [self begin_invoke__:op mode:mode marshal:marshal returnsData:NO completed:completed exception:exception 
                 sent:sent context:ctx];
}

-(ICEAsyncResult*) begin_invoke__:(NSString*)op
                             mode:(ICEOperationMode)mode 
                          marshal:(void(^)(id<ICEOutputStream>))marshal
                        completed:(void(^)(id<ICEInputStream>, BOOL))completed
                        exception:(void(^)(ICEException*))exception 
                             sent:(void(^)(BOOL))sent 
                          context:(ICEContext*)ctx
{
    return [self begin_invoke__:op mode:mode marshal:marshal returnsData:TRUE completed:completed exception:exception
                 sent:sent context:ctx];
}


-(void)end_invoke__:(NSString*)operation unmarshal:(ICEUnmarshalCB)unmarshal result:(ICEAsyncResult*)result
{
    if(operation != [result operation])
    {
        @throw [NSException exceptionWithName:NSInvalidArgumentException
                            reason:[NSString stringWithFormat:@"Incorrect operation for end_%@ method: %@", 
                                             operation, [result operation]]
                            userInfo:nil];
    }
    if(result == nil)
    {
        @throw [NSException exceptionWithName:NSInvalidArgumentException
                            reason:@"ICEAsyncResult is nil"
                            userInfo:nil];
    }

    BOOL ok = YES; // Keep the compiler happy.
    NSException* nsex = nil;
    ICEInputStream* is = nil;
    try
    {
        std::vector<Ice::Byte> outParams;
        ok = OBJECTPRX->end_ice_invoke(outParams, [result asyncResult__]);

        if(unmarshal)
        {
            Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), outParams);
            is = [ICEInputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
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
    catch(const IceUtil::IllegalArgumentException& ex)
    {
        if(is != nil)
        {
            [is release];
            is = nil;
        }
        nsex = [NSException exceptionWithName:NSInvalidArgumentException reason:[toNSString(ex.reason()) autorelease]
                            userInfo:nil];
    }
    catch(const std::exception& ex)
    {
        if(is != nil)
        {
            [is release];
            is = nil;
        }
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }

    if(is)
    {
        @try
        {
            unmarshal(is, ok);
        }
        @finally
        {
            [is release];
        }
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
    __block BOOL ret__;
    cppCall(^ { ret__ = OBJECTPRX->ice_isA(fromNSString(typeId)); });
    return ret__;
}
-(BOOL) ice_isA:(NSString*)typeId context:(ICEContext*)context
{
    __block BOOL ret__;
    cppCall(^(const Ice::Context& ctx) { ret__ = OBJECTPRX->ice_isA(fromNSString(typeId), ctx); }, context);
    return ret__;
}
-(ICEAsyncResult*) begin_ice_isA:(NSString*)typeId
{
    return beginCppCall(^(Ice::AsyncResultPtr& result)
                        {
                            result = OBJECTPRX->begin_ice_isA(fromNSString(typeId)); 
                        });
}
-(ICEAsyncResult*) begin_ice_isA:(NSString*)typeId context:(ICEContext*)context
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx) 
                        { 
                            result = OBJECTPRX->begin_ice_isA(fromNSString(typeId), ctx); 
                        }, context);
}
-(ICEAsyncResult*) begin_ice_isA:(NSString*)typeId 
                        response:(void(^)(BOOL))response 
                       exception:(void(^)(ICEException*))exception
{
    return [self begin_ice_isA:typeId response:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_isA:(NSString*)typeId 
                         context:(ICEContext*)context 
                        response:(void(^)(BOOL))response 
                       exception:(void(^)(ICEException*))exception 
                         
{
    return [self begin_ice_isA:typeId context:context response:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_isA:(NSString*)typeId 
                        response:(void(^)(BOOL))response 
                       exception:(void(^)(ICEException*))exception 
                            sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_isA(fromNSString(typeId), cb); 
                        }, 
                        ^(const Ice::AsyncResultPtr& result) {
                            BOOL ret__ = OBJECTPRX->end_ice_isA(result);
                            if(response) 
                            {
                                response(ret__);
                            }
                        },
                        exception, sent);

}
-(ICEAsyncResult*) begin_ice_isA:(NSString*)typeId 
                         context:(ICEContext*)context 
                        response:(void(^)(BOOL))response 
                       exception:(void(^)(ICEException*))exception 
                            sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_isA(fromNSString(typeId), ctx, cb); 
                        }, 
                        context,
                        ^(const Ice::AsyncResultPtr& result) {
                            BOOL ret__ = OBJECTPRX->end_ice_isA(result);
                            if(response)
                            {
                                response(ret__);
                            }
                        },
                        exception, sent);

}
-(BOOL) end_ice_isA:(ICEAsyncResult*)result
{
    __block BOOL ret__;
    endCppCall(^(const Ice::AsyncResultPtr& r) { ret__ = OBJECTPRX->end_ice_isA(r); }, result);
    return ret__;
}

-(void) ice_ping
{
    cppCall(^ { OBJECTPRX->ice_ping(); });
}
-(void) ice_ping:(ICEContext*)context
{
    cppCall(^(const Ice::Context& ctx) { OBJECTPRX->ice_ping(ctx); }, context);
}
-(ICEAsyncResult*) begin_ice_ping
{
    return beginCppCall(^(Ice::AsyncResultPtr& result) { result = OBJECTPRX->begin_ice_ping(); } );
}
-(ICEAsyncResult*) begin_ice_ping:(ICEContext*)context
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx) 
                        {
                            result = OBJECTPRX->begin_ice_ping(ctx); 
                        }, context);
}
-(ICEAsyncResult*) begin_ice_ping:(void(^)())response exception:(void(^)(ICEException*))exception
{
    return [self begin_ice_ping:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_ping:(ICEContext*)context
                         response:(void(^)())response 
                        exception:(void(^)(ICEException*))exception 
                          
{
    return [self begin_ice_ping:context response:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_ping:(void(^)())response
                        exception:(void(^)(ICEException*))exception 
                             sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_ping(cb); 
                        }, 
                        ^(const Ice::AsyncResultPtr& result) {
                            OBJECTPRX->end_ice_ping(result);
                            if(response) 
                            {
                                response();
                            }
                        },
                        exception, sent);
}
-(ICEAsyncResult*) begin_ice_ping:(ICEContext*)context 
                         response:(void(^)())response 
                        exception:(void(^)(ICEException*))exception 
                             sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_ping(ctx, cb); 
                        }, 
                        context,
                        ^(const Ice::AsyncResultPtr& result) {
                            OBJECTPRX->end_ice_ping(result);
                            if(response)
                            {
                                response();
                            }
                        },
                        exception, sent);

}
-(void) end_ice_ping:(ICEAsyncResult*)result
{
    endCppCall(^(const Ice::AsyncResultPtr& r) { OBJECTPRX->end_ice_ping(r); }, result);
}

-(NSArray*) ice_ids
{
    __block NSArray* ret__;
    cppCall(^ { ret__ = [toNSArray(OBJECTPRX->ice_ids()) autorelease]; });
    return ret__;
}
-(NSArray*) ice_ids:(ICEContext*)context
{
    __block NSArray* ret__;
    cppCall(^(const Ice::Context& ctx) { ret__ = [toNSArray(OBJECTPRX->ice_ids(ctx)) autorelease]; }, context);
    return ret__;
}
-(ICEAsyncResult*) begin_ice_ids
{
    return beginCppCall(^(Ice::AsyncResultPtr& result) { result = OBJECTPRX->begin_ice_ids(); } );
}
-(ICEAsyncResult*) begin_ice_ids:(ICEContext*)context
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx) 
                        {
                            result = OBJECTPRX->begin_ice_ids(ctx); 
                        }, context);
}
-(ICEAsyncResult*) begin_ice_ids:(void(^)(NSArray*))response exception:(void(^)(ICEException*))exception
{
    return [self begin_ice_ids:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_ids:(ICEContext*)context 
                        response:(void(^)(NSArray*))response
                       exception:(void(^)(ICEException*))exception 
{
    return [self begin_ice_ids:context response:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_ids:(void(^)(NSArray*))response 
                       exception:(void(^)(ICEException*))exception 
                            sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_ids(cb); 
                        }, 
                        ^(const Ice::AsyncResultPtr& result) {
                            NSArray* ret__ = [toNSArray(OBJECTPRX->end_ice_ids(result)) autorelease];
                            if(response) 
                            {
                                response(ret__);
                            }
                        },
                        exception, sent);

}
-(ICEAsyncResult*) begin_ice_ids:(ICEContext*)context 
                        response:(void(^)(NSArray*))response 
                       exception:(void(^)(ICEException*))exception 
                            sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_ids(ctx, cb); 
                        }, 
                        context,
                        ^(const Ice::AsyncResultPtr& result) {
                            NSArray* ret__ = [toNSArray(OBJECTPRX->end_ice_ids(result)) autorelease];
                            if(response)
                            {
                                response(ret__);
                            }
                        },
                        exception, sent);

}
-(NSArray*) end_ice_ids:(ICEAsyncResult*)result
{
    __block NSArray* ret__;
    endCppCall(^(const Ice::AsyncResultPtr& r) { ret__ = [toNSArray(OBJECTPRX->end_ice_ids(r)) autorelease]; }, 
               result);
    return ret__;
}

-(NSString*) ice_id
{
    __block NSString* ret__;
    cppCall(^ { ret__ = [toNSString(OBJECTPRX->ice_id()) autorelease]; });
    return ret__;
}
-(NSString*) ice_id:(ICEContext*)context
{
    __block NSString* ret__;
    cppCall(^(const Ice::Context& ctx) { ret__ = [toNSString(OBJECTPRX->ice_id(ctx)) autorelease]; }, context);
    return ret__;
}
-(ICEAsyncResult*) begin_ice_id
{
    return beginCppCall(^(Ice::AsyncResultPtr& result) { result = OBJECTPRX->begin_ice_id(); } );
}
-(ICEAsyncResult*) begin_ice_id:(ICEContext*)context
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx) 
                        {
                            result = OBJECTPRX->begin_ice_id(ctx); 
                        }, context);
}
-(ICEAsyncResult*) begin_ice_id:(void(^)(NSString*))response exception:(void(^)(ICEException*))exception
{
    return [self begin_ice_id:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_id:(ICEContext*)context 
                       response:(void(^)(NSString*))response 
                      exception:(void(^)(ICEException*))exception 
                        
{
    return [self begin_ice_id:context response:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_id:(void(^)(NSString*))response 
                      exception:(void(^)(ICEException*))exception 
                           sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_id(cb); 
                        }, 
                        ^(const Ice::AsyncResultPtr& result) {
                            NSString* ret__ = [toNSString(OBJECTPRX->end_ice_id(result)) autorelease];
                            if(response) 
                            {
                                response(ret__);
                            }
                        },
                        exception, sent);

}
-(ICEAsyncResult*) begin_ice_id:(ICEContext*)context 
                       response:(void(^)(NSString*))response 
                      exception:(void(^)(ICEException*))exception 
                           sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_id(ctx, cb); 
                        }, 
                        context,
                        ^(const Ice::AsyncResultPtr& result) {
                            NSString* ret__ = [toNSString(OBJECTPRX->end_ice_id(result)) autorelease];
                            if(response)
                            {
                                response(ret__);
                            }
                        },
                        exception, sent);

}
-(NSString*) end_ice_id:(ICEAsyncResult*)result
{
    __block NSString* ret__;
    endCppCall(^(const Ice::AsyncResultPtr& r) { ret__ = [toNSString(OBJECTPRX->end_ice_id(r)) autorelease]; }, 
               result);
    return ret__;
}

-(BOOL) ice_invoke:(NSString*)operation 
              mode:(ICEOperationMode)mode
          inParams:(NSData*)inParams 
         outParams:(NSMutableData**)outParams
{
    __block BOOL ret__;
    cppCall(^ { 
            std::pair<const Ice::Byte*, const Ice::Byte*> inP((ICEByte*)[inParams bytes], 
                                                              (ICEByte*)[inParams bytes] + [inParams length]);
            std::vector<Ice::Byte> outP;
            ret__ = OBJECTPRX->ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inP, outP);
            *outParams = [NSMutableData dataWithBytes:&outP[0] length:outP.size()];
        });
    return ret__;
}

-(BOOL) ice_invoke:(NSString*)operation 
              mode:(ICEOperationMode)mode 
          inParams:(NSData*)inParams 
         outParams:(NSMutableData**)outParams
           context:(ICEContext*)context
{
    __block BOOL ret__;
    cppCall(^(const Ice::Context& ctx) {
            std::pair<const Ice::Byte*, const Ice::Byte*> inP((ICEByte*)[inParams bytes], 
                                                              (ICEByte*)[inParams bytes] + [inParams length]);
            std::vector<Ice::Byte> outP;
            ret__ = OBJECTPRX->ice_invoke(fromNSString(operation), (Ice::OperationMode)mode, inP, outP, ctx);
            *outParams = [NSMutableData dataWithBytes:&outP[0] length:outP.size()];
        }, context);
    return ret__;
}
-(ICEAsyncResult*) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams
{
    return beginCppCall(^(Ice::AsyncResultPtr& result)
                        {
                            std::pair<const Ice::Byte*, const Ice::Byte*> 
                                inP((ICEByte*)[inParams bytes], (ICEByte*)[inParams bytes] + [inParams length]);
                            result = OBJECTPRX->begin_ice_invoke(fromNSString(operation), 
                                                                 (Ice::OperationMode)mode, 
                                                                 inP); 
                        });
}
-(ICEAsyncResult*) begin_ice_invoke:(NSString*)operation 
                               mode:(ICEOperationMode)mode 
                           inParams:(NSData*)inParams 
                            context:(ICEContext*)context
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx) 
                        { 
                            std::pair<const Ice::Byte*, const Ice::Byte*> 
                                inP((ICEByte*)[inParams bytes], (ICEByte*)[inParams bytes] + [inParams length]);
                            result = OBJECTPRX->begin_ice_invoke(fromNSString(operation), 
                                                                 (Ice::OperationMode)mode, 
                                                                 inP,
                                                                 ctx); 
                        }, context);
}
-(ICEAsyncResult*) begin_ice_invoke:(NSString*)operation 
                               mode:(ICEOperationMode)mode 
                           inParams:(NSData*)inParams 
                           response:(void(^)(BOOL, NSMutableData*))response 
                          exception:(void(^)(ICEException*))exception
{
    return [self begin_ice_invoke:operation mode:mode inParams:inParams response:response exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_invoke:(NSString*)operation 
                               mode:(ICEOperationMode)mode 
                           inParams:(NSData*)inParams 
                            context:(ICEContext*)context
                           response:(void(^)(BOOL, NSMutableData*))response 
                          exception:(void(^)(ICEException*))exception 
{
    return [self begin_ice_invoke:operation mode:mode inParams:inParams context:context response:response 
                 exception:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_invoke:(NSString*)operation 
                               mode:(ICEOperationMode)mode 
                           inParams:(NSData*)inParams 
                           response:(void(^)(BOOL, NSMutableData*))response 
                          exception:(void(^)(ICEException*))exception 
                               sent:(void(^)(BOOL))sent
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            std::pair<const Ice::Byte*, const Ice::Byte*> 
                                inP((ICEByte*)[inParams bytes], (ICEByte*)[inParams bytes] + [inParams length]);
                            result = OBJECTPRX->begin_ice_invoke(fromNSString(operation), 
                                                                 (Ice::OperationMode)mode, 
                                                                 inP,
                                                                 cb); 
                        },
                        ^(const Ice::AsyncResultPtr& result) {
                            std::pair<const ::Ice::Byte*, const ::Ice::Byte*> outP;
                            BOOL ret__ = OBJECTPRX->___end_ice_invoke(outP, result);
                            NSMutableData* outParams = 
                                [NSMutableData dataWithBytes:outP.first length:(outP.second - outP.first)];
                            if(response) 
                            {
                                response(ret__, outParams);
                            }
                        },
                        exception, sent);

}
-(ICEAsyncResult*) begin_ice_invoke:(NSString*)operation 
                               mode:(ICEOperationMode)mode 
                           inParams:(NSData*)inParams
                            context:(ICEContext*)context
                           response:(void(^)(BOOL, NSMutableData*))response 
                          exception:(void(^)(ICEException*))exception 
                               sent:(void(^)(BOOL))sent 
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::Context& ctx, const Ice::CallbackPtr& cb) 
                        {
                            std::pair<const Ice::Byte*, const Ice::Byte*> 
                                inP((ICEByte*)[inParams bytes], (ICEByte*)[inParams bytes] + [inParams length]);
                            result = OBJECTPRX->begin_ice_invoke(fromNSString(operation), 
                                                                 (Ice::OperationMode)mode, 
                                                                 inP,
                                                                 ctx, 
                                                                 cb); 
                        }, 
                        context,
                        ^(const Ice::AsyncResultPtr& result) {
                            std::pair<const ::Ice::Byte*, const ::Ice::Byte*> outP;
                            BOOL ret__ = OBJECTPRX->___end_ice_invoke(outP, result);
                            NSMutableData* outParams = 
                                [NSMutableData dataWithBytes:outP.first length:(outP.second - outP.first)];
                            if(response)
                            {
                                response(ret__, outParams);
                            }
                        },
                        exception, sent);
}
-(BOOL) end_ice_invoke:(NSMutableData**)outParams result:(ICEAsyncResult*)result
             
{
    __block BOOL ret__;
    endCppCall(^(const Ice::AsyncResultPtr& r) 
               {
                   std::vector<Ice::Byte> outP;
                   ret__ = OBJECTPRX->end_ice_invoke(outP, r); 
                   *outParams = [NSMutableData dataWithBytes:&outP[0] length:outP.size()];
               }, result);
    return ret__;
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
    NSException* nsex = nil;
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
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return NO; // Keep the compiler happy.
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
    NSException* nsex = nil;
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
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return NO; // Keep the compiler happy.
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
    NSException* nsex = nil;
    try
    {
        return [ICEConnection wrapperWithCxxObject:OBJECTPRX->ice_getConnection().get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}
-(id<ICEConnection>) ice_getCachedConnection
{
    NSException* nsex = nil;
    try
    {
        return [ICEConnection wrapperWithCxxObject:OBJECTPRX->ice_getCachedConnection().get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}
-(void) ice_flushBatchRequests
{
    NSException* nsex = nil;
    try
    {
        OBJECTPRX->ice_flushBatchRequests();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    if(nsex != nil)
    {
        @throw nsex;
    }
}
-(BOOL) ice_flushBatchRequests_async:(id)target exception:(SEL)exception
{
    return [self ice_flushBatchRequests_async:target exception:exception sent:nil];
}
-(BOOL) ice_flushBatchRequests_async:(id)target exception:(SEL)exception sent:(SEL)sent
{
    NSException* nsex = nil;
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
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return NO; // Keep the compiler happy.
}
-(ICEAsyncResult*) begin_ice_flushBatchRequests
{
    return beginCppCall(^(Ice::AsyncResultPtr& result) 
                        {
                            result = OBJECTPRX->begin_ice_flushBatchRequests(); 
                        });
}
-(ICEAsyncResult*) begin_ice_flushBatchRequests:(void(^)(ICEException*))exception
{
    return [self begin_ice_flushBatchRequests:exception sent:nil];
}
-(ICEAsyncResult*) begin_ice_flushBatchRequests:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent 
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            result = OBJECTPRX->begin_ice_flushBatchRequests(cb); 
                        }, 
                        ^(const Ice::AsyncResultPtr& result) {
                            OBJECTPRX->end_ice_flushBatchRequests(result);
                        },
                        exception, sent);
}
-(void) end_ice_flushBatchRequests:(ICEAsyncResult*)result
{
    endCppCall(^(const Ice::AsyncResultPtr& r) 
               {
                   OBJECTPRX->end_ice_flushBatchRequests(r); 
               }, result);
}
@end
