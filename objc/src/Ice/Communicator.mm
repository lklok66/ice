// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/CommunicatorI.h>
#import <Ice/PropertiesI.h>
#import <Ice/ProxyI.h>
#import <Ice/IdentityI.h>
#import <Ice/LoggerI.h>
#import <Ice/ObjectAdapterI.h>
#import <Ice/Router.h>
#import <Ice/Locator.h>
#import <Ice/Util.h>
#import <Ice/ObjectFactory.h>
#import <Ice/StreamI.h>

#include <IceCpp/Router.h>
#include <IceCpp/Locator.h>
#include <IceCpp/ObjectFactory.h>

#import <objc/runtime.h>

#define COMMUNICATOR dynamic_cast<Ice::Communicator*>(static_cast<IceUtil::Shared*>(cxxObject_))

namespace IceObjC
{

class ObjectFactoryI : public Ice::ObjectFactory
{
public:

    // We must explicitely CFRetain/CFRelease so that the garbage
    // collector does not trash the dictionaries.
    ObjectFactoryI(NSDictionary* factories, NSDictionary* prefixTable) : 
        _factories(factories), _prefixTable(prefixTable)
    {
        CFRetain(_factories);
        CFRetain(_prefixTable);
    }

    ~ObjectFactoryI()
    {
        CFRelease(_factories);
        CFRelease(_prefixTable);
    }

    virtual Ice::ObjectPtr
    create(const std::string& type)
    {
        NSString* sliceId = [[NSString alloc] initWithUTF8String:type.c_str()];
        @try
        {
            id<ICEObjectFactory> factory = nil;
            @synchronized(_factories)
            {
                factory = [_factories objectForKey:sliceId];
            }

            ICEObject* obj = nil;
            if(factory != nil)
            {
                obj = [factory create:sliceId];
            }

            if(obj == nil)
            {
                std::string tId = toObjCSliceId(type, _prefixTable);
                Class c = objc_lookUpClass(tId.c_str());
                if(c == nil)
                {
                    return 0; // No object factory.
                }
                if([c isSubclassOfClass:[ICEObject class]])
                {
                    obj = (ICEObject*)[[c alloc] init];
                }
            }

            Ice::ObjectPtr o;
            if(obj != nil)
            {
                o = [ICEInputStream createObjectReader:obj];
                [obj release];
            }
            return o;
        }
        @catch(NSException* ex)
        {
            rethrowCxxException(ex);
        }
        @finally
        {
            [sliceId release];
        }
        return nil; // Keep the compiler happy.
    }

    virtual void
    destroy()
    {
        for(NSString* k in _factories)
        {
            [[_factories objectForKey:k] destroy];
        }
    }

private:
    
    NSDictionary* _factories;
    NSDictionary* _prefixTable;
};

}

@interface ICEInternalPrefixTable(ICEInternal)
+(NSDictionary*) newPrefixTable;
@end

@implementation ICEInternalPrefixTable(ICEInternal)
+(NSDictionary*) newPrefixTable
{
    NSMutableDictionary* prefixTable = [[NSMutableDictionary alloc] init];
    ICEInternalPrefixTable* table = [[ICEInternalPrefixTable alloc] init];
    unsigned int count;
    Method* methods = class_copyMethodList([ICEInternalPrefixTable class], &count);
    for(unsigned int i = 0; i < count; ++i)
    {
        SEL selector = method_getName(methods[i]);
        const char* methodName = sel_getName(selector);
        if(strncmp(methodName, "addPrefixes_C", 13) == 0)
        {
            [table performSelector:selector withObject:prefixTable];
        }
    }
    free(methods);
    [table release];
    return prefixTable;
}
@end

@implementation ICECommunicator
-(void)setup:(NSDictionary*)prefixTable
{
    objectFactories_ = [[NSMutableDictionary alloc] init];
    if(prefixTable)
    {
        prefixTable_ = [prefixTable retain];
    }
    else
    {
        prefixTable_ = [ICEInternalPrefixTable newPrefixTable];
    }
    COMMUNICATOR->addObjectFactory(new IceObjC::ObjectFactoryI(objectFactories_, prefixTable_), "");
}
-(void) dealloc
{
    [prefixTable_ release];
    [objectFactories_ release];
    [super dealloc];
}
-(Ice::Communicator*) communicator
{
    return COMMUNICATOR;
}
-(NSDictionary*) getPrefixTable
{
    return prefixTable_;
}

//
// Methods from @protocol ICECommunicator
//

