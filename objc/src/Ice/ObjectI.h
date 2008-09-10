// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Object.h>

#include <IceCpp/Object.h>

//
// Forward declarations.
//
@class ICECurrent;
@class ICEInputStream;
@class ICEOutputStream;

namespace IceObjC
{

class ObjectWrapper : virtual public Ice::Object
{
public:

    virtual ~ObjectWrapper() { }
    virtual ICEObject* getObject() = 0;
};
typedef IceUtil::Handle<ObjectWrapper> ObjectWrapperPtr;

};

@interface ICEObject (ICEInternal)
-(Ice::Object*) object__;
@end
