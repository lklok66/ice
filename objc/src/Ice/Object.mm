// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ObjectI.h>
#import <Ice/StreamI.h>
#import <Ice/CurrentI.h>
#import <Ice/Util.h>
#import <Ice/LocalException.h>

#include <IceCpp/Object.h>
#include <IceCpp/IncomingAsync.h>
#include <IceCpp/Initialize.h>
#include <IceCpp/ObjectAdapter.h>

#import <Foundation/NSAutoreleasePool.h>

#define OBJECT ((Ice::Object*)object__)

int
ICELookupString(const char** array, size_t count, const char* str)
{
    //
    // TODO: Optimize to do a binary search.
    //
    for(size_t i = 0; i < count; i++)
    {
        if(strcmp(array[i], str) == 0)
        {
            return i;
        }
    }
    return -1;
}

static NSString*
operationModeToString(ICEOperationMode mode)
{
    switch(mode)
    {
    case ICENormal:
        return @"::Ice::Normal";

    case ICENonmutating:
        return @"::Ice::Nonmutating";

    case ICEIdempotent:
        return @"::Ice::Idempotent";

    default:
        return [NSString stringWithFormat:@"unknown value(%d)", mode];
    }
}

namespace IceObjC
{

class ObjectI : public ObjectWrapper, public Ice::BlobjectArrayAsync
{
public:

    ObjectI(id<ICEObject>);
    virtual ~ObjectI();

    virtual void ice_invoke_async(const Ice::AMD_Array_Object_ice_invokePtr&, 
                                  const std::pair<const Ice::Byte*, const Ice::Byte*>&,
                                  const Ice::Current&);

    virtual ICEObject* getObject();

private:

    ICEObject* _object;
};

}

IceObjC::ObjectI::ObjectI(id<ICEObject> object) : _object((ICEObject*)object)
{
}

IceObjC::ObjectI::~ObjectI()
{
}
    
void
IceObjC::ObjectI::ice_invoke_async(const Ice::AMD_Array_Object_ice_invokePtr& cb, 
                                   const std::pair<const Ice::Byte*, const Ice::Byte*>& inParams,
                                   const Ice::Current& current)
{
    ICEInputStream* is = nil;
    ICEOutputStream* os = nil;
    {
        Ice::InputStreamPtr s = Ice::createInputStream(current.adapter->getCommunicator(), inParams);
        is = [ICEInputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
    }
    {
        Ice::OutputStreamPtr s = Ice::createOutputStream(current.adapter->getCommunicator());
        os = [ICEOutputStream wrapperWithCxxObjectNoAutoRelease:s.get()];
    }

    ICECurrent* c = [[ICECurrent alloc] initWithCurrent:current];
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    BOOL ok;
    @try
    {
        ok = [_object dispatch__:c is:is os:os];
    }
    @catch(NSException* ex)
    {
        [c release];
        [is release];
        [os release];
        try
        {
            rethrowCxxException(ex);
        }
        catch(const std::exception&)
        {
            [pool release];
            throw;
        }
    }
    [pool release];
    [c release];
    [is release];
    
    std::vector<Ice::Byte> outParams;
    [os os]->finished(outParams);
    [os release];

    cb->ice_response(ok, std::make_pair(&outParams[0], &outParams[0] + outParams.size()));
}

ICEObject*
IceObjC::ObjectI::getObject()
{
    return _object;
}

@implementation ICEObject (Internal)

-(ICEObject*)init
{
    if(![super init])
    {
        return nil;
    }
    object__ = new IceObjC::ObjectI(self);
    OBJECT->__incRef();
    return self;
}

-(ICEObject*)initWithObject__:(const Ice::ObjectPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    object__ = arg.get();
    OBJECT->__incRef();
    return self;
}

-(Ice::Object*) object__
{
    return (Ice::Object*)object__;
}

-(void) dealloc
{
    OBJECT->__decRef();
    object__ = 0;
    [super dealloc];
}

+(ICEObject*)objectWithObject__:(const Ice::ObjectPtr&)arg
{
    if(!arg)
    {
        return nil;
    }
    else
    {
        return [[[self alloc] initWithObject__:arg] autorelease];
    }
}
@end

@implementation ICEObject

static const char* ICEObject_ids__[1] =
{
    "::Ice::Object"
};

static const char* ICEObject_all__[4] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

-(void) checkModeAndSelector__:(ICEOperationMode)expected selector:(SEL)sel current:(ICECurrent*)current
{
    ICEOperationMode received = current.mode;
    if(expected != received)
    {
        if(expected == ICEIdempotent && received == ICENonmutating)
        {
            // 
            // Fine: typically an old client still using the deprecated nonmutating keyword
            //
            
            //
            // Note that expected == Nonmutating and received == Idempotent is not ok:
            // the server may still use the deprecated nonmutating keyword to detect updates
            // and the client should not break this (deprecated) feature.
            //
        }
        else
        {
            ICEMarshalException* ex = [ICEMarshalException marshalException:__FILE__ line:__LINE__];
            [ex setReason_:[NSString stringWithFormat:@"unexpected operation mode. expected = %@ received=%@", 
                                     operationModeToString(expected), operationModeToString(received)]]; 
            @throw ex;
        }
    }

    if(![self respondsToSelector:sel])
    {
        @throw [ICEOperationNotExistException requestFailedException:__FILE__ 
                                              line:__LINE__ 
                                              id_:current.id_ 
                                              facet:current.facet
                                              operation:current.operation];
    }
}

+(BOOL) ice_isA___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    NSString* id__ = [[is readString] autorelease];
    BOOL ret__ = [servant ice_isA:id__ current:current];
    [os writeBool:ret__];
    return YES;
}

