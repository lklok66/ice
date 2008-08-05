// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/ObjectI.h>

#include <Ice/Object.h>

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
//     Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), inParams);
//     ICEInputStream* is = [[ICEInputStream alloc] initWithInputStream:s.get()];
//     ICEOutputStream* os;
//     @try
//     {
//         BOOL ok = [_servant dispatch__:current.operation.c_str() is:is os:&os];
//         std::vector<Ice::Byte> outParams;
//         [os os__]->finished(outParams);
//         [os release];
//         cb->ice_response(ok, outParams);
//     } 
//     catch(const std::exception& ex)
//     {
//         rethrowObjCException(ex);
//     }
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
-(BOOL) ice_isA:(NSString*)typeId
{
    return OBJECT->ice_isA([typeId UTF8String]);
}
-(void) ice_ping
{
    OBJECT->ice_ping();
}

@end
