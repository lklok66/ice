// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSException.h>

#import <IceObjC/ObjectI.h>
#import <IceObjC/StreamI.h>
#import <IceObjC/CurrentI.h>

#include <Ice/Object.h>
#include <Ice/IncomingAsync.h>
#include <Ice/Initialize.h>
#include <Ice/ObjectAdapter.h>

#define OBJECT ((Ice::Object*)object__)

namespace IceObjC
{

class BlobjectI : public ServantWrapper, public Ice::BlobjectArrayAsync
{
public:

BlobjectI(id<ICEObject> servant) : _servant((ICEObject*)servant)
{
}
    
virtual void
ice_invoke_async(const Ice::AMD_Array_Object_ice_invokePtr& cb, 
                 const std::pair<const Ice::Byte*, const Ice::Byte*>& inParams,
                 const Ice::Current& current)
{
    Ice::InputStreamPtr s = Ice::createInputStream(current.adapter->getCommunicator(), inParams);
    ICEInputStream* is = [[ICEInputStream alloc] initWithInputStream:s];
    ICEOutputStream* os;
    @try
    {
        ICECurrent* c = [[ICECurrent alloc] initWithCurrent:current]; // TODO: create from cached current.
        BOOL ok = [_servant dispatch__:c is:is os:&os];
        [c release];
        std::vector<Ice::Byte> outParams;
        [os os__]->finished(outParams);
        [os release];
        cb->ice_response(ok, std::make_pair(&outParams[0], &outParams[0] + outParams.size()));
    } 
    @catch(NSException* ex)
    {
        //rethrowCxxException(ex);
    }
}

virtual ICEObject*
getServant()
{
    return _servant;
}

private:

ICEObject* _servant;
    
};

}

@implementation ICEObject (Internal)

-(ICEObject*)init
{
    if(![super init])
    {
        return nil;
    }
    object__ = new IceObjC::BlobjectI(self);
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

-(ICEObject*)servant__
{
    return self;
}

-(ICEOutputStream*) createOutputStream__:(ICECurrent*)current
{
    Ice::OutputStreamPtr os = Ice::createOutputStream([current current__]->adapter->getCommunicator());
    return [[ICEOutputStream alloc] initWithOutputStream:os.get()];
}

-(BOOL) ice_isA___:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream**)os
{
    NSString* __id = [is readString];
    [is release];
    BOOL __ret = [(id)self ice_isA:__id current:current];
    *os = [self createOutputStream__:current];
    [*os writeBool:__ret];
    return TRUE;
}
-(BOOL) ice_ping___:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream**)os
{
    [is release];
    [(id)self ice_ping:current];
    *os = [self createOutputStream__:current];
    return TRUE;
}
-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current
{
    return OBJECT->ice_isA([typeId UTF8String]);
}
-(void) ice_ping:(ICECurrent*)current
{
    OBJECT->ice_ping();
}
-(BOOL) dispatch__:(ICECurrent*)current is:(ICEInputStream*)is os:(ICEOutputStream**)os
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
    else
    {
        // TODO: throw operation not exist exception.
    }
    return TRUE;
}

@end