+(BOOL) ice_ping___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [servant ice_ping:current];
    return YES;
}

+(BOOL) ice_id___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    NSString* ret__ = [servant ice_id:current];
    [os writeString:ret__];
    return YES;
}

+(BOOL) ice_ids___:(id)servant current:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    NSArray* ret__ = [servant ice_ids:current];
    [os writeStringSeq:ret__];
    return YES;
}

-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current
{
    int count, index;
    const char** staticIds = [[self class] staticIds__:&count idIndex:&index];
    return ICELookupString(staticIds, count, [typeId UTF8String]) >= 0;
}

-(void) ice_ping:(ICECurrent*)current
{
    // Nothing to do.
}

-(NSString*) ice_id:(ICECurrent*)current
{
    return [[self class] ice_staticId];
}

-(NSArray*) ice_ids:(ICECurrent*)current
{
    try
    {
        int count, index;
        const char** staticIds = [[self class] staticIds__:&count idIndex:&index];
        return [toNSArray(staticIds, count) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}

+(NSString*) ice_staticId
{
    int count, index;
    const char** staticIds = [self staticIds__:&count idIndex:&index];
    return [NSString stringWithUTF8String:staticIds[index]];
}

+(const char**) staticIds__:(int*)count idIndex:(int*)idx
{
    *count = sizeof(ICEObject_ids__) / sizeof(const char*);
    *idx = 0;
    return ICEObject_ids__;
}

-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    switch(ICELookupString(ICEObject_all__, 
                           sizeof(ICEObject_all__) / sizeof(const char*),
                           [[current operation] UTF8String]))
    {
    case 0:
        return [ICEObject ice_id___:self current:current is:is os:os];
    case 1:
        return [ICEObject ice_ids___:self current:current is:is os:os];
    case 2:
        return [ICEObject ice_isA___:self current:current is:is os:os];
    case 3:
        return [ICEObject ice_ping___:self current:current is:is os:os];
    default:
        @throw [ICEOperationNotExistException requestFailedException:__FILE__ 
                                              line:__LINE__ 
                                              id_:current.id_ 
                                              facet:current.facet
                                              operation:current.operation];
    }
}

-(void) write__:(id<ICEOutputStream>)os
{
    [os writeTypeId:ICEObject_ids__[0]];
    [os startSlice];
    [os writeSize:0]; // For compatibility with the old AFM.
    [os endSlice];
}

-(void) read__:(id<ICEInputStream>)is readTypeId:(BOOL)rid
{
    if(rid)
    {
        [[is readTypeId] release];
    }

    [is startSlice];

    // For compatibility with the old AFM.
    ICEInt sz = [is readSize];
    if(sz != 0)
    {
        @throw [ICEMarshalException marshalException:__FILE__ line:__LINE__];
    }

    [is endSlice];
}

-(id) copyWithZone:(NSZone*)zone
{
    NSAssert(NO, @"copyWithZone must be overriddent");
    return nil;
}
@end
