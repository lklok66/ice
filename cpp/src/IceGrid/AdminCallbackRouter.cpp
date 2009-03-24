// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceGrid/AdminCallbackRouter.h>

using namespace Ice;
using namespace std;

namespace
{

class AMICallback : public AMI_Array_Object_ice_invoke
{
public:

    AMICallback(const AMD_Array_Object_ice_invokePtr& cb) :
        _cb(cb)
    {
    }

    virtual void ice_response(bool ok, const std::pair<const Byte*, const Byte*>& outParams)
    {
        _cb->ice_response(ok, outParams);
    }
    
    virtual void ice_exception(const Ice::Exception&)
    {
        _cb->ice_exception(ObjectNotExistException(__FILE__, __LINE__)); // Callback object is unreachable.
    }
    
private:
    AMD_Array_Object_ice_invokePtr _cb;
};

}


void
IceGrid::AdminCallbackRouter::addMapping(const string& category, const ConnectionPtr& con)
{
    IceUtil::Mutex::Lock sync(_mutex);

#ifndef NDEBUG
    bool inserted =
#endif
        _categoryToConnection.insert(map<string, ConnectionPtr>::value_type(category, con)).second;
    
    assert(inserted == true);
}

void
IceGrid::AdminCallbackRouter::removeMapping(const string& category)
{
    IceUtil::Mutex::Lock sync(_mutex);

#ifndef NDEBUG
    size_t one =
#endif
        _categoryToConnection.erase(category);

    assert(one == 1);
}


void
IceGrid::AdminCallbackRouter::ice_invoke_async(const AMD_Array_Object_ice_invokePtr& cb, 
                                               const pair<const Byte*, const Byte*>& inParams,
                                               const Current& current)
{
    ConnectionPtr con;

    {
        IceUtil::Mutex::Lock sync(_mutex);
        map<string, ConnectionPtr>::iterator p = _categoryToConnection.find(current.id.category);
        if(p == _categoryToConnection.end())
        {
            throw ObjectNotExistException(__FILE__, __LINE__);
        }
        con = p->second;
    }

  
    ObjectPrx target = con->createProxy(current.id)->ice_facet(current.facet);
    
        
    //
    // Call with AMI
    //
    target->ice_invoke_async(new AMICallback(cb), current.operation, current.mode, inParams, current.ctx);
}

