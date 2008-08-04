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

BlobjectI(id<Ice_Object> servant) : _servant((Ice_Object*)servant)
{
}
    
virtual void
ice_invoke_async(const Ice::AMD_Array_Object_ice_invokePtr& cb, 
                 const std::pair<const Ice::Byte*, const Ice::Byte*>& inParams,
                 const Ice::Current& current)
{
//     Ice::InputStreamPtr s = Ice::createInputStream(OBJECTPRX->ice_getCommunicator(), inParams);
//     Ice_InputStream* is = [[Ice_InputStream alloc] initWithInputStream:s.get()];
//     Ice_OutputStream* os;
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

virtual Ice_Object*
getServant()
{
    return _servant;
}

private:

Ice_Object* _servant;
    
};

}

@implementation Ice_Object (Internal)

-(Ice_Object*)init
{
    if(![super init])
    {
        return nil;
    }
    object__ = new IceObjC::BlobjectI(self);
    OBJECT->__incRef();
    return self;
}

-(Ice_Object*)initWithObject__:(const Ice::ObjectPtr&)arg
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

+(Ice_Object*)objectWithObject__:(const Ice::ObjectPtr&)arg
{
    return [[[self alloc] initWithObject__:arg] autorelease];
}
@end

@implementation Ice_Object

-(Ice_Object*)servant__
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
