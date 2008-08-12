// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/ObjectI.h>
#import <IceObjC/StreamI.h>
#import <IceObjC/CurrentI.h>
#import <IceObjC/Util.h>
#import <IceObjC/LocalException.h>

#include <Ice/Object.h>
#include <Ice/IncomingAsync.h>
#include <Ice/Initialize.h>
#include <Ice/ObjectAdapter.h>

#define OBJECT ((Ice::Object*)object__)

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
        is = [[ICEInputStream alloc] initWithInputStream:s];
    }
    {
        Ice::OutputStreamPtr s = Ice::createOutputStream(current.adapter->getCommunicator());
        os = [[ICEOutputStream alloc] initWithOutputStream:s];
    }
    ICECurrent* c = [[ICECurrent alloc] initWithCurrent:current];
    BOOL ok;
    @try
    {
        ok = [_object dispatch__:c is:is os:os];
    }
    @catch(NSException* ex)
    {
        [os release];
        rethrowCxxException(ex);
    }
    @finally
    {
        [c release];
        [is release];
    }
    
    std::vector<Ice::Byte> outParams;
    [os os__]->finished(outParams);
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
    return [[[self alloc] initWithObject__:arg] autorelease];
}
@end

@implementation ICEObject

static const char* ICEObject_ids__[] =
{
    "::Ice::Object"
};

-(BOOL) ice_isA___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    NSString* id__ = [is readString];
    BOOL ret__ = [(id)self ice_isA:id__ current:current];
    [os writeBool:ret__];
    return YES;
}
-(BOOL) ice_ping___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [(id)self ice_ping:current];
    return YES;
}
-(BOOL) ice_id___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    NSString* ret__ = [(id)self ice_id:current];
    [os writeString:ret__];
    return YES;
}
-(BOOL) ice_ids___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    NSArray* ret__ = [(id)self ice_ids:current];
    [os writeStringSeq:ret__];
    return YES;
}
-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current
{
    try
    {
        return OBJECT->ice_isA(fromNSString(typeId));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}
-(void) ice_ping:(ICECurrent*)current
{
    try
    {
        OBJECT->ice_ping();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSString*) ice_id:(ICECurrent*)current
{
    return [NSString stringWithUTF8String:ICEObject_ids__[0]];
}
-(NSArray*) ice_ids:(ICECurrent*)current
{
    try
    {
        return [toNSArray(ICEObject_ids__, sizeof(ICEObject_ids__) / sizeof(const char*)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return NO; // Keep the compiler happy.
    }
}
+(const char*) ice_staticId
{
    return ICEObject_ids__[0];
}
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    //
    // TODO: Optimize
    //

    if([[current operation] isEqualToString:@"ice_isA"])
    {
        return [self ice_isA___:current is:is os:os];
    }
    else if([[current operation] isEqualToString:@"ice_ping"])
    {
        return [self ice_ping___:current is:is os:os];
    }
    else if([[current operation] isEqualToString:@"ice_id"])
    {
        return [self ice_id___:current is:is os:os];
    }
    else if([[current operation] isEqualToString:@"ice_ids"])
    {
        return [self ice_ids___:current is:is os:os];
    }
    else
    {
        // TODO: throw operation not exist exception.
    }
    return YES;
}

-(void) write__:(id<ICEOutputStream>)os
{
    [os writeTypeId:[ICEObject ice_staticId]];
    [os startSlice];
    [os writeSize:0]; // For compatibility with the old AFM.
    [os endSlice];
}
-(void) read__:(id<ICEInputStream>)is readTypeId:(BOOL)rid
{
    if(rid)
    {
        [is readTypeId];
    }

    [is startSlice];

    // For compatibility with the old AFM.
    ICEInt sz = [is readSize];
    if(sz != 0)
    {
        @throw [ICEMarshalException localException:__FILE__ line:__LINE__];
    }

    [is endSlice];
}
@end