-(void) destroy
{
    NSException* nsex = nil;
    try
    {
        COMMUNICATOR->destroy();
        return;
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

-(void) shutdown
{
    NSException* nsex = nil;
    try
    {
        COMMUNICATOR->shutdown();
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

-(void) waitForShutdown
{
    NSException* nsex = nil;
    try
    {
        COMMUNICATOR->waitForShutdown();
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

-(BOOL) isShutdown
{
    NSException* nsex = nil;
    try
    {
        return COMMUNICATOR->isShutdown();
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return NO; // Keep the compiler happy.
}

-(id<ICEObjectPrx>) stringToProxy:(NSString*)str
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->stringToProxy(fromNSString(str))];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSString*)proxyToString:(id<ICEObjectPrx>)obj
{
    NSException* nsex = nil;
    try
    {
        return [toNSString(COMMUNICATOR->proxyToString([(ICEObjectPrx*)obj objectPrx__])) autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectPrx>)propertyToProxy:(NSString*)property
{
    NSException* nsex = nil;
    try
    {
        return [ICEObjectPrx objectPrxWithObjectPrx__:COMMUNICATOR->propertyToProxy(fromNSString(property))];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(ICEIdentity*) stringToIdentity:(NSString*)str
{
    NSException* nsex = nil;
    try
    {
        return [ICEIdentity identityWithIdentity:COMMUNICATOR->stringToIdentity(fromNSString(str))];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(NSString*) identityToString:(ICEIdentity*)ident
{
    NSException* nsex = nil;
    try
    {
        return [toNSString(COMMUNICATOR->identityToString([ident identity])) autorelease];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectAdapter>) createObjectAdapter:(NSString*)name;
{
    NSException* nsex = nil;
    try
    {
        ICEObjectAdapter* adapter = [ICEObjectAdapter wrapperWithCxxObject:
                                                          COMMUNICATOR->createObjectAdapter(
                                                              fromNSString(name)).get()];
        return adapter;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectAdapter>) createObjectAdapterWithEndpoints:(NSString*)name endpoints:(NSString*)endpoints;
{
    NSException* nsex = nil;
    try
    {
        ICEObjectAdapter* adapter = [ICEObjectAdapter wrapperWithCxxObject:
                                                          COMMUNICATOR->createObjectAdapterWithEndpoints(
                                                              fromNSString(name), fromNSString(endpoints)).get()];
        return adapter;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICEObjectAdapter>) createObjectAdapterWithRouter:(NSString*)name router:(id<ICERouterPrx>)rtr
{
    NSException* nsex = nil;
    try
    {
        Ice::RouterPrx router = Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__]));
        ICEObjectAdapter* adapter = [ICEObjectAdapter wrapperWithCxxObject:
                                                          COMMUNICATOR->createObjectAdapterWithRouter(
                                                              fromNSString(name), router).get()];
        return adapter;
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}
-(void) addObjectFactory:(id<ICEObjectFactory>)factory sliceId:(NSString*)sliceId
{
    @synchronized(objectFactories_)
    {
        [objectFactories_ setObject:factory forKey:sliceId];
    }
}
-(id<ICEObjectFactory>) findObjectFactory:(NSString*)sliceId
{
    @synchronized(objectFactories_)
    {
        return [objectFactories_ objectForKey:sliceId];
    }
    return nil; // Keep the compiler happy.
}
-(id<ICEProperties>) getProperties
{
    NSException* nsex = nil;
    try
    {
        return [ICEProperties wrapperWithCxxObject:COMMUNICATOR->getProperties().get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICELogger>) getLogger
{
    NSException* nsex = nil;
    try
    {
        return [ICELogger wrapperWithCxxObject:COMMUNICATOR->getLogger().get()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(id<ICERouterPrx>) getDefaultRouter
{
    NSException* nsex = nil;
    try
    {
        return (id<ICERouterPrx>)[ICERouterPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultRouter()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) setDefaultRouter:(id<ICERouterPrx>)rtr
{
    NSException* nsex = nil;
    try
    {
        COMMUNICATOR->setDefaultRouter(Ice::RouterPrx::uncheckedCast(Ice::ObjectPrx([(ICEObjectPrx*)rtr objectPrx__])));
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

-(id<ICELocatorPrx>) getDefaultLocator
{
    NSException* nsex = nil;
    try
    {
        return (id<ICELocatorPrx>)[ICELocatorPrx objectPrxWithObjectPrx__:COMMUNICATOR->getDefaultLocator()];
    }
    catch(const std::exception& ex)
    {
        nsex = toObjCException(ex);
    }
    @throw nsex;
    return nil; // Keep the compiler happy.
}

-(void) setDefaultLocator:(id<ICELocatorPrx>)loc
{
    NSException* nsex = nil;
    try
    {
        COMMUNICATOR->setDefaultLocator(Ice::LocatorPrx::uncheckedCast(
                                            Ice::ObjectPrx([(ICEObjectPrx*)loc objectPrx__])));
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

-(void) flushBatchRequests
{
    NSException* nsex = nil;
    try
    {
        COMMUNICATOR->flushBatchRequests();
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
-(id<ICEAsyncResult>) begin_flushBatchRequests
{
    return beginCppCall(^(Ice::AsyncResultPtr& result) 
                        {
                            result = COMMUNICATOR->begin_flushBatchRequests(); 
                        });
}
-(id<ICEAsyncResult>) begin_flushBatchRequests:(void(^)(ICEException*))exception
{
    return [self begin_flushBatchRequests:exception sent:nil];
}
-(id<ICEAsyncResult>) begin_flushBatchRequests:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent 
{
    return beginCppCall(^(Ice::AsyncResultPtr& result, const Ice::CallbackPtr& cb) 
                        {
                            result = COMMUNICATOR->begin_flushBatchRequests(cb); 
                        }, 
                        ^(const Ice::AsyncResultPtr& result) {
                            COMMUNICATOR->end_flushBatchRequests(result);
                        },
                        exception, sent);
}
-(void) end_flushBatchRequests:(id<ICEAsyncResult>)result
{
    endCppCall(^(const Ice::AsyncResultPtr& r) 
               {
                   COMMUNICATOR->end_flushBatchRequests(r); 
               }, result);
}

@end
