// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Object.h>

#include <Ice/Object.h>

namespace IceObjC
{

class ServantWrapper : virtual public Ice::Object
{
public:

    virtual Ice_Object* getServant() = 0;
};
typedef IceUtil::Handle<ServantWrapper> ServantWrapperPtr;

};

@interface Ice_Object (Internal)
-(Ice_Object*)initWithObject__:(const Ice::ObjectPtr&)arg;
-(Ice::Object*) object__;
+(Ice_Object*)objectWithObject__:(const Ice::ObjectPtr&)arg;
@end
